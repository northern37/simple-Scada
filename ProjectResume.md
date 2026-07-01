# QSimpleScada — 跨平台 SCADA 数据监控系统

**技术栈**：Qt 6.9 / C++17 / QML / Modbus TCP & RTU / SQLite / XML / CMake

---

## 项目概述

从零设计和实现了一套轻量级 SCADA（数据采集与监控）系统 Demo，支持多数据源接入、实时 QML 可视化、阈值报警和历史记录，具备完整的编辑/运行双模式工作流。

---

## 核心职责与成果

### 架构设计
- 以 `DataSource` 抽象基类 + Qt 信号-槽实现**数据源与 UI 解耦**
- 新协议（MQTT / OPC UA）只需继承基类、一行 `connect` 即可接入

### 多协议支持
- 基于 `QModbusTcpClient` / `QModbusRtuSerialMaster` 实现 Modbus TCP 和 RTU 双协议采集
- 寄存器归一化算法使上层图元无需感知底层协议差异

### QML 可视化引擎
- 将 8 种 SCADA 图元（仪表盘/趋势图/柱状图/水位计等）封装为 `QQuickWidget` + `QScadaObjectQML` 层级结构
- 支持运行时动态添加/删除

### 报警引擎（Phase 3.0）
- 设计多级阈值检测引擎（Warning / Critical），QMutex 线程安全
- 400ms 去重防抖 + 弹窗通知器，支持实时报警列表和颜色编码

### 历史记录
- 基于 SQLite + `QSqlQuery::prepare` 参数化查询，事务批量写入
- 屏蔽 SQL 注入风险

### 编辑模式
- 实现图元拖拽/缩放/网格吸附（10px 对齐）完整工作流
- 自定义光标状态机（掌状→闭手→缩放箭头）
- `eventFilter` + `grabMouse` 确保跨平台鼠标事件完整捕获

### 项目持久化
- 基于 XML 的 `.irp` 项目文件格式
- 完整支持板卡/图元/位置的序列化与反序列化

### Qt 6 迁移
- 将项目从 Qt 5 移植至 Qt 6.9
- 修复 `setMenuWidget` 与 `menuBar` 冲突等破坏性变更

---

## Keywords

`Qt 6` `C++` `SCADA` `Modbus` `QML` `SQLite` `实时数据` `信号-槽` `多线程` `XML 序列化`

---

## 项目难点与解决方案

### 难点 1：编辑模式下光标状态机混乱

**现象**：进入编辑模式后光标不变成掌状；拖拽时短暂变成闭手，松手后光标变回箭头而不是掌状；多次拖拽后光标行为完全不可预测。

**根因分析**：三个层次各写各的光标代码，互相打架——
1. `QScadaBoard::mouseMoveEvent` 每次移动都用 `setOverrideCursor(ArrowCursor)` 压栈，导致光标栈无限增长
2. `QScadaObject::mouseReleaseEvent` 用 `restoreOverrideCursor` 直接弹出栈顶，弹出的可能是 Board 压的 ArrowCursor 而不再是掌状
3. 进入编辑模式时用 `changeOverrideCursor` 在空栈上调用，Qt 无任何效果

**解决方案**：
- 统一改用 `changeOverrideCursor`（修改栈顶）替代 `setOverrideCursor`（压栈），杜绝栈无限增长
- Board 在 `mouseMoveEvent` 中加 `if (!mEditable)` 守卫，编辑模式下不再干预光标
- 进入编辑时 `setOverrideCursor(OpenHandCursor)` 显式创建栈帧，退出时 `restoreOverrideCursor` 清理
- `mouseReleaseEvent` 中用 `changeOverrideCursor(OpenHandCursor)` 恢复掌状而非 pop 栈

---

### 难点 2：QQuickWidget 拖拽/缩放事件劫持

**现象**：为 SCADA 图元（QQuickWidget 子控件）安装 `eventFilter` 后，`MouseButtonPress` 返回 `true` 吞掉事件，但 `MouseMove` 和 `MouseRelease` 再也收不到。

