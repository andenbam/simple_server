#ifndef MYSERVER_H
#define MYSERVER_H

#include <QWidget>
class QTcpServer;
class QTextEdit;
class QTcpSocket;

class MyServer: public QWidget
{
    Q_OBJECT
private:
    QTcpServer* m_ptcpServer;

public:
    MyServer();
    ~MyServer();
    QTcpSocket* socket;
    QByteArray data;

public slots:
    void startServer();
    void incomingConnetction(int socketDescriptor);
    void sockReady();
    void sockDisc();
};

#endif // MYSERVER_H
