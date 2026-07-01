#include "MainWindow.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QStatusBar>
#include <QToolBar>
#include <QComboBox>
#include <QPushButton>
#include <QDateTime>
#include <QDebug>
#include <QHostAddress>
#include <QDockWidget>
#include <QCoreApplication>
#include <QMenuBar>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QSpinBox>
#include <QComboBox>
#include <QDialog>

// SCADA 库
#include "QScadaBoard/qscadaboard.h"
#include "QScadaBoard/qscadaboardcontroller.h"
#include "QScadaDevice/qscadadeviceinfo.h"
#include "QScadaObject/qscadaobjectinfo.h"

// QSimpleScadaWidgets 资源(EEIoT)
#include "qscadaconfig.h"   // QMLConfig 单例

// 数据源
#include "DataSource/ModbusTcpSource.h"
#include "DataSource/ModbusRtuSource.h"

// 历史(Phase 2.2)
#include "History/HistoryRecorder.h"
#include "History/HistoryPanel.h"

// 报警(Phase 3.0)
#include "Alarm/AlarmEngine.h"
#include "Alarm/AlarmPanel.h"
#include "Alarm/AlarmNotifier.h"

// QML 图元 qrc 路径前缀
#define QML_WIDGETS_PATH   ":/qsimplescada/widgets/EEIoT/"
#define QML_EXTENSIONS_PATH ":/qsimplescada/extensions/"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setWindowTitle(tr("QSimpleScada 上位机 - Qt 6 Demo"));
    resize(1280, 800);

    buildUi();
    setupScadaBoard();
    setupSources();
    setupHistory();
    setupAlarm();

    // 时钟每秒更新
    connect(&mClockTimer, &QTimer::timeout, this, &MainWindow::updateClock);
    mClockTimer.start(1000);
    updateClock();
}

MainWindow::~MainWindow()
{
    teardownCurrentSource();
}

