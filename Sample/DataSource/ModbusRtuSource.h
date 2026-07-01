#ifndef MODBUSRTUSOURCE_H
#define MODBUSRTUSOURCE_H

#include <QObject>
#include <QTimer>
#include <QStringList>
#include <QList>
#include <atomic>
#include "IoSimulator.h"

class QModbusClient;
class QModbusReply;

// Modbus RTU 串口数据源
// 通过 QModbusRtuSerialMaster 周期性读 Holding Registers
// 配置:串口名(由 VSPD 创建的虚拟串口 COM1/COM2)+ 波特率 + 校验位 等
class ModbusRtuSource : public DataSource
{
    Q_OBJECT
public:
    explicit ModbusRtuSource(QObject *parent = nullptr);
    ~ModbusRtuSource() override;

    // 串口配置
    void setSerialPort(const QString &name) { m_serialPort = name; }
    void setBaudRate(int baud)              { m_baudRate   = baud; }
    void setParity(int parity)              { m_parity     = parity; }  // 0=None 2=Even 3=Odd
    void setSlaveAddress(int addr)          { m_slaveAddr  = addr; }

    // 轮询配置
    void setPollingInterval(int ms) { m_intervalMs = ms; }
    void setRegisterBase(int addr)  { m_baseAddr   = addr; }
    void setRegisterCount(int cnt)  { m_regCount   = cnt; }
    void setChannelNames(const QStringList &names) { m_channelNames = names; }

    void start() override;
    void stop()  override;
    QString name() const override { return "Modbus RTU"; }

    bool isConnected() const;

private slots:
    void onPollTimer();
    void onStateChanged();

private:
    QModbusClient *m_client = nullptr;
    QTimer         m_pollTimer;
    QList<QModbusReply *> m_pendingReplies;
    std::atomic<bool> m_stopping{false};           // ★ 守卫:stop() 后拒绝过期 reply
    int            m_intervalMs = 1000;
    int            m_baseAddr   = 0;
    int            m_regCount   = 4;
    QString        m_serialPort = "COM2";          // 配合 VSPD:COM1↔COM2 对,Sample 用 COM2
    int            m_baudRate   = 9600;
    int            m_parity     = 0;               // 0=None
    int            m_slaveAddr  = 1;
    QStringList    m_channelNames = {"temperature", "pressure", "level", "flow"};
};

#endif // MODBUSRTUSOURCE_H
