#include "ModbusTcpSource.h"

#include <QModbusClient>
#include <QModbusTcpClient>
#include <QModbusReply>
#include <QModbusDataUnit>
#include <QModbusDevice>
#include <QHash>
#include <QList>
#include <QVariantList>
#include <QMetaObject>
#include <QDebug>

ModbusTcpSource::ModbusTcpSource(QObject *parent) : DataSource(parent)
{
    m_pollTimer.setInterval(m_intervalMs);
    connect(&m_pollTimer, &QTimer::timeout, this, &ModbusTcpSource::onPollTimer);

    m_client = new QModbusTcpClient(this);
    connect(m_client, &QModbusClient::stateChanged,
            this, &ModbusTcpSource::onStateChanged);

    m_client->setTimeout(3000);
    m_client->setNumberOfRetries(2);
}

ModbusTcpSource::~ModbusTcpSource()
{
    stop();
}

void ModbusTcpSource::setConnection(const QString &host, quint16 port)
{
    m_host = host;
    m_port = port;
}

bool ModbusTcpSource::isConnected() const
{
    return m_client && m_client->state() == QModbusDevice::ConnectedState;
}

void ModbusTcpSource::start()
{
    if (m_client == nullptr) return;

    // ★ 启动前重置守卫
    m_stopping = false;

    m_client->setConnectionParameter(QModbusDevice::NetworkAddressParameter, m_host);
    m_client->setConnectionParameter(QModbusDevice::NetworkPortParameter,    m_port);
    if (!m_client->connectDevice()) {
        const QString reason = tr("connectDevice 失败: %1").arg(m_client->errorString());
        qWarning() << "[ModbusTcpSource]" << reason;
        emit startFailed(reason);   // ★ UI 反馈
        return;
    }
    m_pollTimer.start();
    qDebug() << "[ModbusTcpSource] start, target =" << m_host << ":" << m_port;
}

void ModbusTcpSource::stop()
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

void ModbusTcpSource::onStateChanged()
{
    const auto state = m_client->state();
    qDebug() << "[ModbusTcpSource] state:" << state;
    emit connectionChanged();
}

void ModbusTcpSource::onPollTimer()
{
    if (m_client == nullptr || m_client->state() != QModbusDevice::ConnectedState)
        return;

    QModbusDataUnit unit(QModbusDataUnit::HoldingRegisters, m_baseAddr, m_regCount);
    QModbusReply *reply = m_client->sendReadRequest(unit, 1);
    if (reply == nullptr) {
        qWarning() << "[ModbusTcpSource] sendReadRequest failed:" << m_client->errorString();
        return;
    }
    m_pendingReplies.append(reply);

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
            if (counter[e] % 10 == 1) {
                qWarning() << "[ModbusTcpSource] reply error:" << reply->errorString()
                           << "(累计" << counter[e] << "次)";
            }
            reply->deleteLater();
            return;
        }

        const QModbusDataUnit unit = reply->result();
        QVariantList values;
        if (unit.valueCount() >= m_regCount) {
            // ★ 每个通道只占 1 个 16-bit 寄存器(0~65535)
            // 适配 Modbus Slave 的 Auto Increment(只能按 16-bit 整数 +1)
            // 归一化到 0~100(与 IoSimulator 输出范围一致,SCADA 图元不用改)
            const int nChannels = qMin(m_channelNames.size(), m_regCount);
            values.reserve(nChannels);
            for (int i = 0; i < nChannels; ++i) {
                const quint16 raw = unit.value(i);
                values.append(QVariant(double(raw) / 655.35));   // 0.0 ~ 100.0
            }
        } else {
            qWarning() << "[ModbusTcpSource] incomplete: got"
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
