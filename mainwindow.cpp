#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->widget->init_QGW();
    timer = new QTimer();
    connect(timer,SIGNAL(timeout()),this,SLOT(onTimeOut()));
    timer->start(20);
}

MainWindow::~MainWindow()
{
    timer->deleteLater();
    delete ui;
}

void MainWindow::onTimeOut()
{
    ui->widget->drawImage();
}
