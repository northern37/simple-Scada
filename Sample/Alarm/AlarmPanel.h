#ifndef ALARMPANEL_H
#define ALARMPANEL_H

#include <QWidget>
#include <QTableView>
#include <QStandardItemModel>
#include <QPushButton>
#include "AlarmEngine.h"

// 报警历史面板:以表格形式展示所有报警事件
class AlarmPanel : public QWidget
{
    Q_OBJECT
public:
    explicit AlarmPanel(QWidget *parent = nullptr);

    // 绑定引擎(读取历史 + 实时刷新)
    void setEngine(AlarmEngine *engine);

public slots:
    void appendEvent(const AlarmEvent &event);  // 实时追加单行
    void clearHistory();                         // 清空 + 同步 engine
    void refreshTable();                         // 全量刷新(从 engine 重新读取)

private:
    void setupUi();
    QColor colorForLevel(AlarmLevel level) const;

    AlarmEngine       *mEngine  = nullptr;
    QTableView        *mTable   = nullptr;
    QStandardItemModel *mModel  = nullptr;
    QPushButton       *mClearBtn = nullptr;
};

#endif // ALARMPANEL_H
