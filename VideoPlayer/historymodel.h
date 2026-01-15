#ifndef HISTORYMODEL_H
#define HISTORYMODEL_H

#include <QAbstractListModel>
#include <QUrl>
#include <QList>
#include <QString>
#include <QDateTime>
#include <QFileInfo>
class HistoryItem
{
public:
    HistoryItem() {}
    HistoryItem(const QString &filePath, qint64 lastPosition = 0, const QString &title = QString())
        : m_filePath(filePath)
        , m_lastPosition(lastPosition)
        , m_title(title)
        , m_timestamp(QDateTime::currentDateTime())
    {
        if (m_title.isEmpty()) {
            m_title = QFileInfo(filePath).fileName();
        }
    }

    QString filePath() const { return m_filePath; }
    QString title() const { return m_title; }
    qint64 lastPosition() const { return m_lastPosition; }
    QDateTime timestamp() const { return m_timestamp; }

    void setLastPosition(qint64 position) { m_lastPosition = position; }
    void setTimestamp(const QDateTime &timestamp) { m_timestamp = timestamp; }

private:
    QString m_filePath;
    QString m_title;
    qint64 m_lastPosition = 0;
    QDateTime m_timestamp;
};

class HistoryModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit HistoryModel(QObject *parent = nullptr);

    enum Role {
        FilePathRole = Qt::UserRole + 1,
        TitleRole,
        LastPositionRole,
        TimestampRole,
        FormattedTimestampRole,
        FormattedPositionRole
    };

    // 重写基类方法
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // 历史记录操作方法
    void addItem(const QString &filePath, qint64 lastPosition = 0);
    void updateItem(const QString &filePath, qint64 lastPosition);
    void removeItem(const QString &filePath);
    void clear();

    // 辅助方法
    QString formatPosition(qint64 position) const;

    // 获取项目信息
    QString getFilePath(int row) const;
    qint64 getLastPosition(int row) const;
    qint64 getLastPosition(const QString &filePath) const;
    int getIndex(const QString &filePath) const;
    int count() const { return m_items.count(); }

    // 检查文件是否在历史记录中
    bool contains(const QString &filePath) const;

protected:
    QHash<int, QByteArray> roleNames() const override;

private:
    QList<HistoryItem> m_items;
};

#endif // HISTORYMODEL_H