// ============================================================
// UI 搭建
// ============================================================
void MainWindow::buildUi()
{
    // ---- 顶部状态栏(自定义 widget) ----
    QWidget *topBar = new QWidget(this);
    topBar->setFixedHeight(48);
    topBar->setStyleSheet("QWidget { background-color: #0f172a; color: #e2e8f0; }");

    QHBoxLayout *topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(16, 0, 16, 0);
    topLayout->setSpacing(16);

    mTopTitleLabel = new QLabel(tr("QSimpleScada 上位机"), topBar);
    mTopTitleLabel->setStyleSheet("font-size: 16px; font-weight: bold;");
    topLayout->addWidget(mTopTitleLabel);

    topLayout->addSpacing(8);
    QFrame *sep = new QFrame(topBar);
    sep->setFrameShape(QFrame::VLine);
    sep->setStyleSheet("color: #334155;");
    topLayout->addWidget(sep);

    mTopStatusLabel = new QLabel(tr("● 未启动"), topBar);
    mTopStatusLabel->setStyleSheet("color: #f59e0b; font-size: 13px;");
    topLayout->addWidget(mTopStatusLabel);

    topLayout->addStretch(1);

    mTopClockLabel = new QLabel(topBar);
    mTopClockLabel->setStyleSheet("color: #94a3b8; font-family: Consolas; font-size: 13px;");
    topLayout->addWidget(mTopClockLabel);

    setMenuWidget(topBar);

    // ---- 工具栏:数据源切换 ----
    QToolBar *toolbar = addToolBar(tr("数据源"));
    toolbar->setMovable(false);
    toolbar->setStyleSheet(
        "QToolBar { background: #1e293b; color: #f1f5f9; border: none; padding: 4px; spacing: 8px; }"
        "QToolBar QLabel { color: #f1f5f9; font-weight: 600; padding: 0 4px; }"
        "QToolButton { color: #f1f5f9; padding: 4px 12px; }"
        "QToolButton:hover { background: #334155; }"
        "QComboBox { background: #0f172a; color: #f1f5f9; border: 1px solid #475569;"
        "            padding: 4px 8px; min-width: 160px; }"
        "QComboBox QAbstractItemView { background: #0f172a; color: #f1f5f9;"
        "                               selection-background-color: #22d3ee;"
        "                               selection-color: #0f172a; }"
        "QPushButton { background: #0f172a; color: #f1f5f9; border: 1px solid #475569;"
        "             padding: 4px 16px; font-weight: 600; }"
        "QPushButton:hover { background: #334155; border-color: #22d3ee; }"
        "QPushButton:pressed { background: #22d3ee; color: #0f172a; }"
        "QPushButton:checked { background: #0891b2; color: #f1f5f9; border-color: #22d3ee; }"
    );

    toolbar->addWidget(new QLabel(tr("  数据源:"), toolbar));
    mSourceCombo = new QComboBox(toolbar);
    mSourceCombo->addItems({
        "IoSimulator (模拟)",
        "Modbus TCP (127.0.0.1:502)",
        "Modbus RTU (COM2, 9600)"
    });
    toolbar->addWidget(mSourceCombo);

    mStartBtn = new QPushButton(tr("▶ 启动"), toolbar);
    mStopBtn  = new QPushButton(tr("■ 停止"), toolbar);
    toolbar->addWidget(mStartBtn);
    toolbar->addWidget(mStopBtn);

    toolbar->addSeparator();

    QPushButton *historyBtn = new QPushButton(tr("📊 历史数据"), toolbar);
    historyBtn->setCheckable(true);
    historyBtn->setChecked(false);
    toolbar->addWidget(historyBtn);
    connect(historyBtn, &QPushButton::toggled,
            this, &MainWindow::onHistoryToggleClicked);

    // ★ 报警面板按钮
    mAlarmBtn = new QPushButton(tr("⚠ 报警"), toolbar);
    mAlarmBtn->setCheckable(true);
    mAlarmBtn->setChecked(false);
    toolbar->addWidget(mAlarmBtn);
    connect(mAlarmBtn, &QPushButton::toggled,
            this, &MainWindow::onAlarmToggleClicked);

    connect(mSourceCombo, &QComboBox::currentTextChanged,
            this, &MainWindow::onSourceChanged);
    connect(mStartBtn, &QPushButton::clicked, this, &MainWindow::onStartClicked);
    connect(mStopBtn,  &QPushButton::clicked, this, &MainWindow::onStopClicked);

    toolbar->addSeparator();
    QPushButton *editBtn = new QPushButton(tr("✎ 编辑"), toolbar);
    editBtn->setCheckable(true);
    editBtn->setChecked(false);
    toolbar->addWidget(editBtn);
    connect(editBtn, &QPushButton::toggled, this, &MainWindow::onEditModeToggled);

    QPushButton *addBtn = new QPushButton(tr("＋ 添加图元"), toolbar);
    toolbar->addWidget(addBtn);
    connect(addBtn, &QPushButton::clicked, this, &MainWindow::onAddWidget);

    // ---- 文件菜单(用工具栏按钮+QMenu,避免 setMenuWidget 与 menuBar 冲突) ----
    QPushButton *fileBtn = new QPushButton(tr("文件"), toolbar);
    QMenu *fileMenu = new QMenu(fileBtn);
    QAction *saveAct = fileMenu->addAction(tr("保存项目(&S)..."));
    saveAct->setShortcut(QKeySequence::Save);
    connect(saveAct, &QAction::triggered, this, &MainWindow::onSaveProject);
    QAction *loadAct = fileMenu->addAction(tr("加载项目(&O)..."));
    loadAct->setShortcut(QKeySequence::Open);
    connect(loadAct, &QAction::triggered, this, &MainWindow::onLoadProject);
    fileBtn->setMenu(fileMenu);
    toolbar->addWidget(fileBtn);

    // ---- 底部状态栏 ----
    mStatusBarLabel = new QLabel(this);
    statusBar()->addWidget(mStatusBarLabel, 1);
}

void MainWindow::updateClock()
{
    mTopClockLabel->setText(
        QDateTime::currentDateTime().toString("yyyy-MM-dd  HH:mm:ss"));
}

