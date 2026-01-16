#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QAudioOutput>
#include <QPushButton>
#include <QSlider>
#include <QLabel>
#include <QListView>
#include <QDockWidget>
#include <QList>
#include <QSet>
#include <QTimer>

#include "playlistmodel.h"
#include "historymodel.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void openFile();
    void play();
    void stop();
    void seekForward();
    void seekBackward();
    void onPositionSliderValueChanged(int value);
    void onSliderPressed();
    void onSliderReleased();
    void positionChanged(qint64 position);
    void durationChanged(qint64 duration);

    // 播放列表相关槽函数
    void addFilesToPlaylist();
    void removeFromPlaylist();
    void clearPlaylist();
    void onPlaylistActivated(const QModelIndex &index);
    void onPlaylistDoubleClicked(const QModelIndex &index);
    void updatePlaylistStatus();

    // 历史记录相关槽函数
    void recordPlaybackProgress();
    void removeFromHistory();
    void clearHistory();
    void onHistoryActivated(const QModelIndex &index);
    void onHistoryDoubleClicked(const QModelIndex &index);
    void updateHistoryStatus();

private:
    Ui::MainWindow *ui;

    // 视频播放相关控件
    QMediaPlayer *m_mediaPlayer;
    QVideoWidget *m_videoWidget;
    QAudioOutput *m_audioOutput;

    // 播放控制按钮
    QPushButton *m_playButton;
    QPushButton *m_openButton;
    QPushButton *m_stopButton;
    QPushButton *m_forwardButton;
    QPushButton *m_backwardButton;

    // 进度条
    QSlider *m_positionSlider;
    QLabel *m_positionLabel;
    QLabel *m_durationLabel;

    // 播放列表相关控件
    QDockWidget *m_playlistDock;
    QListView *m_playlistView;
    QPushButton *m_addToPlaylistButton;
    QPushButton *m_removeFromPlaylistButton;
    QPushButton *m_clearPlaylistButton;

    // 播放列表数据模型
    PlaylistModel *m_playlistModel;

    // 历史记录数据模型
    HistoryModel *m_historyModel;

    // 历史记录UI控件
    QDockWidget *m_historyDock;
    QListView *m_historyView;
    QPushButton *m_removeFromHistoryButton;
    QPushButton *m_clearHistoryButton;

    // 历史记录保存定时器
    QTimer *m_historyTimer;

    // 标记是否正在拖动进度条
    bool m_seekBarDown;

    // 当前播放文件路径
    QString m_currentFilePath;

    // 最后保存的播放位置（用于避免重复保存）
    qint64 m_lastSavedPosition;

    // 待恢复的播放位置（用于媒体加载完成后恢复）
    qint64 m_pendingPosition;

    // 是否在恢复位置后自动播放（从历史记录打开时使用）
    bool m_autoPlayAfterSeek;

    void setupUI();
    void setupConnections();
    void applyStyles();
    void updatePositionLabel(qint64 position);
    void updateDurationLabel(qint64 duration);
    void doSeek(int value);

    // 播放列表辅助方法
    void setupPlaylistUI();
    void loadPlaylistFromFile();
    void savePlaylistToFile();
    void playFile(const QString &filePath, bool autoPlay = false);

    // 历史记录辅助方法
    void setupHistory();
    void setupHistoryUI();
    void loadHistoryFromFile();
    void saveHistoryToFile();
    void recordCurrentPosition();
};
#endif // MAINWINDOW_H