**根因分析**：`eventFilter` 中 `return true` 阻止了事件继续传播，Qt 的隐式鼠标抓取（implicit mouse grab）不会在过滤器中自动触发。没有抓取的 widget 只能在自己的几何区域内接收鼠标事件，拖拽一旦超出边界就断流。

**解决方案**：
- `MouseButtonPress` 分支中在 `return true` 前调用 `child->grabMouse()` 显式抓取
- `MouseButtonRelease` 分支中调用 `releaseMouse()` 释放抓取
- 这样无论鼠标拖到板卡任何位置甚至窗外，Move 事件都会通过 eventFilter 持续送达

---

### 难点 3：Modbus RTU 同步栈死锁

**现象**：Modbus RTU 连接 Modbus Slave 后，程序启动几秒就卡死，UI 完全不动。

**根因分析**：`QModbusReply::finished` 在同一事件循环迭代中同步触发。在 finished 槽中直接 `emit valueProduced(...)`，如果下游的 `onValueProduced` 链中有耗时操作（如 SQLite 写入），会阻塞事件循环，导致 Modbus 客户端的下一次轮询 timeout，形成连锁反应，最终栈溢出死锁。

**解决方案**：
- `setTimeout(5000)` + `setNumberOfRetries(0)` 给 RTU 足够宽松的超时
- 收到 finished 后立即 `disconnect(reply, &finished, ...)` 解绑
- 用 `QMetaObject::invokeMethod(this, [...], Qt::QueuedConnection)` 将 `emit valueProduced` 推迟到下一事件循环 — 让 reply 先完成生命周期，再异步分发数据
- 增加 `m_stopping` atomic 标志，停止时拒绝过期 reply 进入解析流程

---

### 难点 4：加载项目后 SIGSEGV 崩溃

**现象**：保存 `.irp` 项目 → 加载 → 程序段错误崩溃，GDB 指向 QScadaObject 析构。

**根因分析**（三重根因）：
1. **XML 结构错误**：`qscadaconnecteddeviceinfo.cpp` 中 `XMLFromDeviceInfo()` 把 `</device>` 写在 `<boards>` 之前，导致 XML 嵌套关系错乱，反序列化时解析出错误的层级
2. **野指针**：`QScadaObjectQML::initFromQML` 中 `setInfo(info)` 把 `mInfo` 指向外部 `lConnectedDevceInfo` 的成员指针，`delete lConnectedDevceInfo` 后 `mInfo` 变成悬挂指针
3. **setMenuWidget 与 menuBar 冲突**：Qt 6 中 `QWidget::setMenuWidget(topBar)` 和 `QMainWindow::menuBar()->addMenu()` 同时存在会触发 SIGSEGV

**解决方案**：
1. 调整 XML 生成顺序，将 `</device>` 移到 `</boards>` 之后
2. 改为 `info()->setUIProperties()` — 只更新数据内容而非替换指针指向
3. 弃用 `menuBar()`，全部改为工具栏 `QPushButton::setMenu(QMenu*)` 实现文件菜单

---

### 难点 5：报警阈值与模拟波形不匹配

**现象**：温度（temperature）通道报警永远不会触发 — IoSimulator 生成的正弦波在 25~75°C 范围，而报警阈值设为 Warning > 80°C / Critical > 100°C，阈值始终在波形之外。

**解决方案**：调整 IoSimulator 的振幅和基线参数，使 4 个通道的波形都能跨越 Warning 和 Critical 两条线：

| 通道 | 原范围 | 新范围 | Warning 阈值 | Critical 阈值 |
|------|--------|--------|-------------|--------------|
| temperature | 25~75 | -5~105 | > 80 | > 100 |
| pressure | 20~80 | 0~100 | > 75 | > 90 |
| level | 30~90 | 0~100 | > 80 | > 95 |
| flow | 25~75 | 0~100 | > 70 | > 90 |

同时给每个通道不同的周期（6s/8s/10s/12s），让报警错峰触发，方便在 Demo 中逐个观察。

---

### 难点 6：数据源切换/加载后路由断裂

**现象**：切换数据源或加载项目后，SCADA 图元全部静止，不再显示实时数据。

