#ifndef IOSIMULATOR_H
#define IOSIMULATOR_H

#include <QObject>
#include <QTimer>
#include <QVariant>

// 数据源接口:每个 DataSource 子类只需发 valueProduced(name, value) 信号
class DataSource : public QObject
{
    Q_OBJECT
public:
    explicit DataSource(QObject *parent = nullptr) : QObject(parent) {}
    ~DataSource() override = default;

    virtual void start() = 0;
    virtual void stop()  = 0;
    virtual QString name() const = 0;

signals:
    void valueProduced(const QString &name, const QVariant &value);
    void connectionChanged();    // 数据源连接状态变化(用于状态栏)
    void startFailed(const QString &reason);   // start() 失败(用于状态栏告警)
};

// 模拟数据源:无硬件时,自动生成正弦+噪声数据
// 用于在没串口/没 Modbus 设备/没 MQTT broker 时,验证整个 SCADA 链路
class IoSimulator : public DataSource
{
    Q_OBJECT
public:
    explicit IoSimulator(QObject *parent = nullptr);

    // 模拟通道定义
    struct Channel {
        QString name;
        double  amplitude;   // 振幅
        double  baseline;    // 基线
        double  period;      // 周期(秒)
    };

    void setChannels(const QList<Channel> &chs) { mChannels = chs; }
    void setIntervalMs(int ms) { mIntervalMs = ms; }

    void start() override;
    void stop()  override;
    QString name() const override { return "IoSimulator"; }

private slots:
    void onTick();

private:
    QTimer  mTimer;
    int     mIntervalMs = 1000;
    qint64  mTick = 0;
    QList<Channel> mChannels;
};

#endif // IOSIMULATOR_H
