#include "mainwindow.h"
#include "materiagateway.h"
#include "materiagatewaythread.h"
#include "Models/strategydatamodel.h"
#include <QApplication>
#include <QSettings>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QFile file(":/qdarkstyle/style.qss");
    file.open(QFile::ReadOnly);

    QString style = file.readAll();
    a.setStyleSheet(style);

    QSettings settings("settings.ini", QSettings::IniFormat);
    auto ip = settings.value("ip", "").toString();
    auto port = settings.value("port", "").toString();

    auto materiaGateway = new MateriaGateway(ip);
    auto thread = new MateriaGatewayThread();

    auto strategyModel = new StrategyDataModel(*materiaGateway);

    materiaGateway->moveToThread(thread);
    strategyModel->moveToThread(thread);

    MainWindow w(nullptr, *strategyModel);
    w.showMaximized();

    QObject::connect(thread, SIGNAL(started()), strategyModel, SLOT(init()));
    thread->start();

    return a.exec();
}
