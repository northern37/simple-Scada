#-------------------------------------------------
# QSimpleScada 上位机 — 顶层工程
# 默认只编 Sample(完整 SCADA 上位机)
#
# QDashBoardApp 是独立 demo,与 SCADA 无依赖关系,
# 如需运行请单独打开 D:\QSimpleScada-Qt6Demo\QDashBoardApp\QDashBoard.pro
#-------------------------------------------------

TEMPLATE = subdirs

SUBDIRS  += Sample/Sample.pro

# 如需同时构建 QDashBoardApp,取消注释下一行:
# 注:QDashBoard 依赖 Qt Charts/Location/Positioning 模块,需在 Qt 安装时勾选
# SUBDIRS  += QDashBoardApp/QDashBoard.pro

CONFIG  += ordered