void MainWindow::refreshStatusBar()
{
    QStringList items;
    for (const auto &r : std::as_const(mChannelRoutes)) {
        items << QString("%1→obj%2").arg(r.first).arg(r.second);
    }
    const QString srcName = mActiveSource ? mActiveSource->name() : "(未启动)";
    mStatusBarLabel->setText(
        tr("数据源: %1 | 路由: %2").arg(srcName).arg(items.join(", ")));
}

// ============================================================
// SCADA 板卡初始化
// ============================================================
void MainWindow::setupScadaBoard()
{
    QMLConfig::instance().appendQMLPath(QML_WIDGETS_PATH);
    QMLConfig::instance().appendQMLPath(QML_EXTENSIONS_PATH);

    mController = new QScadaBoardController(this);

    mDevice = new QScadaDeviceInfo();
    mDevice->setName(tr("模拟设备 1"));
    mDevice->setIp(QHostAddress("127.0.0.1"));
    mController->appendDevice(mDevice);

    mController->initBoardForDeviceIp("127.0.0.1");

    QScadaBoard *board = mController->getBoardListForDeviceIp("127.0.0.1").first();
    if (board == nullptr) {
        qWarning() << "板卡未初始化!";
        return;
    }

    auto makeInfo = [](int id, const QRect &g, const QString &qmlPath) {
        auto *info = new QScadaObjectInfo();
        info->setId(id);
        info->setGeometry(g);
        info->setShowBackground(true);
        info->setType(QScadaObjectTypeQML);
        info->setUIResourcePath(qmlPath);
        return info;
    };

    board->createNewObject(makeInfo(0, QRect( 20,  20, 240, 360),
                                    QString(QML_WIDGETS_PATH) + "Linear.qml"));
    board->createNewObject(makeInfo(1, QRect(280,  20, 240, 360),
                                    QString(QML_WIDGETS_PATH) + "Knob.qml"));
    board->createNewObject(makeInfo(2, QRect(540,  20, 360, 360),
                                    QString(QML_EXTENSIONS_PATH) + "TrendChart.qml"));
    board->createNewObject(makeInfo(3, QRect(920,  20, 240, 360),
                                    QString(QML_WIDGETS_PATH) + "WaterLevel.qml"));
    board->createNewObject(makeInfo(4, QRect( 20, 400, 240, 180),
                                    QString(QML_EXTENSIONS_PATH) + "ValueLabel.qml"));
    board->createNewObject(makeInfo(5, QRect(280, 400, 240, 180),
                                    QString(QML_EXTENSIONS_PATH) + "PieChart.qml"));
    board->createNewObject(makeInfo(6, QRect(540, 400, 360, 180),
                                    QString(QML_EXTENSIONS_PATH) + "BarGraph.qml"));
    board->createNewObject(makeInfo(7, QRect(920, 400, 240, 180),
                                    QString(QML_EXTENSIONS_PATH) + "ValueLabel.qml"));

    // 一通道 → 多图元(用列表而非 QHash,避免 key 覆盖)
    mChannelRoutes.append({"temperature", 0});  // Linear(左列顶部)
    mChannelRoutes.append({"pressure",    1});  // Knob(中列顶部)
    mChannelRoutes.append({"temperature", 2});  // TrendChart(中右)
    mChannelRoutes.append({"level",       3});  // WaterLevel(右列)
    mChannelRoutes.append({"flow",        4});  // ValueLabel(左下)
    mChannelRoutes.append({"pressure",    5});  // PieChart(中下)
    mChannelRoutes.append({"flow",        6});  // BarGraph(中右下)
    mChannelRoutes.append({"temperature", 7});  // ValueLabel(右下)

    setCentralWidget(mController);
}

