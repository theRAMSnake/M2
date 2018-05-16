#include "mainwindow.h"
#include "logger.h"
#include "ui_mainwindow.h"
#include <QDockWidget>
#include <QPlainTextEdit>
#include <QStyleFactory>
#include <QScrollBar>
#include "Views/strategyview.h"

MainWindow::MainWindow(QWidget *parent, StrategyDataModel &strategyDataModel) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/icons/Icons/M.png"));

    QDockWidget *dock = new QDockWidget(tr("Output"), this);
    dock->setAllowedAreas(Qt::BottomDockWidgetArea);

    QPlainTextEdit *output = new QPlainTextEdit();
    dock->setWidget(output);
    dock->setFeatures(QDockWidget::DockWidgetFeature::NoDockWidgetFeatures);
    addDockWidget(Qt::BottomDockWidgetArea, dock);

    output->setReadOnly(true);
    connect(&Logger::instance(), &Logger::onNewMessage,
            [=]( auto msg ) {

        output->appendPlainText(msg.str);
        output->verticalScrollBar()->setValue(output->verticalScrollBar()->maximum());

    });

    setCentralWidget(new StrategyView(this, strategyDataModel));
}

MainWindow::~MainWindow()
{
    delete ui;
}
