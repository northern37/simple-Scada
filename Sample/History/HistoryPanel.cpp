#include "HistoryPanel.h"

#include <QTableView>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QSqlError>
#include <QHeaderView>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QTextStream>
#include <QFile>
#include <QMessageBox>
#include <QDateTime>
#include <QDebug>

HistoryPanel::HistoryPanel(QWidget *parent) : QWidget(parent)
{
    buildUi();
}

HistoryPanel::~HistoryPanel() = default;

void HistoryPanel::buildUi()
{
    setStyleSheet(
        "QWidget { background: #0f172a; color: #e2e8f0; }"
        "QTableView { background: #1e293b; alternate-background-color: #0f172a;"
        "            gridline-color: #334155; color: #e2e8f0;"
        "            selection-background-color: #22d3ee; selection-color: #0f172a; }"
        "QHeaderView::section { background: #334155; color: #e2e8f0; padding: 6px; border: none; }"
        "QLineEdit, QComboBox { background: #1e293b; color: #e2e8f0; border: 1px solid #334155;"
        "                       padding: 4px 8px; }"
        "QPushButton { background: #1e293b; color: #e2e8f0; border: 1px solid #334155;"
        "             padding: 4px 12px; }"
        "QPushButton:hover { background: #334155; }"
    );

    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(8, 8, 8, 8);
    root->setSpacing(8);

    // ---- 顶部筛选栏 ----
    auto *filterBar = new QHBoxLayout();
    filterBar->setSpacing(8);

    filterBar->addWidget(new QLabel(tr("通道:"), this));
    mChannelCombo = new QComboBox(this);
    // ★ 必须用 addItem(text, userData),否则 currentData() 永远返回空
    mChannelCombo->addItem(tr("(全部)"),    QString());
    mChannelCombo->addItem("temperature",   "temperature");
    mChannelCombo->addItem("pressure",      "pressure");
    mChannelCombo->addItem("level",         "level");
    mChannelCombo->addItem("flow",          "flow");
    filterBar->addWidget(mChannelCombo);

    filterBar->addWidget(new QLabel(tr("搜索:"), this));
    mFilterEdit = new QLineEdit(this);
    mFilterEdit->setPlaceholderText(tr("时间戳或值包含..."));
    filterBar->addWidget(mFilterEdit, 1);

    mRefreshBtn = new QPushButton(tr("刷新"), this);
    mClearBtn   = new QPushButton(tr("清空"), this);
    mExportBtn  = new QPushButton(tr("导出 CSV"), this);
    filterBar->addWidget(mRefreshBtn);
    filterBar->addWidget(mClearBtn);
    filterBar->addWidget(mExportBtn);

    root->addLayout(filterBar);

    // ---- 中部表格 ----
    mView = new QTableView(this);
    mView->setAlternatingRowColors(true);
    mView->setSelectionBehavior(QAbstractItemView::SelectRows);
    mView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mView->horizontalHeader()->setStretchLastSection(true);
    mView->verticalHeader()->setVisible(false);
    root->addWidget(mView, 1);

    // ---- 底部统计 ----
    mCountLabel = new QLabel(tr("记录数: 0"), this);
    mCountLabel->setStyleSheet("color: #94a3b8;");
    root->addWidget(mCountLabel);

    // ---- 防抖定时器(200ms,用户停止输入后才刷新) ----
    mDebounceTimer = new QTimer(this);
    mDebounceTimer->setSingleShot(true);
    mDebounceTimer->setInterval(200);
    connect(mDebounceTimer, &QTimer::timeout, this, &HistoryPanel::refresh);

    // 信号
    connect(mChannelCombo, &QComboBox::currentTextChanged,
            this, &HistoryPanel::onFilterChanged);
    connect(mFilterEdit, &QLineEdit::textChanged,
            this, &HistoryPanel::onFilterChanged);
    connect(mRefreshBtn, &QPushButton::clicked,
            this, &HistoryPanel::refresh);
    connect(mClearBtn, &QPushButton::clicked,
            this, &HistoryPanel::onClearClicked);
    connect(mExportBtn, &QPushButton::clicked,
            this, &HistoryPanel::onExportClicked);
}

