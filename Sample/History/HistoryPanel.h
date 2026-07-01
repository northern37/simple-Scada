#ifndef HISTORYPANEL_H
#define HISTORYPANEL_H

#include <QWidget>
#include <QSqlDatabase>
#include <QTimer>

class QTableView;
class QSqlQueryModel;
class QLineEdit;
class QComboBox;
class QPushButton;
class QLabel;

// 历史数据展示面板:QTableView + QSqlQueryModel
// - 共享 HistoryRecorder 的数据库连接
// - 支持按通道过滤、关键字搜索、清空、CSV 导出
class HistoryPanel : public QWidget
{
    Q_OBJECT
public:
    explicit HistoryPanel(QWidget *parent = nullptr);
    ~HistoryPanel() override;

    // 设置数据源(由 MainWindow 传入 Recorder 的 db)
    void setDatabase(const QSqlDatabase &db);
    void refresh();

private slots:
    void onFilterChanged();
    void onClearClicked();
    void onExportClicked();

private:
    void buildUi();

    QSqlDatabase mDb;
    QSqlQueryModel *mModel = nullptr;
    QTableView   *mView        = nullptr;
    QLineEdit    *mFilterEdit  = nullptr;
    QComboBox    *mChannelCombo = nullptr;
    QPushButton  *mRefreshBtn  = nullptr;
    QPushButton  *mClearBtn    = nullptr;
    QPushButton  *mExportBtn   = nullptr;
    QLabel       *mCountLabel  = nullptr;
    QTimer       *mDebounceTimer = nullptr;   // ★ 200ms 防抖
};

#endif // HISTORYPANEL_H