// ============================================================
// 数据源管理
// ============================================================
void MainWindow::setupSources()
{
    // 构造三个候选数据源(都不启动)
    mSimulator = new IoSimulator(this);
    mModbus    = new ModbusTcpSource(this);
    mModbusRtu = new ModbusRtuSource(this);

    // 全部候选数据源统一监听 valueProduced
    connect(mSimulator, &DataSource::valueProduced,
            this, &MainWindow::onValueProduced);
    connect(mModbus, &DataSource::valueProduced,
            this, &MainWindow::onValueProduced);
    connect(mModbusRtu, &DataSource::valueProduced,
            this, &MainWindow::onValueProduced);

    // Modbus 状态变化
    connect(mModbus, &DataSource::connectionChanged,
            this, &MainWindow::onSourceConnectionChanged);
    connect(mModbusRtu, &DataSource::connectionChanged,
            this, &MainWindow::onSourceConnectionChanged);

    // Modbus 启动失败 → 状态栏红字提示
    connect(mModbus,    &DataSource::startFailed,
            this, &MainWindow::onSourceStartFailed);
    connect(mModbusRtu, &DataSource::startFailed,
            this, &MainWindow::onSourceStartFailed);

    // 默认激活 IoSimulator 但不启动(用户点启动才开始)
    mActiveSource = mSimulator;
    refreshStatusBar();
}

// ============================================================
// 历史数据(Phase 2.2)
// ============================================================
void MainWindow::setupHistory()
{
    // 1. 构造 Recorder 并打开 SQLite(存到可执行目录旁的 data/)
    mHistory = new HistoryRecorder(this);
    const QString dbPath = QCoreApplication::applicationDirPath()
                            + "/data/scada_history.db";
    if (!mHistory->open(dbPath)) {
        qWarning() << "[MainWindow] history recorder open failed, fallback to memory";
        mHistory->open(":memory:");
    }

    // 2. 把 Recorder 挂到所有 3 个数据源上(只对 active 的有数据流)
    connect(mSimulator, &DataSource::valueProduced,
            mHistory,    &HistoryRecorder::onValueProduced);
    connect(mModbus,    &DataSource::valueProduced,
            mHistory,    &HistoryRecorder::onValueProduced);
    connect(mModbusRtu, &DataSource::valueProduced,
            mHistory,    &HistoryRecorder::onValueProduced);

    // 3. 构造 Panel
    mHistoryPanel = new HistoryPanel(this);
    mHistoryPanel->setDatabase(mHistory->database());

    // 4. 包成 dock
    mHistoryDock = new QDockWidget(tr("📊 历史数据 (SQLite)"), this);
    mHistoryDock->setWidget(mHistoryPanel);
    mHistoryDock->setFeatures(QDockWidget::DockWidgetMovable
                              | QDockWidget::DockWidgetFloatable
                              | QDockWidget::DockWidgetClosable);
    mHistoryDock->hide();      // 默认隐藏
    addDockWidget(Qt::BottomDockWidgetArea, mHistoryDock);

    // 5. 自动定时刷新表格(2 秒)
    connect(&mHistoryRefreshTimer, &QTimer::timeout,
            this, &MainWindow::onHistoryRefreshTick);
    mHistoryRefreshTimer.start(2000);
}

void MainWindow::onHistoryToggleClicked(bool checked)
{
    mHistoryDock->setVisible(checked);
    if (checked && mHistoryPanel) {
        mHistoryPanel->refresh();
    }
}

void MainWindow::onHistoryRefreshTick()
{
    if (mHistoryDock->isVisible() && mHistoryPanel) {
        mHistoryPanel->refresh();
    }
}

void MainWindow::teardownCurrentSource()
{
    if (mActiveSource) {
        mActiveSource->stop();
    }
}

void MainWindow::onSourceChanged(const QString &sourceName)
{
    // 切换源时先停掉旧的
    teardownCurrentSource();

    if (sourceName.startsWith("IoSimulator")) {
        mActiveSource = mSimulator;
    } else if (sourceName.startsWith("Modbus TCP")) {
        mActiveSource = mModbus;
    } else if (sourceName.startsWith("Modbus RTU")) {
        mActiveSource = mModbusRtu;
    } else {
        mActiveSource = nullptr;
    }

    // ★ 切换后源处于停止状态,同步按钮/顶栏 UI
    mStartBtn->setEnabled(true);
    mStopBtn->setEnabled(false);
    mTopStatusLabel->setText(tr("● 已停止"));
    mTopStatusLabel->setStyleSheet("color: #f59e0b; font-size: 13px;");

    refreshStatusBar();
}

