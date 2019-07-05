#ifndef MYSERVER_H
#define MYSERVER_H

#include <QWidget>
class QTcpServer;
class QTextEdit;
class QPushButton;
class QAbstractSocket;
class QLineEdit;

class MyServer: public QWidget {
    Q_OBJECT
private:
    QLineEdit* usersAmountField;
    QTcpServer* server;
    QLineEdit* portLine;
    QTextEdit* textBox;
    QPushButton* startButton;
    QPushButton* stopButton;
    QList<QAbstractSocket*>* clientsList;
    QMap<QAbstractSocket*, qintptr>* clientsDescMap;

private:
    void sendToClient(QAbstractSocket*, const QString&);
public:
    MyServer();

public slots:
    void slotStart();
    void slotStop();
    virtual void slotNewConnection();
    virtual void slotDisconnected();
    void slotReadClient();
};
#endif // MYSERVER_H
