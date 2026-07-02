cmake_minimum_required(VERSION 3.16)
project(QSimpleScada VERSION 3.0 LANGUAGES CXX)

# Qt 6 要求
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Qt 6 模块
find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Qml
    Quick
    QuickWidgets
    SerialBus
    Sql
)

# ============================================================
# QScada 核心库（原 com_indeema_QSimpleScada.pri 的内容）
# ============================================================
add_library(QScadaLib STATIC
    QScadaObject/qscadaobject.cpp
    QScadaObject/qscadaobjectinfo.cpp
    QScadaObject/qscadaobjectinfodialog.cpp
    QScadaObject/qscadaobjectqml.cpp
    QScadaBoard/qscadaboard.cpp
    QScadaBoard/qscadaboardcontroller.cpp
    QScadaBoard/qscadaboardmanager.cpp
    QScadaEntity/qscadaconnecteddeviceinfo.cpp
    QScadaEntity/qscadabaseprefentity.cpp
    QScadaDevice/qscadadeviceinfo.cpp
    QScadaObject/qaxiswidget.ui
    QScadaObject/qscadaobjectinfodialog.ui
    com_indeema_QSimpleScada.qrc
)

target_include_directories(QScadaLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(QScadaLib PUBLIC
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickWidgets
)

# ============================================================
# Sample 子项目
# ============================================================
add_subdirectory(Sample)