**根因分析**：
1. 切换数据源：`onSourceChanged()` 停掉旧源但不起新源，且按钮状态（启动/停止）残留旧源的运行态
2. 加载项目：`onLoadProject()` 中 `openProject()` 重建了整个板卡和图元集合（ID 全部变化），但 `mChannelRoutes` 仍然是旧的 ID 路由表，`board->updateValue(oldId)` 找不到目标图元
3. 加载项目后数据源被 `teardownCurrentSource()` 停止，没有重新启动

**解决方案**：
1. 切换数据源后在 `onSourceChanged()` 末尾显式重置启动/停止按钮和顶栏状态
2. 加载项目后清空 `mChannelRoutes` 并重新扫描板卡上的图元，轮询分配通道
3. 加载项目末尾自动重启当前数据源

---

### 难点 7：Qt 5 → Qt 6 移植的破坏性变更

**问题**：原始项目基于 Qt 5，迁移到 Qt 6.9 遇到多个 API 破坏性变更：
- `QTextStream::setCodec()` 被移除，改用 `QStringConverter`
- `QVariant::type()` → `QVariant::typeId()`
- `QHash::insertMulti()` 废弃，改用 `QMultiHash`
- `setMenuWidget` 与 `menuBar()` 在 Qt 6 中冲突导致 SIGSEGV
- QML 引擎版本从 2.0 升级到 6.x，部分 import 路径变更
- 构建系统从 `.pro` 适配 MinGW 13.x 编译链

**解决方案**：逐一适配 API 变更；弃用 `menuBar()` 全面改用工具栏方案；保持兼容性最简原则，不引入 Qt 6 独占特性。

---

### 难点 8：SQLite 历史记录的查询性能与注入风险

**问题**：原始实现用字符串拼接构造 SQL（`"SELECT * FROM history WHERE channel = '" + filter + "'"`），存在 SQL 注入风险；批量写入逐条 INSERT 无事务，性能差。

**解决方案**：
- 所有查询改用 `QSqlQuery::prepare()` + 参数绑定，杜绝注入
- 写入端用 `mDb.transaction()` + `commit()` 包裹，减少磁盘 IO
- 移除 `const_cast` 对 const 引用取地址的未定义行为
- HistoryRecorder 加缓冲区 + 定时 flush，避免每秒多次磁盘写入

---

## 面试常见问题与解答

### Q1：DataSource 的设计模式是什么？为什么不用接口（纯虚类）？

**答**：DataSource 是**模板方法模式 + 策略模式**的组合。基类定义 `valueProduced` 信号和 `start()/stop()/name()` 纯虚接口，每个协议（IoSimulator、ModbusTCP、ModbusRTU）是具体策略。选择非纯虚基类而非接口的原因是 C++ 没有 interface 关键字，抽象类 + 信号（需要 QObject 的 moc 支持）是最直接的 Qt 惯用写法。使用者只需 `connect(source, &DataSource::valueProduced, ...)` 接入，新增协议零向下修改。

---

### Q2：SCADA 图元是如何收到实时数据的？描述完整数据链路。

**答**：三层链路：
1. **数据源层**：IoSimulator 用 `QTimer` 每秒计算正弦波值，Modbus 轮询 Holding Register，都归一化后 `emit valueProduced(channelName, value)`
2. **路由层（MainWindow）**：构造函数中 `connect(source, &DataSource::valueProduced, ...)` 一入三出 — 分别连到 `onValueProduced`（图元）、`HistoryRecorder`（SQLite）、`AlarmEngine`（报警）
3. **分发层**：`onValueProduced` 遍历 `QList<QPair<QString,int>> mChannelRoutes`，channelName 匹配则 `board->updateValue(objectId, value)` → QML 属性绑定自动刷新 UI

**关键设计决策**：用 `QList<QPair>` 而非 `QHash`，因为 temperature 一个通道驱动 3 个图元（Linear + TrendChart + ValueLabel），`QHash` 同 key 覆盖会丢目标。

---

### Q3：为什么 Modbus RTU 比 TCP 更容易死锁？

**答**：TCP 有 IP 层的流控和重传，物理层稳定；RTU 通过串口（9600bps 低速率），帧间隙小、易受干扰。`QModbusReply::finished` 默认同步触发，在槽中直接 `emit valueProduced` 会阻塞事件循环，导致下一帧超时 → 重试 → 栈递归。解决方案是在 finished 槽中延迟 emit（`QMetaObject::invokeMethod` + `QueuedConnection`），确保 reply 的清理完成后再分发数据。

