#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QFileInfo>
#include <QStyle>
#include <QSignalBlocker>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_seekBarDown(false)
{
    ui->setupUi(this);

    setWindowTitle(tr("简易视频播放器"));
    resize(900, 650);
    setMinimumSize(600, 400);

    setupUI();
    setupConnections();
    applyStyles();
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
    m_forwardButton = ui->forwardButton;
    m_backwardButton = ui->backwardButton;
    m_positionSlider = ui->positionSlider;
    m_positionLabel = ui->positionLabel;
    m_durationLabel = ui->durationLabel;

    // 初始化控件状态
    m_playButton->setEnabled(false);
    m_stopButton->setEnabled(false);
    m_forwardButton->setEnabled(false);
    m_backwardButton->setEnabled(false);
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
    connect(m_forwardButton, &QPushButton::clicked, this, &MainWindow::seekForward);
    connect(m_backwardButton, &QPushButton::clicked, this, &MainWindow::seekBackward);

    // 连接进度条信号
    connect(m_positionSlider, &QSlider::valueChanged,
            this, &MainWindow::onPositionSliderValueChanged);
    connect(m_positionSlider, &QSlider::sliderPressed,
            this, &MainWindow::onSliderPressed);
    connect(m_positionSlider, &QSlider::sliderReleased,
            this, &MainWindow::onSliderReleased);

    // 连接媒体播放器信号
    connect(m_mediaPlayer, &QMediaPlayer::positionChanged,
            this, &MainWindow::positionChanged);
    connect(m_mediaPlayer, &QMediaPlayer::durationChanged,
            this, &MainWindow::durationChanged);
}

void MainWindow::applyStyles()
{
    // 设置主窗口样式
    setStyleSheet(
        "QMainWindow {"
        "    background-color: #1a1a1a;"
        "}"
        ""
        "QWidget {"
        "    background-color: #1a1a1a;"
        "    color: #e0e0e0;"
        "    font-family: Microsoft YaHei, Arial;"
        "    font-size: 13px;"
        "}"
        ""
        "QMenuBar {"
        "    background-color: #2a2a2a;"
        "    color: #e0e0e0;"
        "    border-bottom: 1px solid #3a3a3a;"
        "    padding: 4px;"
        "}"
        ""
        "QStatusBar {"
        "    background-color: #2a2a2a;"
        "    color: #999999;"
        "    border-top: 1px solid #3a3a3a;"
        "}"
        ""
        "QVideoWidget {"
        "    background-color: #000000;"
        "}"
        ""
        "QPushButton {"
        "    background-color: #3a3a3a;"
        "    color: #e0e0e0;"
        "    border-radius: 4px;"
        "    padding: 6px 12px;"
        "    border: 1px solid #4a4a4a;"
        "    min-width: 70px;"
        "}"
        ""
        "QPushButton:hover {"
        "    background-color: #4a4a4a;"
        "    border-color: #5a5a5a;"
        "}"
        ""
        "QPushButton:pressed {"
        "    background-color: #2a2a2a;"
        "}"
        ""
        "QPushButton:disabled {"
        "    background-color: #2a2a2a;"
        "    color: #666666;"
        "    border-color: #3a3a3a;"
        "}"
        ""
        "QSlider::groove:horizontal {"
        "    background-color: #3a3a3a;"
        "    height: 6px;"
        "    border-radius: 3px;"
        "}"
        ""
        "QSlider::sub-page:horizontal {"
        "    background-color: #4a90d9;"
        "    border-radius: 3px;"
        "}"
        ""
        "QSlider::handle:horizontal {"
        "    background-color: #e0e0e0;"
        "    width: 14px;"
        "    height: 14px;"
        "    margin: -4px 0;"
        "    border-radius: 7px;"
        "}"
        ""
        "QSlider::handle:horizontal:hover {"
        "    background-color: #ffffff;"
        "}"
        ""
        "QLabel {"
        "    color: #aaaaaa;"
        "    font-size: 12px;"
        "}"
        );
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
        m_forwardButton->setEnabled(true);
        m_backwardButton->setEnabled(true);
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
        statusBar()->showMessage(tr("已暂停"));
        break;
    case QMediaPlayer::PausedState:
    case QMediaPlayer::StoppedState:
        m_mediaPlayer->play();
        m_playButton->setText(tr("暂停"));
        statusBar()->showMessage(tr("正在播放"));
        break;
    }
}

void MainWindow::stop()
{
    m_mediaPlayer->stop();
    m_playButton->setText(tr("播放"));
    m_positionSlider->setValue(0);
    m_positionLabel->setText("00:00");
    statusBar()->showMessage(tr("已停止"));
}

void MainWindow::seekForward()
{
    // 快进10秒
    qint64 currentPosition = m_mediaPlayer->position();
    qint64 duration = m_mediaPlayer->duration();
    qint64 newPosition = qMin(currentPosition + 10000, duration);
    m_mediaPlayer->setPosition(newPosition);
    statusBar()->showMessage(tr("快进至 %1").arg(m_positionLabel->text()));
}

void MainWindow::seekBackward()
{
    // 快退10秒
    qint64 currentPosition = m_mediaPlayer->position();
    qint64 newPosition = qMax(currentPosition - 10000, static_cast<qint64>(0));
    m_mediaPlayer->setPosition(newPosition);
    statusBar()->showMessage(tr("快退至 %1").arg(m_positionLabel->text()));
}

void MainWindow::onPositionSliderValueChanged(int value)
{
    // 拖动时不处理
    if (m_seekBarDown) {
        return;
    }
    doSeek(value);
}

void MainWindow::onSliderPressed()
{
    m_seekBarDown = true;
}

void MainWindow::onSliderReleased()
{
    m_seekBarDown = false;
    doSeek(m_positionSlider->value());
}

void MainWindow::doSeek(int value)
{
    // 计算目标位置
    qint64 duration = m_mediaPlayer->duration();
    if (duration <= 0) {
        return;
    }

    qint64 targetPosition = static_cast<qint64>(
        (static_cast<double>(value) / 100.0) * duration);
    m_mediaPlayer->setPosition(targetPosition);
}

void MainWindow::positionChanged(qint64 position)
{
    qint64 duration = m_mediaPlayer->duration();
    if (duration <= 0) {
        return;
    }

    // 拖动时不更新
    if (m_seekBarDown) {
        return;
    }

    // 计算进度条值
    int sliderValue = static_cast<int>((static_cast<double>(position) / duration) * 100);

    // 临时阻塞信号，防止循环
    {
        QSignalBlocker blocker(m_positionSlider);
        m_positionSlider->setValue(sliderValue);
    }

    // 更新时间标签
    updatePositionLabel(position);
}

void MainWindow::durationChanged(qint64 duration)
{
    // 更新时间标签
    updateDurationLabel(duration);
}

void MainWindow::updatePositionLabel(qint64 position)
{
    qint64 seconds = position / 1000;
    qint64 minutes = seconds / 60;
    seconds = seconds % 60;

    QString timeStr = QString("%1:%2")
                          .arg(minutes, 2, 10, QChar('0'))
                          .arg(seconds, 2, 10, QChar('0'));

    m_positionLabel->setText(timeStr);
}

void MainWindow::updateDurationLabel(qint64 duration)
{
    qint64 seconds = duration / 1000;
    qint64 minutes = seconds / 60;
    seconds = seconds % 60;

    QString timeStr = QString("%1:%2")
                          .arg(minutes, 2, 10, QChar('0'))
                          .arg(seconds, 2, 10, QChar('0'));

    m_durationLabel->setText(timeStr);
}

