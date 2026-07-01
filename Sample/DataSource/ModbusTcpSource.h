#ifndef MODBUSTCPSOURCE_H
#define MODBUSTCPSOURCE_H

#include <QObject>
#include <QTimer>
#include <QStringList>
#include <QList>
#include <atomic>
#include "IoSimulator.h"

class QModbusClient;
class QModbusReply;

// Modbus TCP 数据源
// 周期性读取 Holding Registers,每 2 个寄存器解析为一个 IEEE 754 float
// 默认布局:Reg[0..7] 对应 4 通道: temperature/pressure/level/flow
class ModbusTcpSource : public DataSource
{
    Q_OBJECT
public:
    explicit ModbusTcpSource(QObject *parent = nullptr);
    ~ModbusTcpSource() override;

    // 连接配置
    void setConnection(const QString &host, quint16 port);
    QString host() const { return m_host; }
    quint16 port() const { return m_port; }

    // 轮询配置
    void setPollingInterval(int ms) { m_intervalMs = ms; }
    void setRegisterBase(int addr)  { m_baseAddr   = addr; }
    void setRegisterCount(int cnt)  { m_regCount   = cnt; }
    void setChannelNames(const QStringList &names) { m_channelNames = names; }

    void start() override;
    void stop()  override;
    QString name() const override { return "Modbus TCP"; }

    bool isConnected() const;

private slots:
    void onPollTimer();
    void onStateChanged();

private:
    QModbusClient *m_client = nullptr;
    QTimer         m_pollTimer;
    QList<QModbusReply *> m_pendingReplies;   // 待响应的 reply(stop 时清理)
    std::atomic<bool> m_stopping{false};       // ★ 守卫:stop() 后拒绝过期 reply
    int            m_intervalMs = 1000;
    int            m_baseAddr   = 0;
    int            m_regCount   = 4;   // 4 通道 × 1 个 uint16 寄存器(适配 Modbus Slave Auto Increment)
    QString        m_host      = "127.0.0.1";
    quint16        m_port      = 502;
    QStringList    m_channelNames = {"temperature", "pressure", "level", "flow"};
};

#endif // MODBUSTCPSOURCE_H
