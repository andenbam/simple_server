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
    QTcpServer* tcpServer;
    QLineEdit* portLine;
    QTextEdit* textBox;
    QPushButton* startButton;
    QPushButton* stopButton;

private:
    void sendToClient(QAbstractSocket* pSocket, const QString& str);
public:
    MyServer();

public slots:
    virtual void slotNewConnection();
    void slotStart();
    void slotStop();
    void slotReadClient();
};
#endif // MYSERVER_H
