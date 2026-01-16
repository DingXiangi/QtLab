#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <Qt>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QFileInfo>
#include <QStyle>
#include <QSignalBlocker>
#include <QScrollBar>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QDir>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_seekBarDown(false)
    , m_playlistModel(nullptr)
    , m_playlistDock(nullptr)
    , m_playlistView(nullptr)
    , m_addToPlaylistButton(nullptr)
    , m_removeFromPlaylistButton(nullptr)
    , m_clearPlaylistButton(nullptr)
    , m_historyModel(nullptr)
    , m_historyDock(nullptr)
    , m_historyView(nullptr)
    , m_removeFromHistoryButton(nullptr)
    , m_clearHistoryButton(nullptr)
    , m_historyTimer(nullptr)
    , m_lastSavedPosition(0)
    , m_pendingPosition(-1)
    , m_autoPlayAfterSeek(false)
{
    ui->setupUi(this);

    setWindowTitle(tr("简易视频播放器"));
    resize(900, 650);
    setMinimumSize(600, 400);

    setupUI();
    setupPlaylistUI();
    setupHistory();
    setupHistoryUI();
    setupConnections();
    applyStyles();

    // 加载保存的播放列表
    loadPlaylistFromFile();
    // 加载历史记录
    loadHistoryFromFile();
}

MainWindow::~MainWindow()
{
    // 保存当前播放进度
    recordCurrentPosition();
    // 保存历史记录
    saveHistoryToFile();
    delete ui;
}

void MainWindow::setupUI()
{
    // 初始化媒体播放器
    m_mediaPlayer = new QMediaPlayer(this);
    m_videoWidget = ui->videoWidget;
    m_audioOutput = new QAudioOutput(this);
    m_mediaPlayer->setVideoOutput(m_videoWidget);
    m_mediaPlayer->setAudioOutput(m_audioOutput);
    m_audioOutput->setVolume(1.0);  // 最大音量

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

    // 播放列表按钮初始状态（将在setupPlaylistUI中初始化）

    // 更新状态栏
    statusBar()->showMessage(tr("欢迎使用简易视频播放器"));
}

void MainWindow::setupPlaylistUI()
{
    // 创建播放列表模型
    m_playlistModel = new PlaylistModel(this);

    // 创建播放列表停靠窗口
    m_playlistDock = new QDockWidget(tr("播放列表"), this);
    m_playlistDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    m_playlistDock->setMinimumWidth(200);
    m_playlistDock->setMaximumWidth(350);
    m_playlistDock->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable);

    // 创建停靠窗口的内容部件
    QWidget *playlistContents = new QWidget(m_playlistDock);
    QVBoxLayout *playlistLayout = new QVBoxLayout(playlistContents);
    playlistLayout->setContentsMargins(4, 4, 4, 4);
    playlistLayout->setSpacing(2);

    // 创建播放列表视图
    m_playlistView = new QListView(playlistContents);
    m_playlistView->setModel(m_playlistModel);
    m_playlistView->setAlternatingRowColors(true);
    m_playlistView->setSelectionMode(QListView::SingleSelection);
    m_playlistView->setSelectionBehavior(QListView::SelectRows);
    m_playlistView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_playlistView->setStyleSheet(
        "QListView {"
        "    background-color: #2a2a2a;"
        "    color: #e0e0e0;"
        "    border: none;"
        "    outline: none;"
        "}"
        ""
        "QListView::item {"
        "    height: 26px;"
        "    padding-left: 6px;"
        "    border-bottom: 1px solid #333333;"
        "}"
        ""
        "QListView::item:selected {"
        "    background-color: #4a90d9;"
        "    color: #ffffff;"
        "}"
        ""
        "QListView::item:hover:!selected {"
        "    background-color: #3a3a3a;"
        "}"
        );

    // 创建按钮布局
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(4);

    // 创建按钮
    m_addToPlaylistButton = new QPushButton(tr("添加文件"), playlistContents);
    m_removeFromPlaylistButton = new QPushButton(tr("移除"), playlistContents);
    m_clearPlaylistButton = new QPushButton(tr("清空"), playlistContents);

    m_addToPlaylistButton->setMinimumWidth(70);
    m_removeFromPlaylistButton->setMinimumWidth(50);
    m_clearPlaylistButton->setMinimumWidth(50);

    m_removeFromPlaylistButton->setEnabled(false);
    m_clearPlaylistButton->setEnabled(false);

    buttonLayout->addWidget(m_addToPlaylistButton);
    buttonLayout->addWidget(m_removeFromPlaylistButton);
    buttonLayout->addWidget(m_clearPlaylistButton);
    buttonLayout->addStretch();

    // 添加到布局
    playlistLayout->addWidget(m_playlistView);
    playlistLayout->addLayout(buttonLayout);

    // 设置停靠窗口的内容
    m_playlistDock->setWidget(playlistContents);

    // 添加到主窗口 - 使用显式类型转换确保正确解析
    QMainWindow::addDockWidget(static_cast<Qt::DockWidgetArea>(Qt::LeftDockWidgetArea), m_playlistDock);
}

