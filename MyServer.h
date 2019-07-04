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
    QTcpServer* server;
    QLineEdit* portLine;
    QTextEdit* textBox;
    QPushButton* startButton;
    QPushButton* stopButton;
    QList<qintptr>* clientsList;

private:
    void sendToClient(QAbstractSocket*, const QString&);
public:
    MyServer();

public slots:
    void slotStart();
    void slotStop();
    virtual void slotNewConnection();
    void slotReadClient();
};
#endif // MYSERVER_H
