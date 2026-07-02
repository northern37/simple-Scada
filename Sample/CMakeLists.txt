# ============================================================
# Sample 可执行文件（原 Sample.pro 的内容）
# ============================================================

# 主程序源文件
add_executable(QSimpleScadaSample
    main.cpp
    MainWindow.cpp
    MainWindow.h
    DataSource/IoSimulator.cpp
    DataSource/IoSimulator.h
    DataSource/ModbusTcpSource.cpp
    DataSource/ModbusTcpSource.h
    DataSource/ModbusRtuSource.cpp
    DataSource/ModbusRtuSource.h
    History/HistoryRecorder.cpp
    History/HistoryRecorder.h
    History/HistoryPanel.cpp
    History/HistoryPanel.h
    Alarm/AlarmEngine.cpp
    Alarm/AlarmEngine.h
    Alarm/AlarmPanel.cpp
    Alarm/AlarmPanel.h
    Alarm/AlarmNotifier.cpp
    Alarm/AlarmNotifier.h
)

# 链接核心库
target_link_libraries(QSimpleScadaSample PRIVATE
    QScadaLib
    Qt6::SerialBus
    Qt6::Sql
)

# QML 图元资源（原 QSimpleScadaWidgets.pri + QSimpleScadaExtensions.pri）
target_sources(QSimpleScadaSample PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../QSimpleScadaWidgets/widgets.qrc
    ${CMAKE_CURRENT_SOURCE_DIR}/../QSimpleScadaExtensions/extensions.qrc
)
