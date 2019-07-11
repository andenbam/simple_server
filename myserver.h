#ifndef MYSERVER_H
#define MYSERVER_H

#include "SslServer.h"

#include <QWidget>
class QTextEdit;
class QPushButton;
class QSslSocket;
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
    QList<QSslSocket*>*         clientsList;
    QMap<qintptr, QString>* clientsNamesMap;

    QString externalAddress = "";
    QVBoxLayout* mainLayout;
    QLineEdit*   linePort;
    QTextEdit*   textBox;
    QPushButton* buttonStart;
    QPushButton* buttonStop;
    QLineEdit*   lineUsers;

private:
    void clearConsole();
    void sendToClient(QSslSocket*,  const QString&);
    void broadcastFrom(QSslSocket*, const QString&);
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
