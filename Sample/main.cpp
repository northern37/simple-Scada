// QSimpleScada 上位机 — 程序入口

#include <QApplication>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    // Qt 6 默认开启 High-DPI
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName("QSimpleScadaDemo");
    QCoreApplication::setApplicationName("QSimpleScadaSample");

    MainWindow w;
    w.show();
    return app.exec();
}
