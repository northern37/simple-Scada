QT       += widgets network qml quick quickwidgets serialbus sql

TARGET = QSimpleScadaSample
TEMPLATE = app

CONFIG += c++17

# ---- 链接 SCADA 库源码 ----
include($$PWD/../com_indeema_QSimpleScada.pri)

# ---- 集成 QSimpleScadaWidgets(EEIoT 图元集) ----
include($$PWD/../QSimpleScadaWidgets/QSimpleScadaWidgets.pri)

# ---- 集成 QSimpleScadaExtensions(我们自己的扩展图元) ----
include($$PWD/../QSimpleScadaExtensions/QSimpleScadaExtensions.pri)

# ---- Sample 自身源码 ----
HEADERS += \
    MainWindow.h \
    DataSource/IoSimulator.h \
    DataSource/ModbusTcpSource.h \
    DataSource/ModbusRtuSource.h \
    History/HistoryRecorder.h \
    History/HistoryPanel.h

SOURCES += \
    main.cpp \
    MainWindow.cpp \
    DataSource/IoSimulator.cpp \
    DataSource/ModbusTcpSource.cpp \
    DataSource/ModbusRtuSource.cpp \
    History/HistoryRecorder.cpp \
    History/HistoryPanel.cpp

# ---- 后续 Phase 启用 ----
# DataSource/MqttSource.h/cpp (需 QT += mqtt)
# Alarm/AlarmEngine.h/cpp
# Alarm/Notifier.h/cpp
