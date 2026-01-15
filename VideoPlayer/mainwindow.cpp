#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle(tr("简易视频播放器"));
    resize(900, 650);

    setupUI();
    setupConnections();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupUI()
{
    // 初始化媒体播放器
    m_mediaPlayer = new QMediaPlayer(this);
    m_videoWidget = ui->videoWidget;
    m_mediaPlayer->setVideoOutput(m_videoWidget);

    // 获取UI中的控件
    m_playButton = ui->playButton;
    m_openButton = ui->openButton;
    m_stopButton = ui->stopButton;
    m_positionSlider = ui->positionSlider;
    m_positionLabel = ui->positionLabel;
    m_durationLabel = ui->durationLabel;

    // 初始化控件状态
    m_playButton->setEnabled(false);
    m_stopButton->setEnabled(false);
    m_positionSlider->setEnabled(false);
    m_positionSlider->setRange(0, 100);
    m_positionSlider->setValue(0);
    m_positionLabel->setText("00:00");
    m_durationLabel->setText("00:00");

    // 更新状态栏
    statusBar()->showMessage(tr("欢迎使用简易视频播放器"));
}

void MainWindow::setupConnections()
{
    // 连接信号和槽将在后续步骤中添加
}
