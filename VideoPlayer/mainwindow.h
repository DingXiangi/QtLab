#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QPushButton>
#include <QSlider>
#include <QLabel>

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

private:
    Ui::MainWindow *ui;

    // 视频播放相关控件
    QMediaPlayer *m_mediaPlayer;
    QVideoWidget *m_videoWidget;

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

    // 标记是否正在拖动进度条
    bool m_seekBarDown;

    void setupUI();
    void setupConnections();
    void applyStyles();
    void updatePositionLabel(qint64 position);
    void updateDurationLabel(qint64 duration);
    void doSeek(int value);
};
#endif // MAINWINDOW_H
