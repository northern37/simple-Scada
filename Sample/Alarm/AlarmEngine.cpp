#include "AlarmEngine.h"
#include <QMutexLocker>
#include <QDebug>

AlarmEngine::AlarmEngine(QObject *parent)
    : QObject(parent)
{
}

void AlarmEngine::addRule(const AlarmRule &rule)
{
    QMutexLocker lock(&mMutex);
    mRules.append(rule);
}

void AlarmEngine::removeRule(const QString &channel, AlarmLevel level)
{
    QMutexLocker lock(&mMutex);
    mRules.erase(
        std::remove_if(mRules.begin(), mRules.end(),
                       [&](const AlarmRule &r) {
                           return r.channel == channel && r.level == level;
                       }),
        mRules.end());
}

void AlarmEngine::clearRules()
{
    QMutexLocker lock(&mMutex);
    mRules.clear();
    mHistory.clear();
}

QList<AlarmRule> AlarmEngine::rules() const
{
    QMutexLocker lock(&mMutex);
    return mRules;
}

bool AlarmEngine::isChannelInAlarm(const QString &channel) const
{
    QMutexLocker lock(&mMutex);
    for (const auto &r : mRules) {
        if (r.channel == channel && r.triggered)
            return true;
    }
    return false;
}

QList<AlarmEvent> AlarmEngine::alarmEvents() const
{
    QMutexLocker lock(&mMutex);
    return mHistory;
}

int AlarmEngine::activeEventCount() const
{
    QMutexLocker lock(&mMutex);
    int count = 0;
    for (const auto &r : mRules) {
        if (r.triggered) ++count;
    }
    return count;
}

void AlarmEngine::clearHistory()
{
    QMutexLocker lock(&mMutex);
    mHistory.clear();
    // 重置所有规则的 triggered 状态
    for (auto &r : mRules)
        r.triggered = false;
}

void AlarmEngine::updateValue(const QString &name, const QVariant &value)
{
    QMutexLocker lock(&mMutex);

    bool ok = false;
    const double v = value.toDouble(&ok);
    if (!ok) return; // 非数值通道直接忽略

    for (auto &rule : mRules) {
        if (rule.channel != name || !rule.enabled)
            continue;

        // 判断是否越限:低于 min 或高于 max
        const bool outOfRange = (v < rule.min) || (v > rule.max);

        if (outOfRange && !rule.triggered) {
            // ★ 从未触发 → 触发:记录事件
            rule.triggered = true;

            AlarmEvent event;
            event.channel   = name;
            event.value     = v;
            event.level     = rule.level;
            event.timestamp = QDateTime::currentDateTime();

            // 生成规则名
            if (!rule.message.isEmpty()) {
                event.ruleName = rule.message;
            } else {
                event.ruleName = QString("%1 %2 [%3, %4]")
                                    .arg(name)
                                    .arg(rule.level == AlarmLevel::Critical ? "CRIT" : "WARN")
                                    .arg(rule.min, 0, 'f', 1)
                                    .arg(rule.max, 0, 'f', 1);
            }

            mHistory.append(event);
            if (mHistory.size() > 1000)
                mHistory.removeFirst(); // 内存软限制

            qDebug() << "[AlarmEngine] TRIGGERED" << event.ruleName
                     << "value:" << v;

            // 在锁外发信号会死锁? Qt 直连信号在同一线程安全
            lock.unlock();
            emit alarmTriggered(event);
            emit alarmStateChanged();
            lock.relock();

        } else if (!outOfRange && rule.triggered) {
            // ★ 从报警态恢复
            rule.triggered = false;

            qDebug() << "[AlarmEngine] RESOLVED" << name << "value:" << v;

            lock.unlock();
            emit alarmResolved(name);
            emit alarmStateChanged();
            lock.relock();
        }
    }
}