void MainWindow::setupHistory()
{
    // 创建历史记录模型
    m_historyModel = new HistoryModel(this);

    // 创建历史记录保存定时器（每5秒保存一次）
    m_historyTimer = new QTimer(this);
    m_historyTimer->setInterval(5000);  // 5秒
    connect(m_historyTimer, &QTimer::timeout,
            this, &MainWindow::recordPlaybackProgress);
}

void MainWindow::setupHistoryUI()
{
    // 创建历史记录停靠窗口
    m_historyDock = new QDockWidget(tr("播放历史"), this);
    m_historyDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    m_historyDock->setMinimumWidth(200);
    m_historyDock->setMaximumWidth(350);
    m_historyDock->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable);

    // 创建停靠窗口的内容部件
    QWidget *historyContents = new QWidget(m_historyDock);
    QVBoxLayout *historyLayout = new QVBoxLayout(historyContents);
    historyLayout->setContentsMargins(4, 4, 4, 4);
    historyLayout->setSpacing(2);

    // 创建历史记录视图
    m_historyView = new QListView(historyContents);
    m_historyView->setModel(m_historyModel);
    m_historyView->setAlternatingRowColors(true);
    m_historyView->setSelectionMode(QListView::SingleSelection);
    m_historyView->setSelectionBehavior(QListView::SelectRows);
    m_historyView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_historyView->setStyleSheet(
        "QListView {"
        "    background-color: #2a2a2a;"
        "    color: #e0e0e0;"
        "    border: none;"
        "    outline: none;"
        "}"
        ""
        "QListView::item {"
        "    height: 50px;"
        "    padding-left: 6px;"
        "    padding-right: 6px;"
        "    border-bottom: 1px solid #333333;"
        "}"
        ""
        "QListView::item:selected {"
        "    background-color: #4a90d9;"
        "    color: #ffffff;"
        "}"
        ""
        "QListView::item:hover:!selected {"
        "    background-color: #3a3a3a;"
        "}"
        );

    // 创建按钮布局
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(4);

    // 创建按钮
    m_removeFromHistoryButton = new QPushButton(tr("移除"), historyContents);
    m_clearHistoryButton = new QPushButton(tr("清空"), historyContents);

    m_removeFromHistoryButton->setMinimumWidth(50);
    m_clearHistoryButton->setMinimumWidth(50);

    m_removeFromHistoryButton->setEnabled(false);
    m_clearHistoryButton->setEnabled(false);

    buttonLayout->addWidget(m_removeFromHistoryButton);
    buttonLayout->addWidget(m_clearHistoryButton);
    buttonLayout->addStretch();

    // 添加到布局
    historyLayout->addWidget(m_historyView);
    historyLayout->addLayout(buttonLayout);

    // 设置停靠窗口的内容
    m_historyDock->setWidget(historyContents);

    // 添加到主窗口（放在播放列表停靠窗口的右边）
    QMainWindow::addDockWidget(Qt::RightDockWidgetArea, m_historyDock);

    // 连接历史记录视图的信号
    connect(m_removeFromHistoryButton, &QPushButton::clicked,
            this, &MainWindow::removeFromHistory);
    connect(m_clearHistoryButton, &QPushButton::clicked,
            this, &MainWindow::clearHistory);
    connect(m_historyView, &QListView::activated,
            this, &MainWindow::onHistoryActivated);
    connect(m_historyView, &QListView::doubleClicked,
            this, &MainWindow::onHistoryDoubleClicked);
    connect(m_historyModel, &HistoryModel::dataChanged,
            this, &MainWindow::updateHistoryStatus);
    connect(m_historyModel, &HistoryModel::rowsInserted,
            this, &MainWindow::updateHistoryStatus);
    connect(m_historyModel, &HistoryModel::rowsRemoved,
            this, &MainWindow::updateHistoryStatus);
}

