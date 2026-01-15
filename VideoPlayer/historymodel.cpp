#include "historymodel.h"
#include <QFileInfo>
#include <QDateTime>

HistoryModel::HistoryModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int HistoryModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_items.count();
}

QVariant HistoryModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= m_items.count()) {
        return QVariant();
    }

    const HistoryItem &item = m_items.at(index.row());

    switch (role) {
    case Qt::DisplayRole:
    case TitleRole:
        return item.title();
    case FilePathRole:
        return item.filePath();
    case LastPositionRole:
        return item.lastPosition();
    case TimestampRole:
        return item.timestamp();
    case FormattedTimestampRole: {
        QDateTime ts = item.timestamp();
        return ts.toString("yyyy-MM-dd hh:mm");
    }
    case FormattedPositionRole:
        return formatPosition(item.lastPosition());
    default:
        return QVariant();
    }
}

void HistoryModel::addItem(const QString &filePath, qint64 lastPosition)
{
    if (filePath.isEmpty()) {
        return;
    }

    // 检查是否已存在
    int existingIndex = getIndex(filePath);
    if (existingIndex >= 0) {
        // 已存在则更新时间戳和进度
        m_items[existingIndex].setLastPosition(lastPosition);
        m_items[existingIndex].setTimestamp(QDateTime::currentDateTime());
        emit dataChanged(createIndex(existingIndex, 0), createIndex(existingIndex, 0));
        return;
    }

    beginInsertRows(QModelIndex(), m_items.count(), m_items.count());
    m_items.append(HistoryItem(filePath, lastPosition));
    endInsertRows();
}

void HistoryModel::updateItem(const QString &filePath, qint64 lastPosition)
{
    addItem(filePath, lastPosition);
}

void HistoryModel::removeItem(const QString &filePath)
{
    int removeIndex = getIndex(filePath);
    if (removeIndex < 0) {
        return;
    }

    beginRemoveRows(QModelIndex(), removeIndex, removeIndex);
    m_items.removeAt(removeIndex);
    endRemoveRows();
}

void HistoryModel::clear()
{
    if (m_items.isEmpty()) {
        return;
    }

    beginResetModel();
    m_items.clear();
    endResetModel();
}

QString HistoryModel::getFilePath(int row) const
{
    if (row < 0 || row >= m_items.count()) {
        return QString();
    }
    return m_items.at(row).filePath();
}

qint64 HistoryModel::getLastPosition(int row) const
{
    if (row < 0 || row >= m_items.count()) {
        return 0;
    }
    return m_items.at(row).lastPosition();
}

qint64 HistoryModel::getLastPosition(const QString &filePath) const
{
    int idx = getIndex(filePath);
    if (idx >= 0) {
        return m_items.at(idx).lastPosition();
    }
    return 0;
}

int HistoryModel::getIndex(const QString &filePath) const
{
    for (int i = 0; i < m_items.count(); ++i) {
        if (m_items.at(i).filePath() == filePath) {
            return i;
        }
    }
    return -1;
}

bool HistoryModel::contains(const QString &filePath) const
{
    return getIndex(filePath) >= 0;
}

QString HistoryModel::formatPosition(qint64 position) const
{
    qint64 seconds = position / 1000;
    qint64 minutes = seconds / 60;
    qint64 hours = minutes / 60;
    seconds = seconds % 60;
    minutes = minutes % 60;

    if (hours > 0) {
        return QString("%1:%2:%3")
        .arg(hours, 2, 10, QChar('0'))
            .arg(minutes, 2, 10, QChar('0'))
            .arg(seconds, 2, 10, QChar('0'));
    } else {
        return QString("%1:%2")
        .arg(minutes, 2, 10, QChar('0'))
            .arg(seconds, 2, 10, QChar('0'));
    }
}

QHash<int, QByteArray> HistoryModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[FilePathRole] = "filePath";
    roles[TitleRole] = "title";
    roles[LastPositionRole] = "lastPosition";
    roles[TimestampRole] = "timestamp";
    roles[FormattedTimestampRole] = "formattedTimestamp";
    roles[FormattedPositionRole] = "formattedPosition";
    return roles;
}