---

### Q4：如何处理 SQL 注入问题？

**答**：所有 SQL 操作统一用 `QSqlQuery::prepare()` 带 `?` 占位符 + `addBindValue()` 绑定参数，Qt 的 SQL 驱动会自动转义和类型校验。同时用 `QSqlDatabase::transaction()/commit()` 保证批量 INSERT 的原子性和性能。

---

### Q5：编辑模式的 eventFilter 为什么需要 grabMouse？

**答**：`eventFilter` 中 `return true` 吞掉事件后，Qt 不会为子控件触发隐式鼠标抓取。没有抓取的 widget 只能在几何区域内接收鼠标事件 — 拖拽超过边界后 MouseMove/MouseRelease 事件消失，缩放杆永远卡在第一次 press 的位置。`grabMouse()` 显式声明全局鼠标焦点，松手后 `releaseMouse()` 归还，确保完整拖拽流。

---

### Q6：光标状态机如何设计？为什么不用 setOverrideCursor？

**答**：三层协作设计：
- **Board 层**：进入编辑模式时 `setOverrideCursor(OpenHandCursor)` 初始化栈帧；在 `mouseMoveEvent` 中加 `if (!mEditable)` 守卫，编辑模式下不干预
- **Object 层（悬停）**：`eventFilter` 中检测鼠标在右下角 20px 区域 → `changeOverrideCursor(SizeFDiagCursor)`，否则 → `changeOverrideCursor(OpenHandCursor)`
- **Object 层（拖拽）**：Press → `changeOverrideCursor(ClosedHandCursor)` + `grabMouse()`；Release → `changeOverrideCursor(OpenHandCursor)` + `releaseMouse()`

用 `changeOverrideCursor`（修改栈顶）而非 `setOverrideCursor`（重复压栈）防止光标栈无限增长导致行为不可预测。

---

### Q7：项目加载时为什么 mChannelRoutes 会失效？

**答**：`openProject()` 内部清空板卡并重新 `new QScadaObject` 创建图元，每个新对象获得新的 `info()->id()`。但之前的路由表 `[{"temperature", 0}, {"pressure", 1}, ...]` 还是旧 ID。此时 `board->updateValue(0, value)` 找不到 ID=0 的旧图元，数据无法分发。修复方式：加载后清空路由表并重新扫描板卡，按固定顺序轮询分配通道。

---

### Q8：XML 序列化有什么坑？

**答**：最大教训是 XML 嵌套顺序 — 如果设备级关闭标签 `</device>` 写在 `<boards>` 子节点关闭之前，解析器会把整个 `<boards>` 块当作 `<device>` 的直接子节点而非独立层级，导致反序列化时对象层级错乱、析构时 SIGSEGV。解决方案是严格按打开顺序的逆序关闭标签。

---

### Q9：IoSimulator 的精度是怎么保证的？

**答**：每次 `onTick()` 直接基于 `mTick` 计数器计算 `sin(2π·mTick/period)`，而非累加 timeDelta。累加方式会因每 tick 浮点舍入误差累积，长时间运行后周期漂移。重置 `mTick++` 保证每次计算都是绝对相位，无累积误差。

---

### Q10：如果让你重新设计这个项目，你会做什么不同的决策？

**答**：
1. **路由持久化**：将 `channelName` 作为图元属性存入 `.irp` 文件，加载项目时恢复精确路由而非轮询猜测
2. **数据源工厂模式**：用注册表（registry）代替 MainWindow 中硬编码 switch-case，新增协议只需一行 `REGISTER_SOURCE(MqttSource)`
3. **单元测试**：DataSource、AlarmEngine、HistoryRecorder 都是纯逻辑层，非常适合写 Qt Test
4. **报警确认机制**：AlarmPanel 中增加"确认/静音"交互，从简单的触发-清除升级为工业标准的 NAMUR 报警模型
5. **图元属性编辑**：双击图元弹出属性面板（颜色、阈值、通道绑定），提升 Demo 的完整度
