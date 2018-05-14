#include "mainwindow.h"
#include "materiagateway.h"
#include "materiagatewaythread.h"
#include <QApplication>
#include <QSettings>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QSettings settings("settings.ini", QSettings::IniFormat);
    auto ip = settings.value("ip", "").toString();
    auto port = settings.value("port", "").toString();

    auto materiaGateway = new MateriaGateway(ip);
    auto thread = new MateriaGatewayThread();

    thread->start();
    materiaGateway->moveToThread(thread);

    //connect here

    MainWindow w;
    w.showMaximized();

    return a.exec();
}
