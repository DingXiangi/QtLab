#include "serverworker.h"
#include<QDataStream>
#include<QJsonDocument>
#include <QJsonObject>

ServerWorker::ServerWorker(QObject *parent){

    m_serverSocket=new QTcpSocket(this);
    connect(m_serverSocket, &QTcpSocket::readyRead, this, &ServerWorker::onReadyRead);
}

bool ServerWorker::setSocketDescriptor(qintptr socketDescriptor)
{

    return m_serverSocket->setSocketDescriptor(socketDescriptor);
}

void ServerWorker::onReadyRead()
{
    QByteArray jsonData;
    QDataStream socketStream(m_serverSocket);
    socketStream.setVersion(QDataStream::Qt_6_9);
    // start an infinite loop
    for (;;) {
        socketStream.startTransaction();
        socketStream >> jsonData;
        if (socketStream.commitTransaction()) {
            emit logMessage(QString::fromUtf8(jsonData));
            sendMessage("I recieved message");
        } else {
            break;
        }
    }
}
void ServerWorker::sendMessage(const QString &text, const QString &type)
{
    if (m_serverSocket->state() != QAbstractSocket::ConnectedState)
        return;

    if (!text.isEmpty()) {
        // create a QDataStream operating on the socket
        QDataStream serverStream(m_serverSocket);
        serverStream.setVersion(QDataStream::Qt_6_9);

        // Create the JSON we want to send
        QJsonObject message;
        message["type"] = type;
        message["text"] = text;

        // send the JSON using QDataStream
        serverStream << QJsonDocument(message).toJson();
    }
}
