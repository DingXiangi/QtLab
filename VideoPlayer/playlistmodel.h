#ifndef PLAYLISTMODEL_H
#define PLAYLISTMODEL_H

#include <QAbstractListModel>

class PlaylistModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit PlaylistModel(QObject *parent = nullptr);  // 带参数的构造函数
    // 或者 PlaylistModel();  // 不带参数的构造函数
    // 必须与.cpp文件匹配！
};

#endif // PLAYLISTMODEL_H
