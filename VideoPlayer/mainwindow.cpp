#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QFileInfo>

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
    // 连接按钮信号
    connect(m_openButton, &QPushButton::clicked, this, &MainWindow::openFile);
    connect(m_playButton, &QPushButton::clicked, this, &MainWindow::play);
    connect(m_stopButton, &QPushButton::clicked, this, &MainWindow::stop);
}

void MainWindow::openFile()
{
    QString filePath = QFileDialog::getOpenFileName(
        this,
        tr("打开视频文件"),
        QDir::homePath(),
        tr("视频文件 (*.mp4 *.avi *.mkv *.mov *.wmv *.flv *.webm);;所有文件 (*.*)")
        );

    if (!filePath.isEmpty()) {
        m_mediaPlayer->setSource(QUrl::fromLocalFile(filePath));
        m_playButton->setEnabled(true);
        m_stopButton->setEnabled(true);
        m_positionSlider->setEnabled(true);
        m_playButton->setText(tr("播放"));
        statusBar()->showMessage(tr("已加载: %1").arg(QFileInfo(filePath).fileName()));
    }
}

void MainWindow::play()
{
    switch (m_mediaPlayer->playbackState()) {
    case QMediaPlayer::PlayingState:
        m_mediaPlayer->pause();
        m_playButton->setText(tr("播放"));
        break;
    case QMediaPlayer::PausedState:
    case QMediaPlayer::StoppedState:
        m_mediaPlayer->play();
        m_playButton->setText(tr("暂停"));
        break;
    }
}

void MainWindow::stop()
{
    m_mediaPlayer->stop();
    m_playButton->setText(tr("播放"));
    m_positionSlider->setValue(0);
    m_positionLabel->setText("00:00");
}
