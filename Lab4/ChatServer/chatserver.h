#ifndef CHATSERVER_H
#define CHATSERVER_H

#include <QObject>
#include <QTcpServer>

class ChatServer : public QTcpServer
{
    Q_OBJECT
public:
    ChatServer();
};

#endif // CHATSERVER_H
