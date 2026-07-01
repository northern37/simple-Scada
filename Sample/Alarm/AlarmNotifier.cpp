#include "AlarmNotifier.h"

#include <QMessageBox>
#include <QPushButton>
#include <QApplication>

AlarmNotifier::AlarmNotifier(QObject *parent)
    : QObject(parent)
{
    mAutoCloseTimer.setSingleShot(true);
    connect(&mAutoCloseTimer, &QTimer::timeout, this, [this]() {
        // 关闭当前弹窗
        if (mCurrentPopup) {
            mCurrentPopup->close();
            mCurrentPopup->deleteLater();
            mCurrentPopup = nullptr;
        }
    });
}

void AlarmNotifier::onAlarmTriggered(const AlarmEvent &event)
{
    if (!mEnabled) return;

    // 关闭旧弹窗(如果还存在)
    if (mCurrentPopup) {
        mCurrentPopup->close();
        mCurrentPopup->deleteLater();
        mCurrentPopup = nullptr;
    }
    mAutoCloseTimer.stop();

    const QString title = event.level == AlarmLevel::Critical
        ? tr("严重报警!")
        : tr("报警警告");

    const QString msg = tr("通道: %1\n数值: %2\n规则: %3\n时间: %4")
        .arg(event.channel)
        .arg(event.value, 0, 'f', 2)
        .arg(event.ruleName)
        .arg(event.timestamp.toString("HH:mm:ss"));

    // 使用 MessageBox 作为弹窗
    QMessageBox *box = new QMessageBox(
        event.level == AlarmLevel::Critical
            ? QMessageBox::Critical
            : QMessageBox::Warning,
        title, msg, QMessageBox::Ok);

    // 3.5 秒自动关闭
    mAutoCloseTimer.start(3500);
    mCurrentPopup = box;

    // 非模态显示,不阻塞 SCADA 主循环
    box->setModal(false);
    box->show();
}

void AlarmNotifier::onAlarmResolved(const QString &channel)
{
    Q_UNUSED(channel);
    // 可选:显示恢复通知。当前设计只弹触发,不弹恢复,避免刷屏。
}