void MainWindow::onStartClicked()
{
    if (mActiveSource == nullptr) return;
    mStartFailed = false;        // ★ 重置
    mActiveSource->start();
    if (mStartFailed) {          // ★ onSourceStartFailed 已经处理了 UI
        mStartFailed = false;
        return;
    }
    mStartBtn->setEnabled(false);
    mStopBtn->setEnabled(true);
    mTopStatusLabel->setText(tr("● 运行中"));
    mTopStatusLabel->setStyleSheet("color: #10b981; font-size: 13px;");
    refreshStatusBar();
}

void MainWindow::onStopClicked()
{
    teardownCurrentSource();
    mStartBtn->setEnabled(true);
    mStopBtn->setEnabled(false);
    mTopStatusLabel->setText(tr("● 已停止"));
    mTopStatusLabel->setStyleSheet("color: #f59e0b; font-size: 13px;");
    refreshStatusBar();
}

void MainWindow::onSourceConnectionChanged()
{
    // Modbus TCP/RTU 状态变化(顶栏显示具体类型)
    if (mActiveSource == mModbus) {
        if (mModbus->isConnected()) {
            mTopStatusLabel->setText(tr("● 已连接 Modbus TCP"));
            mTopStatusLabel->setStyleSheet("color: #10b981; font-size: 13px;");
        } else {
            mTopStatusLabel->setText(tr("● Modbus TCP 断开"));
            mTopStatusLabel->setStyleSheet("color: #ef4444; font-size: 13px;");
        }
    } else if (mActiveSource == mModbusRtu) {
        if (mModbusRtu->isConnected()) {
            mTopStatusLabel->setText(tr("● 已连接 Modbus RTU"));
            mTopStatusLabel->setStyleSheet("color: #10b981; font-size: 13px;");
        } else {
            mTopStatusLabel->setText(tr("● Modbus RTU 断开"));
            mTopStatusLabel->setStyleSheet("color: #ef4444; font-size: 13px;");
        }
    }
}

void MainWindow::onSourceStartFailed(const QString &reason)
{
    mStartFailed = true;         // ★ 通知 onStartClicked 不要覆盖
    mTopStatusLabel->setText(tr("● 启动失败"));
    mTopStatusLabel->setStyleSheet("color: #ef4444; font-size: 13px; font-weight: bold;");
    statusBar()->showMessage(tr("❌ %1").arg(reason), 5000);
    mStartBtn->setEnabled(true);
    mStopBtn->setEnabled(false);
}

// ============================================================
// 数据路由
// ============================================================
void MainWindow::onValueProduced(const QString &name, const QVariant &value)
{
    const auto all = mController->getBoardListForDeviceIp("127.0.0.1");
    if (all.isEmpty()) return;
    QScadaBoard *board = all.first();

    // 一通道 → 多图元:遍历所有路由,匹配 channel name 的全部推送
    for (const auto &r : std::as_const(mChannelRoutes)) {
        if (r.first == name) {
            board->updateValue(r.second, value);
        }
    }

    statusBar()->showMessage(
        tr("[%1] %2 = %3")
            .arg(QDateTime::currentDateTime().toString("HH:mm:ss"))
            .arg(name)
            .arg(value.toDouble(), 0, 'f', 2),
        1500);
}

// ============================================================
// 编辑器 / 项目
// ============================================================
void MainWindow::onEditModeToggled(bool checked)
{
    mController->setEditingMode(checked);
    statusBar()->showMessage(
        checked ? tr("已进入编辑模式 — 可拖拽/缩放图元")
                : tr("已退出编辑模式"),
        3000);
}

