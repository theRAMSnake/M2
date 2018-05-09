#include "mainwindow.h"
#include <QApplication>
#include <QSettings>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QSettings settings("settings.ini", QSettings::IniFormat);
    auto ip = settings.value("ip", "").toString();
    auto port = settings.value("port", "").toString();

    MainWindow w;
    w.show();

    return a.exec();
}
