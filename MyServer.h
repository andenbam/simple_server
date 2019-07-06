#ifndef MYSERVER_H
#define MYSERVER_H

#include <QWidget>
class QTcpServer;
class QTextEdit;
class QPushButton;
class QAbstractSocket;
class QLineEdit;
class QVBoxLayout;

class MyServer: public QWidget {
    Q_OBJECT
private:
    QTcpServer* server;
    QList<QAbstractSocket*>* clientsList;
    QMap<QAbstractSocket*, qintptr>* clientsDescMap;

    QVBoxLayout* mainLayout;
    QLineEdit* linePort;
    QTextEdit* textBox;
    QPushButton* buttonStart;
    QPushButton* buttonStop;
    QLineEdit* lineUsers;

private:
    void sendToClient(QAbstractSocket*, const QString&);
    void broadcastFrom(QAbstractSocket*, const QString&);
public:
    MyServer();
    void show();

public slots:
    void slotStart();
    void slotStop();
    virtual void slotNewConnection();
    virtual void slotDisconnected();
    void slotReadClient();
};
#endif // MYSERVER_H
