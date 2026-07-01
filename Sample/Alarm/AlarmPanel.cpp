#include "AlarmPanel.h"

#include <QVBoxLayout>
#include <QHeaderView>

AlarmPanel::AlarmPanel(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
}

void AlarmPanel::setupUi()
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(4, 4, 4, 4);

    // 表格
    mModel = new QStandardItemModel(0, 6, this);
    mModel->setHorizontalHeaderLabels({
        tr("时间"), tr("通道"), tr("当前值"),
        tr("类型"), tr("规则"), tr("消息")
    });

    mTable = new QTableView(this);
    mTable->setModel(mModel);
    mTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    mTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mTable->horizontalHeader()->setStretchLastSection(true);
    mTable->verticalHeader()->hide();
    mTable->setAlternatingRowColors(true);
    mTable->setStyleSheet(
        "QTableView { background: #1e293b; color: #f1f5f9; gridline-color: #334155; }"
        "QTableView::item:selected { background: #22d3ee; color: #0f172a; }"
        "QHeaderView::section { background: #0f172a; color: #94a3b8;"
        "                       padding: 4px; border: 1px solid #334155; }"
    );

    // 清除按钮
    mClearBtn = new QPushButton(tr("清除报警历史"), this);
    mClearBtn->setStyleSheet(
        "QPushButton { background: #0f172a; color: #f1f5f9;"
        "             border: 1px solid #475569; padding: 6px 16px; }"
        "QPushButton:hover { background: #ef4444; color: white; }"
    );
    connect(mClearBtn, &QPushButton::clicked, this, &AlarmPanel::clearHistory);

    layout->addWidget(mTable);
    layout->addWidget(mClearBtn);
}

void AlarmPanel::setEngine(AlarmEngine *engine)
{
    mEngine = engine;
    if (mEngine)
        refreshTable();
}

void AlarmPanel::appendEvent(const AlarmEvent &event)
{
    const int row = mModel->rowCount();
    mModel->insertRow(row);

    const QColor color = colorForLevel(event.level);

    auto setItem = [&](int col, const QString &text) {
        auto *item = new QStandardItem(text);
        item->setForeground(color);
        mModel->setItem(row, col, item);
    };

    setItem(0, event.timestamp.toString("HH:mm:ss"));
    setItem(1, event.channel);
    setItem(2, QString::number(event.value, 'f', 2));
    setItem(3, event.levelString());
    setItem(4, event.ruleName);
    setItem(5, event.ruleName); // message 复用规则名

    mTable->scrollToBottom();
}

void AlarmPanel::clearHistory()
{
    if (mEngine)
        mEngine->clearHistory();
    mModel->removeRows(0, mModel->rowCount());
}

void AlarmPanel::refreshTable()
{
    if (!mEngine) return;

    mModel->removeRows(0, mModel->rowCount());
    for (const auto &ev : mEngine->alarmEvents()) {
        appendEvent(ev);
    }
}

QColor AlarmPanel::colorForLevel(AlarmLevel level) const
{
    return level == AlarmLevel::Critical
        ? QColor("#ef4444")    // 红色
        : QColor("#f59e0b");   // 黄色
}
