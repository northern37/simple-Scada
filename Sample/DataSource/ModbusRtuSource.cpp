#include "ModbusRtuSource.h"

#include <QModbusClient>
#include <QModbusRtuSerialMaster>
#include <QModbusReply>
#include <QModbusDataUnit>
#include <QModbusDevice>
#include <QSerialPortInfo>
#include <QHash>
#include <QList>
#include <QVariantList>
#include <QMetaObject>
#include <QDebug>

ModbusRtuSource::ModbusRtuSource(QObject *parent) : DataSource(parent)
{
    m_pollTimer.setInterval(m_intervalMs);
    connect(&m_pollTimer, &QTimer::timeout, this, &ModbusRtuSource::onPollTimer);

    m_client = new QModbusRtuSerialMaster(this);
    connect(m_client, &QModbusClient::stateChanged,
            this, &ModbusRtuSource::onStateChanged);

    // ★ RTU 专用:不要 retries(避免重复发导致 Slave 状态混乱)
    // ★ RTU 专用:超时延长到 5000ms(9600 波特率下 3.5 字符时间 ≈ 4ms,但 Slave 端处理延迟可能到几百 ms)
    m_client->setTimeout(5000);
    m_client->setNumberOfRetries(0);
}

ModbusRtuSource::~ModbusRtuSource()
{
    stop();
}

bool ModbusRtuSource::isConnected() const
{
    return m_client && m_client->state() == QModbusDevice::ConnectedState;
}

void ModbusRtuSource::start()
{
    if (m_client == nullptr) return;

    // ★ 启动前重置守卫
    m_stopping = false;

    // ★ 自动检测可用串口(防止 VSPD 端口名跟用户预期不一致)
    const auto availablePorts = QSerialPortInfo::availablePorts();
    bool portExists = false;
    for (const auto &info : availablePorts) {
        if (info.portName() == m_serialPort) { portExists = true; break; }
    }
    if (!portExists) {
        const QString reason = tr("串口 %1 不存在").arg(m_serialPort);
        qWarning() << "[ModbusRtuSource]" << reason;
        qWarning() << "[ModbusRtuSource] 当前可用串口:";
        for (const auto &info : availablePorts) {
            qWarning() << "   -" << info.portName()
                       << "(" << info.description() << ")";
        }
        emit startFailed(reason);   // ★ UI 反馈
        return;
    }

    // RTU 参数(区别于 TCP:用 SerialPortNameParameter + 串口参数)
    m_client->setConnectionParameter(QModbusDevice::SerialPortNameParameter, m_serialPort);
    m_client->setConnectionParameter(QModbusDevice::SerialBaudRateParameter, m_baudRate);
    m_client->setConnectionParameter(QModbusDevice::SerialParityParameter,   m_parity);
    m_client->setConnectionParameter(QModbusDevice::SerialDataBitsParameter, 8);
    m_client->setConnectionParameter(QModbusDevice::SerialStopBitsParameter, 1);

    if (!m_client->connectDevice()) {
        const QString reason = tr("connectDevice 失败: %1").arg(m_client->errorString());
        qWarning() << "[ModbusRtuSource]" << reason;
        emit startFailed(reason);   // ★ UI 反馈
        return;
    }
    m_pollTimer.start();
    qDebug() << "[ModbusRtuSource] start, port =" << m_serialPort
             << "baud =" << m_baudRate;
}

void ModbusRtuSource::stop()
{
    // ★ 先置守卫,后续 reply 的 finished lambda 看到 m_stopping=true 就直接 return
    m_stopping = true;
    m_pollTimer.stop();
    for (QModbusReply *r : std::as_const(m_pendingReplies)) {
        if (r) {
            disconnect(r, nullptr, this, nullptr);
            r->deleteLater();
        }
    }
    m_pendingReplies.clear();
    if (m_client) m_client->disconnectDevice();
}

void ModbusRtuSource::onStateChanged()
{
    const auto state = m_client->state();
    qDebug() << "[ModbusRtuSource] state:" << state
             << "error:" << m_client->errorString();
    emit connectionChanged();
}

void ModbusRtuSource::onPollTimer()
{
    if (m_client == nullptr || m_client->state() != QModbusDevice::ConnectedState)
        return;

    QModbusDataUnit unit(QModbusDataUnit::HoldingRegisters, m_baseAddr, m_regCount);
    // ★ RTU slave address 由 m_slaveAddr 决定(TCP 用的是 sendReadRequest 第 2 个参数)
    QModbusReply *reply = m_client->sendReadRequest(unit, m_slaveAddr);
    if (reply == nullptr) {
        qWarning() << "[ModbusRtuSource] sendReadRequest failed:" << m_client->errorString();
        return;
    }
    m_pendingReplies.append(reply);

    // 同样的 QueuedConnection 模式(避免 finished 同步触发死锁)
    connect(reply, &QModbusReply::finished, this, [this, reply]() {
        // ★ 守卫:stop() 后到达的过期 reply 直接丢弃,防止 emit 旧值
        if (m_stopping.load()) {
            return;
        }
        m_pendingReplies.removeAll(reply);
        disconnect(reply, &QModbusReply::finished, this, nullptr);

        if (reply->error() != QModbusDevice::NoError) {
            static QHash<QModbusDevice::Error, int> counter;
            const QModbusDevice::Error e = reply->error();
            counter[e]++;
            // ★ 关键:如果是 ProtocolError(Slave 回复 Exception Frame),打印 Exception Code
            if (e == QModbusDevice::ProtocolError) {
                const QModbusExceptionResponse ex = reply->rawResult();
                qWarning() << "[ModbusRtuSource] Exception code: 0x"
                           << QString::number(ex.exceptionCode(), 16)
                           << "(0x01=IllegalFunction, 0x02=IllegalDataAddress, "
                           << "0x03=IllegalDataValue, 0x04=SlaveDeviceFailure, "
                           << "0x05=Ack, 0x06=SlaveBusy, 0x08=MemoryParity, "
                           << "0x0A=GatewayPath, 0x0B=GatewayResponse)";
            }
            if (counter[e] % 10 == 1) {
                qWarning() << "[ModbusRtuSource] reply error:" << reply->errorString()
                           << "(累计" << counter[e] << "次)";
            }
            reply->deleteLater();
            return;
        }

        const QModbusDataUnit unit = reply->result();
        QVariantList values;
        if (unit.valueCount() >= m_regCount) {
            // 与 ModbusTcpSource 一致:uint16 → 归一化 0~100
            const int nChannels = qMin(m_channelNames.size(), m_regCount);
            values.reserve(nChannels);
            for (int i = 0; i < nChannels; ++i) {
                const quint16 raw = unit.value(i);
                values.append(QVariant(double(raw) / 655.35));
            }
        } else {
            qWarning() << "[ModbusRtuSource] incomplete: got"
                       << unit.valueCount() << "expected" << m_regCount;
        }

        reply->deleteLater();

        if (!values.isEmpty()) {
            QMetaObject::invokeMethod(this, [this, values]() {
                for (int i = 0; i < values.size(); ++i) {
                    emit valueProduced(m_channelNames[i], values[i]);
                }
            }, Qt::QueuedConnection);
        }
    });
}
