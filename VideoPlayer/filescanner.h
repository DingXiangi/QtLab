#ifndef FILESCANNER_H
#define FILESCANNER_H

#include <QObject>

class FileScanner : public QObject
{
    Q_OBJECT
public:
    explicit FileScanner(QObject *parent = nullptr);

signals:
};

#endif // FILESCANNER_H
