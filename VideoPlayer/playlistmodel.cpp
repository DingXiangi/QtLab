#include "playlistmodel.h"
#include <QFileInfo>

PlaylistModel::PlaylistModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int PlaylistModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_items.count();
}

QVariant PlaylistModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= m_items.count()) {
        return QVariant();
    }

    const PlaylistItem &item = m_items.at(index.row());

    switch (role) {
    case Qt::DisplayRole:
    case TitleRole:
        return item.title();
    case FilePathRole:
        return item.filePath();
    default:
        return QVariant();
    }
}

void PlaylistModel::addItem(const QString &filePath)
{
    if (filePath.isEmpty()) {
        return;
    }

    beginInsertRows(QModelIndex(), m_items.count(), m_items.count());
    m_items.append(PlaylistItem(filePath));
    endInsertRows();
}

void PlaylistModel::removeItem(int row)
{
    if (row < 0 || row >= m_items.count()) {
        return;
    }

    beginRemoveRows(QModelIndex(), row, row);
    m_items.removeAt(row);
    endRemoveRows();

    // 更新当前索引
    if (m_currentIndex >= m_items.count()) {
        m_currentIndex = m_items.count() - 1;
    }
}

void PlaylistModel::clear()
{
    if (m_items.isEmpty()) {
        return;
    }

    beginResetModel();
    m_items.clear();
    m_currentIndex = -1;
    endResetModel();
}

QString PlaylistModel::getFilePath(int row) const
{
    if (row < 0 || row >= m_items.count()) {
        return QString();
    }
    return m_items.at(row).filePath();
}

int PlaylistModel::getIndex(const QString &filePath) const
{
    for (int i = 0; i < m_items.count(); ++i) {
        if (m_items.at(i).filePath() == filePath) {
            return i;
        }
    }
    return -1;
}

void PlaylistModel::setCurrentIndex(int index)
{
    if (index < 0 || index >= m_items.count()) {
        return;
    }

    int oldIndex = m_currentIndex;
    m_currentIndex = index;

    // 发出数据改变信号以更新视图
    if (oldIndex >= 0 && oldIndex < m_items.count()) {
        emit dataChanged(createIndex(oldIndex, 0), createIndex(oldIndex, 0));
    }
    if (index >= 0 && index < m_items.count()) {
        emit dataChanged(createIndex(index, 0), createIndex(index, 0));
    }
}

QHash<int, QByteArray> PlaylistModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[FilePathRole] = "filePath";
    roles[TitleRole] = "title";
    return roles;
}
