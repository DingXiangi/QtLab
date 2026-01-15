#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QVideoWidget>
#include <QSlider>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QStandardPaths>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnOpen_clicked();
    void on_btnPlayPause_clicked();
    void onPlayerStateChanged(QMediaPlayer::PlaybackState state);
    void on_btnStop_clicked();
    void on_btnPrevious_clicked();
    void on_btnNext_clicked();
    void on_volumeChanged(qreal volume);
    void on_muteClicked();
    void updateProgress(qint64 position);
    void on_sliderProgress_valueChanged(qint64 value);
    void on_sliderVolume_valueChanged(int value);

private:
    Ui::MainWindow *ui;

    // 核心媒体组件
    QMediaPlayer *m_player = nullptr;
    QAudioOutput *m_audioOutput = nullptr;
    QVideoWidget *m_videoWidget = nullptr;

    // 控制组件
    QPushButton *btnPlayPause = nullptr;
    QPushButton *btnStop = nullptr;
    QPushButton *btnPrevious = nullptr;
    QPushButton *btnNext = nullptr;
    QPushButton *btnMute = nullptr;
    QSlider *sliderProgress = nullptr;
    QSlider *sliderVolume = nullptr;
    QLabel *labelTime = nullptr;
    QLabel *labelVolume = nullptr;

    // 布局容器
    QWidget *centralWidget = nullptr;
    QVBoxLayout *mainLayout = nullptr;
    QHBoxLayout *controlLayout = nullptr;
    QHBoxLayout *volumeLayout = nullptr;

    // 状态标志
    bool isDraggingSlider = false;
    bool isMuted = false;

    void initUI();
    void initPlayer();
    void setupConnections();
    void setupControlBar();
    QString formatTime(qint64 seconds) const;
};
#endif // MAINWINDOW_H