void MainWindow::onSaveProject()
{
    const QString path = QFileDialog::getSaveFileName(
        this, tr("保存 SCADA 项目"),
        QString(), tr("SCADA 项目 (*.irp)"));
    if (path.isEmpty()) return;

    mController->saveProject(path);
    statusBar()->showMessage(tr("项目已保存: %1").arg(path), 5000);
}

void MainWindow::onLoadProject()
{
    const QString path = QFileDialog::getOpenFileName(
        this, tr("加载 SCADA 项目"),
        QString(), tr("SCADA 项目 (*.irp)"));
    if (path.isEmpty()) return;

    // 清空当前数据源和板卡
    teardownCurrentSource();

    mController->openProject(path);
    mChannelRoutes.clear();     // ★ 旧路由已失效,加载项目后清空,避免挂到错误的图元

    // ★ 重建通道路由:扫描加载后的板卡,轮询分配 temperature/pressure/level/flow
    {
        const auto boards = mController->getBoardListForDeviceIp("127.0.0.1");
        if (!boards.isEmpty()) {
            QScadaBoard *board = boards.first();
            const QStringList channels = {"temperature", "pressure", "level", "flow"};
            int chIdx = 0;
            for (QScadaObject *obj : *board->objects()) {
                mChannelRoutes.append({channels[chIdx % channels.size()],
                                       obj->info()->id()});
                chIdx++;
            }
        }
    }

    // ★ 自动重启当前数据源(让板卡动画恢复)
    if (mActiveSource) {
        mStartFailed = false;
        mActiveSource->start();
        mStartBtn->setEnabled(false);
        mStopBtn->setEnabled(true);
        mTopStatusLabel->setText(tr("● 运行中"));
        mTopStatusLabel->setStyleSheet("color: #10b981; font-size: 13px;");
    }
    statusBar()->showMessage(tr("项目已加载: %1").arg(path), 5000);
}

