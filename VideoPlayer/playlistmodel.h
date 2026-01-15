#ifndef PLAYLISTMODEL_H
#define PLAYLISTMODEL_H

#include <QAbstractListModel>
#include <QUrl>
#include <QList>
#include <QString>
#include <QFileInfo>

class PlaylistItem
{
public:
    PlaylistItem() {}
    PlaylistItem(const QString &filePath, const QString &title = QString())
        : m_filePath(filePath)
        , m_title(title)
    {
        if (m_title.isEmpty()) {
            m_title = QFileInfo(filePath).fileName();
        }
    }

    QString filePath() const { return m_filePath; }
    QString title() const { return m_title; }

private:
    QString m_filePath;
    QString m_title;
};

class PlaylistModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit PlaylistModel(QObject *parent = nullptr);

    enum Role {
        FilePathRole = Qt::UserRole + 1,
        TitleRole
    };

    // 重写基类方法
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // 播放列表操作方法
    void addItem(const QString &filePath);
    void removeItem(int row);
    void clear();

    // 获取项目信息
    QString getFilePath(int row) const;
    int getIndex(const QString &filePath) const;
    int currentIndex() const { return m_currentIndex; }
    void setCurrentIndex(int index);
    int count() const { return m_items.count(); }

protected:
    QHash<int, QByteArray> roleNames() const override;

private:
    QList<PlaylistItem> m_items;
    int m_currentIndex = -1;
};

#endif // PLAYLISTMODEL_H
