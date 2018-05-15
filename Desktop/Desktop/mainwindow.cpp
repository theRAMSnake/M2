#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDockWidget>
#include <QListWidget>
#include <QStyleFactory>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/icons/Icons/M.png"));

    QDockWidget *dock = new QDockWidget(tr("Output"), this);
    dock->setAllowedAreas(Qt::BottomDockWidgetArea);

    QListWidget *output = new QListWidget();
    dock->setWidget(output);
    dock->setFeatures(QDockWidget::DockWidgetFeature::NoDockWidgetFeatures);
    addDockWidget(Qt::BottomDockWidgetArea, dock);

    output->addItems(QStyleFactory::keys());
}

MainWindow::~MainWindow()
{
    delete ui;
}
