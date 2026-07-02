# QSimpleScada — 跨平台 SCADA 数据监控系统

> 基于 [IndeemaSoftware/QSimpleScada](https://github.com/IndeemaSoftware/QSimpleScada) 改造的 SCADA 上位机 Demo。
> 整合了 [EEIoT](https://github.com/IndeemaSoftware/EEIoT) QML 图元集和 [QDashBoard](https://github.com/IndeemaSoftware/QDashBoard) 仪表盘框架思路，
> 新增报警引擎、编辑模式、项目持久化等功能。

**技术栈**：Qt 6.9 / C++17 / QML / Modbus TCP & RTU / SQLite / CMake

---

## 1. 功能概览

- **8 种 SCADA 图元**：Knob / Linear / WaterLevel / TrendChart / PieChart / BarGraph / ValueLabel × 2
- **3 种数据源**：IoSimulator（正弦波模拟）/ Modbus TCP / Modbus RTU
- **报警引擎**（Phase 3.0）：Warning / Critical 双级阈值，QMutex 线程安全，弹窗通知 + 实时报警列表
- **SQLite 历史记录**：`QSqlQuery::prepare` 参数化防注入，事务批量写入，支持搜索/过滤/CSV 导出
- **编辑模式**：图元拖拽/缩放/网格吸附（10px），自定义光标状态机（掌状→闭手→缩放箭头），`eventFilter` + `grabMouse` 跨平台事件捕获
- **项目持久化**：XML 格式 `.irp` 项目文件，完整支持板卡/图元/位置序列化与反序列化
- **双构建系统**：qmake（`.pro`）+ CMake（`CMakeLists.txt`），C++ 源码零改动

---

## 2. 数据流架构

```
┌───────────────────────────────────────────────────────────────────────┐
│                          MainWindow (QMainWindow)                     │
│  ToolBar: 数据源切换 │ 启动/停止 │ 编辑模式 │ 添加图元 │ 文件 │ 报警 │ │
├───────────────────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────────────────────┐  │
│  │                   QScadaBoardController                         │  │
│  │  ┌──────────┬──────────┬─────────────┬──────────┐               │  │
│  │  │ Linear   │ Knob     │ TrendChart  │ WaterLevel│              │  │
│  │  │ (temp)   │ (pressure)│ (temp)     │ (level)   │              │  │
│  │  ├──────────┼──────────┼─────────────┼──────────┤               │  │
│  │  │ ValLabel │ PieChart │ BarGraph    │ ValLabel  │              │  │
│  │  │ (flow)   │ (pressure)│ (flow)     │ (temp)    │              │  │
│  │  └──────────┴──────────┴─────────────┴──────────┘               │  │
│  └─────────────────────────────────────────────────────────────────┘  │
│  ┌──────────────────────┐  ┌──────────────────────┐                    │
│  │ AlarmPanel (dock)    │  │ HistoryPanel (dock)  │                    │
│  │ QTableView 实时列表   │  │ QTableView + 过滤/搜索│                   │
│  └──────────────────────┘  └──────────────────────┘                    │
└───────────────────────────────────────────────────────────────────────┘
         │                        │
         │  valueProduced ────────┤  valueProduced
         ▼                        ▼
  ┌──────────────┐    ┌────────────────────┐
  │ IoSimulator  │    │ AlarmEngine        │
  │ ModbusTCP    │    │  → 阈值比对         │
  │ ModbusRTU    │    │  → emit triggered  │
  └──────────────┘    │  → AlarmNotifier   │
                      └────────────────────┘
         │
         │  valueProduced
         ▼
  ┌────────────────────┐
  │ HistoryRecorder    │
  │  → SQLite INSERT   │
  │  → 事务批量写入    │
  └────────────────────┘

数据分发（MainWindow::setupSources + setupAlarm）:
  DataSource::valueProduced ──┬──→ onValueProduced → mChannelRoutes → board->updateValue (图元)
                              ├──→ HistoryRecorder::onValueProduced (SQLite)
                              └──→ AlarmEngine::updateValue (报警)
```

---

## 3. 目录结构

```
D:\QSimpleScada-Qt6Demo\
├── CMakeLists.txt                     # ★ CMake 构建（根）
├── Sample.pro                         # qmake 构建（根，备选）
├── com_indeema_QSimpleScada.pri       # SCADA 库 qmake 片段
│
├── QScadaObject/                      # 图元基类 + QML 封装
├── QScadaBoard/                       # 板卡控制器 + 管理器
├── QScadaEntity/                      # 设备信息 + XML 序列化
├── QScadaDevice/                      # 设备信息结构
│
├── QSimpleScadaWidgets/               # EEIoT QML 图元集
│   └── EEIoT/                         #   Linear / Knob / WaterLevel
├── QSimpleScadaExtensions/            # 自定义 QML 扩展图元
│   ├── ValueLabel.qml
│   ├── TrendChart.qml
│   ├── PieChart.qml
│   └── BarGraph.qml
│
├── QDashBoardApp/                     # 原版仪表盘（独立项目，不编译）
│
└── Sample/                            # ★ 主 Demo 项目
    ├── CMakeLists.txt                 #   CMake 构建
    ├── Sample.pro                     #   qmake 构建
    ├── main.cpp
    ├── MainWindow.h / .cpp            #   主窗口 + 图元初始化 + 路由 + 编辑
    │
    ├── DataSource/
    │   ├── DataSource.h               #   抽象基类
    │   ├── IoSimulator.h / .cpp       #   正弦波模拟
    │   ├── ModbusTcpSource.h / .cpp   #   Modbus TCP
    │   └── ModbusRtuSource.h / .cpp   #   Modbus RTU
    │
    ├── History/
    │   ├── HistoryRecorder.h / .cpp   #   SQLite 批量写入 + 事务
    │   └── HistoryPanel.h / .cpp      #   QTableView 历史面板
    │
    └── Alarm/
        ├── AlarmEngine.h / .cpp       #   阈值检测引擎（QMutex）
        ├── AlarmPanel.h / .cpp        #   报警列表面板
        └── AlarmNotifier.h / .cpp     #   弹窗通知器
```

---

## 4. 编译

### 4.1 依赖

| 组件 | 说明 |
|------|------|
| Qt | 6.9.3 MinGW 64-bit |
| Qt 模块 | `Core Gui Widgets Qml Quick QuickWidgets SerialBus Sql` |
| CMake | ≥ 3.16（随 Qt 安装，位于 `Qt/Tools/CMake_64/`） |
| MinGW | 13.1.0（随 Qt 安装） |
| VSPD | 虚拟串口驱动（可选，仅 Modbus RTU 需要） |
| Modbus Slave | 测试工具（可选） |

### 4.2 CMake 构建（推荐）

```powershell
# 配置
cmake -G "MinGW Makefiles" ^
    -DCMAKE_PREFIX_PATH=D:\QT\6.9.3\mingw_64 ^
    -S . -B build

# 编译
cmake --build build --parallel 4

# 运行
.\build\Sample\QSimpleScadaSample.exe
```

### 4.3 qmake 构建（兼容保留）

1. Qt Creator 打开 `Sample/Sample.pro`
2. 选择 Kit `Desktop Qt 6.9.3 MinGW 64-bit`
3. `Ctrl+B` 构建，`Ctrl+R` 运行

---

## 5. 报警系统（Phase 3.0）

### 默认阈值规则

| 通道 | Warning 条件 | Critical 条件 |
|------|-------------|--------------|
| temperature | > 80 | > 100 |
| pressure | > 75 | > 90 |
| level | > 80 | > 95 |
| flow | > 70 | > 90 |

### 报警数据流

```
IoSimulator/Modbus → valueProduced → AlarmEngine::updateValue
  → 遍历 mRules 比对阈值
    → 越限且未触发 → set triggered=true → emit alarmTriggered(event)
      → AlarmPanel::appendEvent       （表格追加行，黄色/红色标记）
      → AlarmNotifier::onAlarmTriggered（QMessageBox 弹窗，3.5s 自动消失）
    → 恢复正常 → emit alarmResolved(channel)
```

---

## 6. 数据源配置

### 6.1 IoSimulator（默认）

下拉框选 `IoSimulator (模拟)`，点 `▶ 启动`，4 通道正弦波自动生成。

当前波形范围：

| 通道 | 范围 | 周期 |
|------|------|------|
| temperature | -5 ~ 105 | 8s |
| pressure | 0 ~ 100 | 10s |
| level | 0 ~ 100 | 12s |
| flow | 0 ~ 100 | 6s |

### 6.2 Modbus TCP

1. Modbus Slave → Connection → TCP/IP → `127.0.0.1:502`
2. Setup → Slave Definition → Function 03, Address 0, Quantity 4
3. Slave 端写入 0~65535 的 uint16，归一化公式：`value = raw / 655.35` → 0~100

### 6.3 Modbus RTU

1. VSPD 创建虚拟串口对：`COM1 ↔ COM2`
2. Modbus Slave → Connection → Serial Port → `COM1`, 9600-8-N-1, Mode=RTU
3. **关键**：Error Simulation 三个选项全部取消勾选（Skip response / CRC error / Busy）
4. Qt 端默认连接 `COM2`（`ModbusRtuSource.h` 中配置）

---

## 7. 编辑模式使用说明

1. 工具栏点击 **✎ 编辑模式** 切换进入
2. **拖拽**：按图元中间区域拖动 → 掌状变闭手，松手恢复掌状
3. **缩放**：按图元右下角 20px 区域拖动 → 变为对角箭头
4. **吸附**：松手时自动对齐到 10px 网格
5. **添加图元**：工具栏 "添加图元" 按钮，选择 QML 类型和位置
6. **保存/加载**：文件菜单 → 保存项目（.irp）/ 加载项目
7. 再次点击编辑模式退出，恢复运行态光标

---

## 8. 常见问题排查

### Modbus RTU 相关

| 现象 | 排查 |
|------|------|
| 持续 `Request timeout` | VSPD 桥接不通 / 串口号错 / Slave 未 Connect |
| `Modbus Exception 0x06` | Slave 的 "Return exception 06, Busy" 被勾选，取消它 |
| `Wrong CRC` | Slave 的 "Insert CRC/LRC error" 被勾选，取消它 |
| 启动即卡死 | `QModbusReply::finished` 同步死锁 → 用 `QueuedConnection` 延迟 emit |

### 其他

| 现象 | 排查 |
|------|------|
| 加载项目后图元不刷新 | 尝试切换编辑模式再退出即可 |
| 报警按钮按了没反应 | dock 面板初始隐藏，点击后从底部弹出 |
| 温度通道不报警 | 确认 IoSimulator 数据源正在运行（波形范围 -5~105，约每 8s 触发一次） |

---

## 9. 关键设计决策

| 决策 | 原因 |
|------|------|
| `QList<QPair>` 而非 `QHash` 做路由表 | temperature 驱动 3 个图元，`QHash` 同 key 覆盖丢目标 |
| `changeOverrideCursor` 而非 `setOverrideCursor` | 防止光标栈无限增长 |
| `grabMouse()` 在 eventFilter 中显式调用 | `return true` 吞掉事件后 Qt 隐式抓取不生效 |
| `QueuedConnection` 延迟 emit | Modbus reply 同步触发时防死锁 |
| `QSqlQuery::prepare` + 事务 | 防 SQL 注入 + 批量写入性能 |
| CMake 静态库 `QScadaLib` | 避免将 SCADA 源码重复编译到每个 target |

---

## 10. Phase 进度

- [x] Phase 1.1  Qt 5 → Qt 6 迁移
- [x] Phase 1.2  库静态编译
- [x] Phase 1.3  EEIoT QML 图元整合
- [x] Phase 1.4  自定义扩展 QML（TrendChart / PieChart / BarGraph / ValueLabel）
- [x] Phase 2.1  8 图元 + IoSimulator 数据源
- [x] Phase 2.2  SQLite 历史记录
- [x] Phase 2.3  编辑模式（拖拽/缩放/网格吸附/项目持久化）
- [x] Phase 2.4  Modbus TCP + Modbus RTU 双协议
- [x] Phase 3.0  报警引擎（阈值检测 + 面板 + 弹窗通知）
- [x] 补充     CMake 构建系统 + 面试文档

---

## 11. License

本项目基于 MIT-style 开源项目二次开发，用于个人学习与求职展示。
所有第三方库版权归原作者所有。