void MainWindow::onAddWidget()
{
    QScadaBoard *board = nullptr;
    const auto boards = mController->getBoardListForDeviceIp("127.0.0.1");
    if (!boards.isEmpty()) {
        board = boards.first();
    }
    if (board == nullptr) {
        QMessageBox::warning(this, tr("添加图元"),
                             tr("没有可用的板卡，请先初始化设备。"));
        return;
    }

    // 收集所有已注册的 QML 图元
    QStringList widgetNames;
    QStringList widgetPaths;
    for ( QMLWidgetsConfig &group : QMLConfig::instance().QMLWidgets()) {
        for (QString &w : group.widgets()) {
            widgetNames << QString("[%1] %2").arg(group.info.groupTitle, w);
            widgetPaths << group.info.groupPath + w;
        }
    }
    if (widgetNames.isEmpty()) {
        QMessageBox::warning(this, tr("添加图元"),
                             tr("没有找到可用的 QML 图元。"));
        return;
    }

    // 简易对话框:选图元 + 位置
    QDialog dlg(this);
    dlg.setWindowTitle(tr("添加 QML 图元"));
    QFormLayout *form = new QFormLayout(&dlg);

    QComboBox *combo = new QComboBox(&dlg);
    combo->addItems(widgetNames);
    form->addRow(tr("图元类型:"), combo);

    QSpinBox *sbX = new QSpinBox(&dlg); sbX->setRange(0, 2000); sbX->setValue(20);
    QSpinBox *sbY = new QSpinBox(&dlg); sbY->setRange(0, 2000); sbY->setValue(20);
    QSpinBox *sbW = new QSpinBox(&dlg); sbW->setRange(50, 800); sbW->setValue(240);
    QSpinBox *sbH = new QSpinBox(&dlg); sbH->setRange(50, 800); sbH->setValue(180);
    QHBoxLayout *posRow = new QHBoxLayout();
    posRow->addWidget(new QLabel(tr("X:"))); posRow->addWidget(sbX);
    posRow->addWidget(new QLabel(tr("Y:"))); posRow->addWidget(sbY);
    posRow->addWidget(new QLabel(tr("W:"))); posRow->addWidget(sbW);
    posRow->addWidget(new QLabel(tr("H:"))); posRow->addWidget(sbH);
    form->addRow(tr("位置 / 大小:"), posRow);

    QDialogButtonBox *btns = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
    form->addRow(btns);
    connect(btns, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(btns, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    if (dlg.exec() != QDialog::Accepted) return;

    const int idx = combo->currentIndex();
    const QString qmlPath = widgetPaths.at(idx);
    const QString widgetLabel = combo->currentText();

    auto *info = new QScadaObjectInfo();
    info->setId(mNextWidgetId);
    info->setGeometry(QRect(sbX->value(), sbY->value(), sbW->value(), sbH->value()));
    info->setShowBackground(true);
    info->setType(QScadaObjectTypeQML);
    info->setUIResourcePath(qmlPath);

    board->createNewObject(info);
    statusBar()->showMessage(
        tr("已添加图元 #%1: %2").arg(mNextWidgetId).arg(widgetLabel), 5000);
    mNextWidgetId++;
}

// ============================================================
// 报警(Phase 3.0)
// ============================================================
void MainWindow::setupAlarm()
{
    // 1. 创建引擎
    mAlarmEngine = new AlarmEngine(this);

    // 2. 添加默认阈值规则(覆盖 4 个通道)
    AlarmRule r;

    r = { "temperature", 0,    80,  AlarmLevel::Warning,  true, "", false };
    mAlarmEngine->addRule(r);
    r = { "temperature", 0,    100, AlarmLevel::Critical, true, "", false };
    mAlarmEngine->addRule(r);

    r = { "pressure",    0,    75,  AlarmLevel::Warning,  true, "", false };
    mAlarmEngine->addRule(r);
    r = { "pressure",    0,    90,  AlarmLevel::Critical, true, "", false };
    mAlarmEngine->addRule(r);

    r = { "level",       0,    80,  AlarmLevel::Warning,  true, "", false };
    mAlarmEngine->addRule(r);
    r = { "level",       0,    95,  AlarmLevel::Critical, true, "", false };
    mAlarmEngine->addRule(r);

    r = { "flow",        0,    70,  AlarmLevel::Warning,  true, "", false };
    mAlarmEngine->addRule(r);
    r = { "flow",        0,    90,  AlarmLevel::Critical, true, "", false };
    mAlarmEngine->addRule(r);

    // 3. 把 3 个数据源的 valueProduced 路由到报警引擎
    connect(mSimulator, &DataSource::valueProduced,
            mAlarmEngine, &AlarmEngine::updateValue);
    connect(mModbus,    &DataSource::valueProduced,
            mAlarmEngine, &AlarmEngine::updateValue);
    connect(mModbusRtu, &DataSource::valueProduced,
            mAlarmEngine, &AlarmEngine::updateValue);

    // 4. 创建报警面板,包成 dock
    mAlarmPanel = new AlarmPanel(this);
    mAlarmPanel->setEngine(mAlarmEngine);

    mAlarmDock = new QDockWidget(tr("⚠ 报警列表"), this);
    mAlarmDock->setWidget(mAlarmPanel);
    mAlarmDock->setFeatures(QDockWidget::DockWidgetMovable
                            | QDockWidget::DockWidgetFloatable
                            | QDockWidget::DockWidgetClosable);
    mAlarmDock->hide();
    addDockWidget(Qt::RightDockWidgetArea, mAlarmDock);

    // 5. 创建弹窗通知器
    mAlarmNotifier = new AlarmNotifier(this);

    // 6. 连接报警信号 → 面板 & 通知器
    connect(mAlarmEngine, &AlarmEngine::alarmTriggered,
            mAlarmPanel,  &AlarmPanel::appendEvent);
    connect(mAlarmEngine, &AlarmEngine::alarmTriggered,
            mAlarmNotifier, &AlarmNotifier::onAlarmTriggered);
    connect(mAlarmEngine, &AlarmEngine::alarmResolved,
            mAlarmNotifier, &AlarmNotifier::onAlarmResolved);
}

void MainWindow::onAlarmToggleClicked(bool checked)
{
    mAlarmDock->setVisible(checked);
}
