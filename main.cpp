#include "mainwindow.h"
#include <QApplication>
#include <QLoggingCategory>

int main(int argc, char *argv[])
{
    QLoggingCategory::setFilterRules(QStringLiteral("qt.bluetooth* = true"));

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