void MainWindow::loadHistoryFromFile()
{
    QString historyPath = QDir::homePath() + "/.videoplayer_history.json";
    QFile file(historyPath);

    if (file.exists() && file.open(QIODevice::ReadOnly)) {
        QByteArray jsonData = file.readAll();
        file.close();

        QJsonDocument doc = QJsonDocument::fromJson(jsonData);
        if (doc.isArray()) {
            QJsonArray array = doc.array();

            // 先收集所有有效的历史记录
            QList<QPair<QString, qint64>> items;
            for (const QJsonValue &value : array) {
                if (value.isObject()) {
                    QJsonObject obj = value.toObject();
                    QString filePath = obj["filePath"].toString();
                    qint64 lastPosition = static_cast<qint64>(obj["lastPosition"].toInt());

                    // 检查文件是否存在
                    if (QFile::exists(filePath)) {
                        items.append(qMakePair(filePath, lastPosition));
                    }
                }
            }

            // 倒序遍历，将最新的放到顶部
            // 跳过重复项（因为每项只添加一次到顶部）
            QSet<QString> addedPaths;
            for (int i = items.size() - 1; i >= 0; --i) {
                const QString &filePath = items[i].first;
                qint64 lastPosition = items[i].second;

                if (!addedPaths.contains(filePath)) {
                    m_historyModel->addItem(filePath, lastPosition);
                    addedPaths.insert(filePath);
                }
            }
        }
    }
}

void MainWindow::saveHistoryToFile()
{
    QString historyPath = QDir::homePath() + "/.videoplayer_history.json";
    QFile file(historyPath);

    if (file.open(QIODevice::WriteOnly)) {
        QJsonArray array;
        for (int i = 0; i < m_historyModel->count(); ++i) {
            QJsonObject obj;
            obj["filePath"] = m_historyModel->getFilePath(i);
            obj["lastPosition"] = static_cast<qint64>(m_historyModel->getLastPosition(i));
            array.append(obj);
        }

        QJsonDocument doc(array);
        file.write(doc.toJson(QJsonDocument::Indented));
        file.close();
    }
}

void MainWindow::recordCurrentPosition()
{
    if (!m_currentFilePath.isEmpty() && m_mediaPlayer) {
        qint64 currentPos = m_mediaPlayer->position();
        m_historyModel->addItem(m_currentFilePath, currentPos);
        m_lastSavedPosition = currentPos;
        // 立即保存到文件
        saveHistoryToFile();
    }
}

void MainWindow::recordPlaybackProgress()
{
    // 只有在播放时才记录
    if (m_mediaPlayer->playbackState() != QMediaPlayer::PlayingState) {
        return;
    }

    // 只有位置变化超过1秒才保存，减少频繁IO
    qint64 currentPos = m_mediaPlayer->position();
    if (qAbs(currentPos - m_lastSavedPosition) >= 1000) {
        if (!m_currentFilePath.isEmpty()) {
            m_historyModel->addItem(m_currentFilePath, currentPos);
            m_lastSavedPosition = currentPos;
            // 定期保存到文件（避免过于频繁）
            static int saveCounter = 0;
            saveCounter++;
            if (saveCounter >= 6) {  // 每6次（约30秒）保存一次
                saveHistoryToFile();
                saveCounter = 0;
            }
        }
    }
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

    // 连接媒体状态变化信号，用于恢复播放进度
    connect(m_mediaPlayer, &QMediaPlayer::mediaStatusChanged,
            this, [this](QMediaPlayer::MediaStatus status) {
                if (status == QMediaPlayer::LoadedMedia) {
                    if (m_pendingPosition >= 0) {
                        m_mediaPlayer->setPosition(m_pendingPosition);
                        m_lastSavedPosition = m_pendingPosition;
                        m_pendingPosition = -1;
                    }
                    // 如果需要自动播放（从历史记录打开）
                    if (m_autoPlayAfterSeek) {
                        m_mediaPlayer->play();
                        m_playButton->setText(tr("暂停"));
                        statusBar()->showMessage(tr("正在播放"));
                        m_autoPlayAfterSeek = false;
                    }
                }
            });

    // 连接播放列表信号
    connect(m_addToPlaylistButton, &QPushButton::clicked,
            this, &MainWindow::addFilesToPlaylist);
    connect(m_removeFromPlaylistButton, &QPushButton::clicked,
            this, &MainWindow::removeFromPlaylist);
    connect(m_clearPlaylistButton, &QPushButton::clicked,
            this, &MainWindow::clearPlaylist);

    // 连接列表视图的双击和单击激活信号
    connect(m_playlistView, &QListView::activated,
            this, &MainWindow::onPlaylistActivated);
    connect(m_playlistView, &QListView::doubleClicked,
            this, &MainWindow::onPlaylistDoubleClicked);

    // 连接模型的数据改变信号，更新按钮状态
    connect(m_playlistModel, &PlaylistModel::dataChanged,
            this, &MainWindow::updatePlaylistStatus);
    connect(m_playlistModel, &PlaylistModel::rowsInserted,
            this, &MainWindow::updatePlaylistStatus);
    connect(m_playlistModel, &PlaylistModel::rowsRemoved,
            this, &MainWindow::updatePlaylistStatus);
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
        // 调用playFile来加载新文件（会自动保存上一个文件进度并恢复播放进度）
        playFile(filePath);
    }
}