void HistoryPanel::setDatabase(const QSqlDatabase &db)
{
    mDb = db;
    if (!mDb.isOpen()) {
        qWarning() << "[HistoryPanel] db not open";
        return;
    }
    mModel = new QSqlQueryModel(this);
    mView->setModel(mModel);
    mView->setColumnWidth(0, 60);
    mView->setColumnWidth(1, 200);
    mView->setColumnWidth(2, 120);
    mView->setColumnWidth(3, 80);
    refresh();
}

void HistoryPanel::refresh()
{
    if (mModel == nullptr) return;

    const QString ch = mChannelCombo->currentData().toString();
    const QString txt = mFilterEdit->text().trimmed();

    // ---- 使用 prepare + bindValue 防 SQL 注入 ----
    QString sql = "SELECT id, ts, channel, value FROM history";
    QStringList conditions;
    QVariantList bindValues;

    if (!ch.isEmpty()) {
        conditions << "channel = ?";
        bindValues << ch;
    }
    if (!txt.isEmpty()) {
        conditions << "(ts LIKE ? OR CAST(value AS TEXT) LIKE ?)";
        const QString like = "%" + txt + "%";
        bindValues << like << like;
    }
    if (!conditions.isEmpty()) {
        sql += " WHERE " + conditions.join(" AND ");
    }
    sql += " ORDER BY id DESC LIMIT 2000";

    qDebug() << "[HistoryPanel] ch=" << ch << "txt=" << txt;
    qDebug() << "[HistoryPanel] SQL:" << sql;

    QSqlQuery q(mDb);
    q.prepare(sql);
    for (const QVariant &v : std::as_const(bindValues)) {
        q.addBindValue(v);
    }
    if (!q.exec()) {
        qWarning() << "[HistoryPanel] query error:" << q.lastError().text();
        return;
    }

    mModel->setQuery(std::move(q));
    // ★ QSqlQueryModel 的 setHeaderData 必须在 setQuery 之后调用才生效
    mModel->setHeaderData(0, Qt::Horizontal, tr("ID"));
    mModel->setHeaderData(1, Qt::Horizontal, tr("时间"));
    mModel->setHeaderData(2, Qt::Horizontal, tr("通道"));
    mModel->setHeaderData(3, Qt::Horizontal, tr("值"));

    // 统计
    QSqlQuery countQ(mDb);
    if (countQ.exec("SELECT COUNT(*) FROM history")) {
        if (countQ.next()) mCountLabel->setText(tr("记录数: %1").arg(countQ.value(0).toInt()));
    } else {
        mCountLabel->setText(tr("记录数: ?"));
    }
}

void HistoryPanel::onFilterChanged()
{
    // ★ 防抖:重启 200ms 定时器,用户停止输入后才触发 refresh
    mDebounceTimer->start();
}

void HistoryPanel::onClearClicked()
{
    if (QMessageBox::question(this, tr("清空历史"),
        tr("确定清空所有历史记录?此操作不可撤销!")) != QMessageBox::Yes) return;

    QSqlQuery q(mDb);
    if (!q.exec("DELETE FROM history")) {
        QMessageBox::warning(this, tr("清空失败"), q.lastError().text());
        return;
    }
    refresh();
}

void HistoryPanel::onExportClicked()
{
    const QString defaultName = QStringLiteral("history_%1.csv")
        .arg(QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss"));
    const QString path = QFileDialog::getSaveFileName(
        this, tr("导出 CSV"), defaultName, tr("CSV (*.csv)"));
    if (path.isEmpty()) return;

    QFile f(path);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("导出失败"), f.errorString());
        return;
    }
    QTextStream ts(&f);
    ts << "id,ts,channel,value\n";

    QSqlQuery q(mDb);
    if (!q.exec("SELECT id, ts, channel, value FROM history ORDER BY id")) {
        QMessageBox::warning(this, tr("导出失败"), q.lastError().text());
        return;
    }
    while (q.next()) {
        ts << q.value(0).toString() << ","
           << q.value(1).toString() << ","
           << q.value(2).toString() << ","
           << q.value(3).toString() << "\n";
    }
    QMessageBox::information(this, tr("导出完成"),
        tr("已导出到:\n%1").arg(path));
}
