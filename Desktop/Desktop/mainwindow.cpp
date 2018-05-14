#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setStyleSheet("background-color: #222222;");
    setWindowIcon(QIcon(":/icons/Icons/M.png"));
}

MainWindow::~MainWindow()
{
    delete ui;
}
