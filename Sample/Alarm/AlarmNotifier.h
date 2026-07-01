#ifndef ALARMNOTIFIER_H
#define ALARMNOTIFIER_H

#include <QObject>
#include <QTimer>
#include "AlarmEngine.h"

class QMessageBox;

// 报警弹窗通知器:每条新报警弹出独立提示,3.5s 自动消失
class AlarmNotifier : public QObject
{
    Q_OBJECT
public:
    explicit AlarmNotifier(QObject *parent = nullptr);

    void setEnabled(bool on) { mEnabled = on; }
    bool isEnabled() const { return mEnabled; }

public slots:
    void onAlarmTriggered(const AlarmEvent &event);
    void onAlarmResolved(const QString &channel);

private:
    bool     mEnabled = true;
    QTimer   mAutoCloseTimer;
    QWidget *mCurrentPopup = nullptr;        // 当前弹窗(仅保留一个)
};

#endif // ALARMNOTIFIER_H