void MainWindow::play()
{
    switch (m_mediaPlayer->playbackState()) {
    case QMediaPlayer::PlayingState:
        m_mediaPlayer->pause();
        m_playButton->setText(tr("播放"));
        m_historyTimer->stop();  // 暂停时停止定时器
        statusBar()->showMessage(tr("已暂停"));
        break;
    case QMediaPlayer::PausedState:
    case QMediaPlayer::StoppedState:
        m_mediaPlayer->play();
        m_playButton->setText(tr("暂停"));
        m_historyTimer->start();  // 开始播放时启动定时器
        statusBar()->showMessage(tr("正在播放"));
        break;
    }
}

void MainWindow::stop()
{
    // 停止前先保存当前播放进度
    recordCurrentPosition();

    m_mediaPlayer->stop();
    m_playButton->setText(tr("播放"));
    m_positionSlider->setValue(0);
    m_positionLabel->setText("00:00");
    m_historyTimer->stop();  // 停止时停止定时器
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

void MainWindow::addFilesToPlaylist()
{
    QStringList filePaths = QFileDialog::getOpenFileNames(
        this,
        tr("添加视频文件到播放列表"),
        QDir::homePath(),
        tr("视频文件 (*.mp4 *.avi *.mkv *.mov *.wmv *.flv *.webm);;所有文件 (*.*)")
        );

    if (!filePaths.isEmpty()) {
        for (const QString &filePath : filePaths) {
            if (m_playlistModel->getIndex(filePath) == -1) {
                m_playlistModel->addItem(filePath);
            }
        }
        savePlaylistToFile();
        statusBar()->showMessage(tr("已添加 %1 个文件到播放列表").arg(filePaths.count()));
    }
}

void MainWindow::removeFromPlaylist()
{
    QModelIndex currentIndex = m_playlistView->currentIndex();
    if (currentIndex.isValid()) {
        QString fileName = m_playlistModel->data(currentIndex, Qt::DisplayRole).toString();
        m_playlistModel->removeItem(currentIndex.row());
        savePlaylistToFile();
        statusBar()->showMessage(tr("已移除: %1").arg(fileName));
    }
}

void MainWindow::clearPlaylist()
{
    if (m_playlistModel->count() > 0) {
        m_playlistModel->clear();
        savePlaylistToFile();
        statusBar()->showMessage(tr("播放列表已清空"));
    }
}

void MainWindow::onPlaylistActivated(const QModelIndex &index)
{
    if (index.isValid()) {
        QString filePath = m_playlistModel->getFilePath(index.row());
        if (!filePath.isEmpty()) {
            playFile(filePath);
            m_playlistModel->setCurrentIndex(index.row());
        }
    }
}

void MainWindow::onPlaylistDoubleClicked(const QModelIndex &index)
{
    onPlaylistActivated(index);
}

void MainWindow::updatePlaylistStatus()
{
    // 更新移除按钮的状态
    bool hasSelection = m_playlistView->currentIndex().isValid();
    m_removeFromPlaylistButton->setEnabled(hasSelection);

    // 更新清空按钮的状态
    m_clearPlaylistButton->setEnabled(m_playlistModel->count() > 0);

    // 如果有正在播放的文件，高亮显示
    if (!m_currentFilePath.isEmpty()) {
        int currentRow = m_playlistModel->getIndex(m_currentFilePath);
        if (currentRow >= 0) {
            QModelIndex modelIndex = m_playlistModel->index(currentRow);
            m_playlistView->setCurrentIndex(modelIndex);
        }
    }
}

void MainWindow::removeFromHistory()
{
    QModelIndex currentIndex = m_historyView->currentIndex();
    if (currentIndex.isValid()) {
        QString filePath = m_historyModel->getFilePath(currentIndex.row());
        m_historyModel->removeItem(filePath);
        saveHistoryToFile();
        statusBar()->showMessage(tr("已从历史记录移除: %1").arg(QFileInfo(filePath).fileName()));
    }
}

void MainWindow::clearHistory()
{
    if (m_historyModel->count() > 0) {
        m_historyModel->clear();
        saveHistoryToFile();
        statusBar()->showMessage(tr("历史记录已清空"));
    }
}

void MainWindow::onHistoryActivated(const QModelIndex &index)
{
    if (index.isValid()) {
        QString filePath = m_historyModel->getFilePath(index.row());
        if (!filePath.isEmpty()) {
            // 从历史记录打开，自动播放并恢复进度
            playFile(filePath, true);
        }
    }
}

void MainWindow::onHistoryDoubleClicked(const QModelIndex &index)
{
    onHistoryActivated(index);
}

void MainWindow::updateHistoryStatus()
{
    // 更新移除按钮的状态
    bool hasSelection = m_historyView->currentIndex().isValid();
    m_removeFromHistoryButton->setEnabled(hasSelection);

    // 更新清空按钮的状态
    m_clearHistoryButton->setEnabled(m_historyModel->count() > 0);

    // 如果有正在播放的文件，高亮显示
    if (!m_currentFilePath.isEmpty()) {
        int currentRow = m_historyModel->getIndex(m_currentFilePath);
        if (currentRow >= 0) {
            QModelIndex modelIndex = m_historyModel->index(currentRow);
            m_historyView->setCurrentIndex(modelIndex);
        }
    }
}

void MainWindow::loadPlaylistFromFile()
{
    QString playlistPath = QDir::homePath() + "/.videoplayer_playlist.json";
    QFile file(playlistPath);

    if (file.exists() && file.open(QIODevice::ReadOnly)) {
        QByteArray jsonData = file.readAll();
        file.close();

        QJsonDocument doc = QJsonDocument::fromJson(jsonData);
        if (doc.isArray()) {
            QJsonArray array = doc.array();
            for (const QJsonValue &value : array) {
                if (value.isString()) {
                    QString filePath = value.toString();
                    // 检查文件是否存在
                    if (QFile::exists(filePath)) {
                        m_playlistModel->addItem(filePath);
                    }
                }
            }
        }
    }
}

void MainWindow::savePlaylistToFile()
{
    QString playlistPath = QDir::homePath() + "/.videoplayer_playlist.json";
    QFile file(playlistPath);

    if (file.open(QIODevice::WriteOnly)) {
        QJsonArray array;
        for (int i = 0; i < m_playlistModel->count(); ++i) {
            array.append(m_playlistModel->getFilePath(i));
        }

        QJsonDocument doc(array);
        file.write(doc.toJson(QJsonDocument::Indented));
        file.close();
    }
}

void MainWindow::playFile(const QString &filePath, bool autoPlay)
{
    // 保存上一个文件的播放进度
    if (!m_currentFilePath.isEmpty() && m_mediaPlayer) {
        qint64 lastPos = m_mediaPlayer->position();
        m_historyModel->addItem(m_currentFilePath, lastPos);
        m_lastSavedPosition = lastPos;
        saveHistoryToFile();  // 立即保存
    }

    m_currentFilePath = filePath;
    m_mediaPlayer->setSource(QUrl::fromLocalFile(filePath));

    // 检查是否有历史记录
    qint64 savedPosition = m_historyModel->getLastPosition(filePath);

    if (autoPlay) {
        // 从历史记录打开，需要自动播放并恢复位置
        m_pendingPosition = savedPosition > 0 ? savedPosition : 0;
        m_lastSavedPosition = m_pendingPosition;
        m_autoPlayAfterSeek = true;
    } else {
        // 普通打开，不自动播放
        m_pendingPosition = -1;
        m_lastSavedPosition = 0;
        m_autoPlayAfterSeek = false;
        // 设置初始按钮状态为"播放"
        m_playButton->setText(tr("播放"));
    }

    m_playButton->setEnabled(true);
    m_stopButton->setEnabled(true);
    m_forwardButton->setEnabled(true);
    m_backwardButton->setEnabled(true);
    m_positionSlider->setEnabled(true);

    // 更新历史记录UI状态（高亮当前播放的文件）
    updateHistoryStatus();

    QFileInfo fileInfo(filePath);
    statusBar()->showMessage(tr("已加载: %1").arg(fileInfo.fileName()));
}


