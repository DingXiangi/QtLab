#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 设置应用程序信息
    a.setApplicationName("Qt视频播放器");
    a.setApplicationVersion("1.0");
    a.setOrganizationName("QtCourseDesign");

    MainWindow w;
    w.show();

    return a.exec();
}
