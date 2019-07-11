#ifndef MYSERVER_H
#define MYSERVER_H

#include "SslServer.h"

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

    QList<QString>* namesBuffer = new QList<QString>{"Lion", "Tiger", "Turtle", "Monkey",
                                                       "Kitten", "Doggy", "Starfish", "Shark",
                                                        "Jaguar", "Giraffe", "Elephant", "Deer",
                                                        "Crocodile", "Panda", "Bear", "Wolf"};

    SslServer*                      server;
    QList<QAbstractSocket*>*         clientsList;
    QMap<QAbstractSocket*, QString>* clientsNamesMap;

    QString externalAddress = "";
    QVBoxLayout* mainLayout;
    QLineEdit*   linePort;
    QTextEdit*   textBox;
    QPushButton* buttonStart;
    QPushButton* buttonStop;
    QLineEdit*   lineUsers;

private:
    void clearConsole();
    void sendToClient(QAbstractSocket*,  const QString&);
    void broadcastFrom(QAbstractSocket*, const QString&);
public:
    MyServer();
    void show();

public slots:

    void gotExternalAddress(QString address);
    void slotStart();
    void slotStop();
    virtual void slotNewConnection();
    virtual void slotDisconnected();
    void slotReadClient();
};
#endif // MYSERVER_H
