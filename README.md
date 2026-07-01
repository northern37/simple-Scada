# QSimpleScada 上位机 · Qt 6 适配项目

> 基于 [IndeemaSoftware/QSimpleScada](https://github.com/IndeemaSoftware/QSimpleScada) 改造的 SCADA 上位机 demo,目标平台 Qt 6.9.3/6.10.2 MinGW。
>
> 适配过程中整合了 [EEIoT](https://github.com/IndeemaSoftware/EEIoT) QML 图元集和 [QDashBoard](https://github.com/IndeemaSoftware/QDashBoard) 仪表盘框架的思路。

---

## 1. 功能概览

- 8 个 SCADA 图元实时显示(Knob / Linear / WaterLevel / TrendChart / PieChart / BarGraph / ValueLabel × 2)
- 3 种数据源:**IoSimulator(模拟)** / **Modbus TCP** / **Modbus RTU**
- SQLite 历史数据存储,支持按通道过滤、关键字搜索、CSV 导出
- 8 图元 → 4 通道的**一对多**路由(`QList<QPair>`)
- Modbus Slave / VSPD 调试配置全流程(踩坑记录)

---

## 2. 架构

```
┌──────────────────────────────────────────────────────────────┐
│                       MainWindow (QMainWindow)               │
│  ┌────────────────────────────────────────────────────────┐  │
│  │  TopBar  │  ToolBar(数据源切换 / 启停 / 历史数据 dock)  │  │
│  └────────────────────────────────────────────────────────┘  │
│  ┌────────────────────────────────────────────────────────┐  │
│  │            QScadaBoardController                       │  │
│  │   ┌─────────┬─────────┬─────────────┬─────────┐         │  │
│  │   │ Linear  │  Knob   │ TrendChart  │WaterLev │         │  │
│  │   │   (0)   │   (1)   │    (2)      │   (3)   │         │  │
│  │   ├─────────┼─────────┼─────────────┼─────────┤         │  │
│  │   │ValLabel │ PieChar │  BarGraph   │ValLabel │         │  │
│  │   │   (4)   │   (5)   │    (6)      │   (7)   │         │  │
│  │   └─────────┴─────────┴─────────────┴─────────┘         │  │
│  └────────────────────────────────────────────────────────┘  │
│  ┌────────────────────────────────────────────────────────┐  │
│  │  QDockWidget(底部,默认隐藏) → HistoryPanel             │  │
│  │   QTableView + QSqlQueryModel + 过滤/搜索/导出          │  │
│  └────────────────────────────────────────────────────────┘  │
└──────────────────────────────────────────────────────────────┘
            │            │              │
            ▼            ▼              ▼
   ┌────────────┐ ┌─────────────┐ ┌──────────────┐
   │IoSimulator │ │ModbusTcpSrc │ │ModbusRtuSrc  │  ← DataSource
   │  (模拟)    │ │  (127.0.0.1 │ │  (COM2,RTU)  │
   └────────────┘ │     :502)   │ └──────────────┘
        │         └─────────────┘        │
        │ valueProduced(name, value)    │
        └──────────────┬────────────────┘
                       ▼
              ┌─────────────────┐
              │  onValueProduced│ ← MainWindow 路由
              │  mChannelRoutes │
              └─────────────────┘
                       │
                       ├─→ board->updateValue(id, value)   (SCADA 图元)
                       └─→ HistoryRecorder::onValueProduced → SQLite
```

---

## 3. 目录结构

```
D:\QSimpleScada-Qt6Demo\
├── com_indeema_QSimpleScada.pri      # SCADA 库源码 .pri
├── QSimpleScadaWidgets/              # EEIoT QML 图元(Knob/Linear/WaterLevel)
│   └── EEIoT/                        #   qmldir + qml + png
├── QSimpleScadaExtensions/           # 自定义 QML 图元
│   ├── ValueLabel.qml
│   ├── TrendChart.qml
│   ├── PieChart.qml
│   └── BarGraph.qml
├── QDashBoardApp/                    # 原版 dashboard(独立项目,不参与编译)
└── Sample/                           # ★ 主项目
    ├── Sample.pro
    ├── main.cpp
    ├── MainWindow.h / .cpp           # 主窗口 + SCADA 板卡 + 路由
    ├── DataSource/
    │   ├── DataSource.h              #   抽象基类
    │   ├── IoSimulator.h / .cpp      #   模拟(正弦波)
    │   ├── ModbusTcpSource.h / .cpp  #   QModbusTcpClient
    │   └── ModbusRtuSource.h / .cpp  #   QModbusRtuSerialMaster
    └── History/                      # Phase 2.2
        ├── HistoryRecorder.h / .cpp  #   SQLite 后台写入(批量 + 2s flush)
        └── HistoryPanel.h / .cpp     #   QTableView + QSqlQueryModel
```

---

## 4. 编译

### 4.1 依赖

| 组件 | 版本 / 来源 |
|---|---|
| Qt | 6.9.3 或 6.10.2 MinGW 64-bit |
| Qt 模块 | `widgets network qml quick quickwidgets serialbus sql` |
| 编译器 | MinGW 13.x(随 Qt 安装) |
| VSPD | 虚拟串口驱动 6.9(可选,仅 RTU 需要) |
| Modbus Slave | 测试用 Slave 工具(中文版) |
| Modbus Poll | TCP 测试备选工具 |

### 4.2 编译步骤

1. 用 Qt Creator 打开 `D:\QSimpleScada-Qt6Demo\Sample\Sample.pro`
2. 选择 Kit:`Desktop Qt 6.9.3 MinGW 64-bit`(或 6.10.2)
3. 右键项目 → `Run qmake`
4. `Ctrl+B` 构建
5. `Ctrl+R` 运行

**注意**:`QDashBoardApp/` 是独立项目,不要作为 subdirs 加入,会因缺 `Qt Charts/Location` 模块而失败。

---

## 5. 8 图元布局与路由

```
┌─────────────┬─────────────┬───────────────┬─────────────┐
│ Linear  (0) │ Knob    (1) │ TrendChart(2) │ WaterLevel(3)│
│ temperature │ pressure    │ temperature   │ level        │
├─────────────┼─────────────┼───────────────┼─────────────┤
│ ValueLabel  │ PieChart (5)│ BarGraph   (6)│ ValueLabel   │
│   (4)       │ pressure    │ flow         │ (7)          │
│ flow        │             │              │ temperature  │
└─────────────┴─────────────┴───────────────┴─────────────┘
```

路由表(`MainWindow::mChannelRoutes`):

| 通道 | 路由目标 | 说明 |
|---|---|---|
| `temperature` | obj 0, 2, 7 | Linear + TrendChart + ValueLabel(右下) |
| `pressure` | obj 1, 5 | Knob + PieChart |
| `level` | obj 3 | WaterLevel |
| `flow` | obj 4, 6 | ValueLabel(左下) + BarGraph |

**注意**:用 `QList<QPair<QString,int>>` 而非 `QHash`,因为 `temperature` 等通道对应 3 个图元,`QHash` 会 key 覆盖。

---

## 6. 数据源配置

### 6.1 IoSimulator(默认,无需外部工具)

打开程序后下拉框选 `IoSimulator (模拟)`,点 `▶ 启动`,4 通道正弦波自动生成,8 图元实时动。

### 6.2 Modbus TCP

1. 打开 [Modbus Poll](https://www.modbustools.com/modbus_poll.html) 或 Modbus Slave
2. Connection → Connect → TCP/IP
3. IP `127.0.0.1`,Port `502`,Mode `TCP`
4. Setup → Slave Definition → Function 03,Address 0,Quantity 8(填入 8 个 uint16)
5. Qt 端下拉框选 `Modbus TCP (127.0.0.1:502)`,点 `▶ 启动`

**Slave 端数值会被 Qt 归一化**:`double(raw) / 655.35` → 0~100,所以 Slave 端写 0~65535 对应 0~100。

### 6.3 Modbus RTU(踩坑最多,务必按下面配置)

#### 6.3.1 VSPD 配对

1. 打开 VSPD → Manage ports
2. 添加虚拟端口对:`COM1 ↔ COM2`
3. 波特率 `9600`,Standard pinout
4. 状态栏显示 `COM1 [9600-N-8-1]` ↔ `COM4 [9600-N-8-1]`(或更高编号)

#### 6.3.2 Modbus Slave 配置(关键!)

1. 启动 Modbus Slave
2. **菜单 `Connection → Connect...`**
3. 弹窗配置:
   - Connection: **`Serial Port`**(不是 `Modbus RTU Over TCP/IP`)
   - Port: **`COM1`**(Qt 接 COM2,Slave 接 COM1)
   - 9600 Baud / 8 Data bits / None Parity / 1 Stop Bit
   - Mode: **`RTU`**
   - Slave ID: `1`
4. **`Setup → Slave Definition`**,Function 03 / Address 0 / Quantity 8
5. **`Setup → Slave Definition` → Error Simulation 区域**:**三个选项全部取消勾选**:
   - ☐ Skip response
   - ☐ Insert CRC/LRC error
   - ☐ Return exception 06, Busy
6. 点 OK

#### 6.3.3 Qt 端配置

`ModbusRtuSource.h` 默认值:

```cpp
m_serialPort = "COM2";      // Qt 接 COM2(VSPD 配对的对端)
m_slaveAddr  = 1;
m_baudRate   = 9600;
m_parity     = 0;           // None
```

`ModbusRtuSource.cpp` 关键参数:

```cpp
m_client->setTimeout(5000);          // ★ RTU 必须长超时
m_client->setNumberOfRetries(0);     // ★ RTU 不能 retries
```

启动后,工具栏下拉框选 `Modbus RTU (COM2, 9600)`,点 `▶ 启动`。

状态栏变绿 `● 已连接 Modbus RTU`,8 图元开始动。

---

## 7. 常见问题排查

### 7.1 `Request timeout` 持续

| 现象 | 排查 |
|---|---|
| Output 一直 `Request timeout` | VSPD 桥接不通 / 串口名错 |
| Modbus Slave 状态栏无 `COM3 [...]` | Slave 没真正 Connect |
| 偶尔 timeout | 正常(总线干扰),可忽略 |

### 7.2 `Modbus Exception Response` + Exception code

- `0x01 IllegalFunction` → Slave 不支持 FC=03(几乎不会出现)
- `0x02 IllegalDataAddress` → Address / Quantity 越界
- `0x03 IllegalDataValue` → 写入值非法
- **`0x06 SlaveDeviceBusy`** → **`Return exception 06, Busy` 选项被勾选**,关掉它
- `0x04 SlaveDeviceFailure` → Slave 内部错

### 7.3 `Discarding response with wrong CRC`

**`Insert CRC/LRC error` 选项被勾选**,关掉它。

### 7.4 程序一连接就卡死

`QModbusReply::finished` 同步触发 + `emit` 在同步栈里,导致死锁。修复方式:
- `setTimeout(3000)` + `setNumberOfRetries(2)`
- 收到 finished 后 `disconnect(reply, &finished, this, nullptr)` 解绑
- `QMetaObject::invokeMethod(this, [..], Qt::QueuedConnection)` 异步 emit

### 7.5 TrendChart / BarGraph 空白

Canvas 渲染缓存 + 根 Rectangle 没填满父布局。修复:`renderTarget: Canvas.Image` + `anchors.fill: parent` + 加边框调试。

### 7.6 历史数据过滤不生效

- `QComboBox::addItems(QStringList)` **不设置 userData**,导致 `currentData()` 永远返回空
- 修复:用 `addItem(text, userData)` 显式设置

---

## 8. Phase 进度

- [x] Phase 1.1 Qt 5 → Qt 6 迁移
- [x] Phase 1.2 库静态编译
- [x] Phase 1.3 EEIoT QML 整合
- [x] Phase 1.4 自定义扩展 QML
- [x] Phase 2.1 8 图元 + IoSimulator
- [x] Phase 2.4.1 Modbus TCP
- [x] Phase 2.4.2 Modbus RTU
- [x] Phase 2.2 SQLite 历史
- [ ] Phase 2.3 告警系统(下一个)
- [ ] Phase 后续 MQTT(可选)

---

## 9. 协议层关键代码片段

### 9.1 Modbus RTU 一对多路由

```cpp
// MainWindow.cpp - onValueProduced
for (const auto &r : std::as_const(mChannelRoutes)) {
    if (r.first == name) {
        board->updateValue(r.second, value);
    }
}
```

### 9.2 Modbus RTU 异步 emit(防卡死)

```cpp
connect(reply, &QModbusReply::finished, this, [this, reply]() {
    m_pendingReplies.removeAll(reply);
    disconnect(reply, &QModbusReply::finished, this, nullptr);

    if (reply->error() != QModbusDevice::NoError) {
        // 错误处理 + Exception Code 日志
        reply->deleteLater();
        return;
    }

    const QModbusDataUnit unit = reply->result();
    // ... 解析 ...

    reply->deleteLater();

    // ★ 关键:QueuedConnection 解耦,避免同步栈死锁
    QMetaObject::invokeMethod(this, [this, values]() {
        for (int i = 0; i < values.size(); ++i) {
            emit valueProduced(m_channelNames[i], values[i]);
        }
    }, Qt::QueuedConnection);
});
```

### 9.3 SQLite 批量写入

```cpp
// HistoryRecorder.cpp - flush
if (!mDb.transaction()) { return; }
QSqlQuery q(mDb);
q.prepare("INSERT INTO history (ts, channel, value) VALUES (?, ?, ?)");
for (const auto &r : std::as_const(mBuffer)) {
    q.addBindValue(QDateTime::fromMSecsSinceEpoch(r.epochMs)
                       .toString("yyyy-MM-dd HH:mm:ss.zzz"));
    q.addBindValue(r.channel);
    q.addBindValue(r.value);
    q.exec();
}
mDb.commit();
```

---

## 10. License

本项目作为**个人学习 / 求职展示**用途,基于 MIT-style 开源项目二次开发。
所有第三方库版权归原作者所有。
