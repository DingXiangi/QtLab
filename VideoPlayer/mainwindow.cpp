#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(tr("简易视频播放器"));
    resize(800, 600);
}

MainWindow::~MainWindow()
{
    delete ui;
}
