#include "MyServer.h"
#include <QMessageBox>
#include <QTcpServer>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QLabel>
#include <QDataStream>
#include <QTcpSocket>
#include <QTime>
#include <QLineEdit>
#include <QPushButton>

MyServer::MyServer() :
                QWidget () {

    textBox = new QTextEdit();
    portLine = new QLineEdit();
    startButton = new QPushButton("start");
    stopButton = new QPushButton("stop");
    stopButton->setDisabled(true);
    textBox->setReadOnly(true);

    connect(startButton, &QPushButton::pressed, this, &MyServer::slotStart);
    connect(stopButton, &QPushButton::pressed, this, &MyServer::slotStop);

    //layout setup

    QHBoxLayout* horizontal = new QHBoxLayout();
    horizontal->addWidget(portLine);
    horizontal->addWidget(startButton);
    horizontal->addWidget(stopButton);
    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(new QLabel("<H1>Server</H1>"));
    layout->addWidget(textBox);
    layout->addLayout(horizontal);
    setLayout(layout);
}

void MyServer::slotNewConnection()
{

    QTcpSocket* pClientSocket = tcpServer->nextPendingConnection();
    connect(pClientSocket, SIGNAL(disconnected()), pClientSocket, SLOT(deleteLater()));
    connect(pClientSocket, SIGNAL(readyRead()), this, SLOT(slotReadClient()));
    sendToClient(pClientSocket, "Server Response: Connected!");
}

void MyServer::slotStart() {

    tcpServer = new QTcpServer(this);

    if (!tcpServer->listen(QHostAddress::Any, quint16(portLine->text().toInt()))) {
        QMessageBox::critical(nullptr, "SeverError",
                              "Unnable to start the server:" +
                              tcpServer->errorString());
        tcpServer->close();
        return;
    }

    connect(tcpServer, &QTcpServer::newConnection, this, &MyServer::slotNewConnection);

    stopButton->setDisabled(false);
    startButton->setDisabled(true);
  }

void MyServer::slotStop()
{

    disconnect(tcpServer, &QTcpServer::newConnection, this, &MyServer::slotNewConnection);
    tcpServer->close();
    delete tcpServer;
    tcpServer = nullptr;

    stopButton->setDisabled(true);
    startButton->setDisabled(false);
}

void MyServer::slotReadClient() {
    QTcpSocket* pClientSocket = (QTcpSocket*)sender();
    char* data = new char[256];
    pClientSocket->read(data,256);
    QString str = QString(data);
    textBox->append(str);
    sendToClient(pClientSocket, "Server response: Received \"" + str + "\"");
}

//46.0.199.93
//5000
void MyServer::sendToClient(QTcpSocket *pSocket, const QString &str) {
    pSocket->write(QTime::currentTime().toString(Qt::LocalDate).
                   append(" ").append(str).toStdString().c_str());
}




