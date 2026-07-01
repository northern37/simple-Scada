#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHash>
#include <QList>
#include <QPair>
#include <QTimer>
#include <QMenu>
#include <QAction>
#include "DataSource/IoSimulator.h"

class QScadaBoardController;
class QScadaDeviceInfo;
class QLabel;
class QComboBox;
class QPushButton;
class QDockWidget;
class ModbusTcpSource;
class ModbusRtuSource;
class HistoryRecorder;
class HistoryPanel;

// 上位机主窗口
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    // 数据源 → SCADA 路由
    void onValueProduced(const QString &name, const QVariant &value);

    // UI 更新
    void updateClock();
    void refreshStatusBar();

    // 数据源切换
    void onSourceChanged(const QString &sourceName);
    void onStartClicked();
    void onStopClicked();
    void onSourceConnectionChanged();
    void onSourceStartFailed(const QString &reason);

    // Phase 2.2 历史
    void onHistoryToggleClicked(bool checked);
    void onHistoryRefreshTick();

    // ★ 编辑器 / 项目
    void onEditModeToggled(bool checked);
    void onSaveProject();
    void onLoadProject();
    void onAddWidget();

private:
    void buildUi();
    void setupScadaBoard();
    void setupSources();
    void setupHistory();
    void teardownCurrentSource();

    QScadaBoardController *mController = nullptr;
    QScadaDeviceInfo      *mDevice     = nullptr;
    DataSource            *mActiveSource = nullptr;   // 当前激活的数据源
    int                    mNextWidgetId = 8;          // 已有 0~7,新图元从 8 开始

    // 候数据源(不会同时激活)
    IoSimulator           *mSimulator  = nullptr;
    ModbusTcpSource       *mModbus     = nullptr;
    ModbusRtuSource       *mModbusRtu  = nullptr;

    // 历史数据(Phase 2.2)
    HistoryRecorder       *mHistory     = nullptr;
    HistoryPanel          *mHistoryPanel = nullptr;
    QDockWidget           *mHistoryDock  = nullptr;

    QComboBox  *mSourceCombo = nullptr;
    QPushButton *mStartBtn   = nullptr;
    QPushButton *mStopBtn    = nullptr;
    QLabel *mTopTitleLabel   = nullptr;
    QLabel *mTopStatusLabel  = nullptr;
    QLabel *mTopClockLabel   = nullptr;
    QLabel *mStatusBarLabel  = nullptr;

    QTimer  mClockTimer;
    QTimer  mHistoryRefreshTimer;

    // 路由表:通道名 → SCADA objectId(一通道可对应多图元)
    QList<QPair<QString, int>> mChannelRoutes;

    // ★ 启动失败标志:onStartClicked 检查此标志,避免 start() 失败后还把顶栏设回绿色
    bool mStartFailed = false;
};

#endif // MAINWINDOW_H
