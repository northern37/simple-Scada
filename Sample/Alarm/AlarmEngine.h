#ifndef ALARMENGINE_H
#define ALARMENGINE_H

#include <QObject>
#include <QList>
#include <QMutex>
#include <QDateTime>
#include <QVariant>

// 报警级别
enum class AlarmLevel { Warning, Critical };

// 报警规则:定义某个通道的阈值条件
struct AlarmRule {
    QString    channel;    // 通道名,如 "temperature"
    double     min = 0;    // 下限(低于此值触发)
    double     max = 0;    // 上限(高于此值触发)
    AlarmLevel level = AlarmLevel::Warning;
    bool       enabled = true;
    QString    message;    // 自定义报警描述
    bool       triggered = false;  // 当前是否处于报警态
};

// 报警事件:触发时记录的快照
struct AlarmEvent {
    QString    channel;
    double     value = 0;
    QString    ruleName;   // 如 "temperature > 80°C"
    AlarmLevel level = AlarmLevel::Warning;
    QDateTime  timestamp;

    QString levelString() const {
        return level == AlarmLevel::Critical ? "Critical" : "Warning";
    }
};

// 报警引擎:存储阈值规则 → 接受实时数据 → 判断是否触发/恢复 → 发出信号
class AlarmEngine : public QObject
{
    Q_OBJECT
public:
    explicit AlarmEngine(QObject *parent = nullptr);

    // 规则管理
    void addRule(const AlarmRule &rule);
    void removeRule(const QString &channel, AlarmLevel level);
    void clearRules();
    QList<AlarmRule> rules() const;

    // 状态查询
    bool isChannelInAlarm(const QString &channel) const;
    QList<AlarmEvent> alarmEvents() const;
    int activeEventCount() const;

    // 核心:外部数据源调用此方法注入实时值
    void updateValue(const QString &name, const QVariant &value);

    // 清空历史事件
    void clearHistory();

signals:
    void alarmTriggered(const AlarmEvent &event);
    void alarmResolved(const QString &channel);
    void alarmStateChanged();   // 任何触发/恢复都会发出,用于面板刷新

private:
    QList<AlarmRule>  mRules;
    QList<AlarmEvent> mHistory;
    mutable QMutex    mMutex;
};

#endif // ALARMENGINE_H
