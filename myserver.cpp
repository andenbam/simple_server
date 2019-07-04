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

MyServer::MyServer() : QWidget () {

    textBox = new QTextEdit();
    portLine = new QLineEdit();
    portLine->setPlaceholderText("Port num.");
    portLine->setText("5005");

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

void MyServer::slotNewConnection() {

    QTcpSocket* pClientSocket = tcpServer->nextPendingConnection();

    connect(pClientSocket, SIGNAL(disconnected()), pClientSocket, SLOT(deleteLater()));
    connect(pClientSocket, SIGNAL(readyRead()), this, SLOT(slotReadClient()));

    sendToClient(pClientSocket, "server-response:connected");
}

void MyServer::slotStart() {

    textBox->append("*starting*");
    tcpServer = new QTcpServer(this);

    if (!tcpServer->listen(QHostAddress::Any, quint16(portLine->text().toInt()))) {

        QMessageBox::critical(nullptr, "SeverError",
                     "Unnable to start the server:" + tcpServer->errorString());
        tcpServer->close();
        return;
    }

    connect(tcpServer, &QTcpServer::newConnection, this, &MyServer::slotNewConnection);

    textBox->append("#server is on...");
    textBox->append(QString("host is ").append(tcpServer->serverAddress().toString()));

    portLine    -> setDisabled(true);
    startButton -> setDisabled(true);
    stopButton  -> setDisabled(false);
  }

void MyServer::slotStop() {

    disconnect(tcpServer, &QTcpServer::newConnection, this, &MyServer::slotNewConnection);

    tcpServer -> close();
    delete tcpServer;
    tcpServer = nullptr;

    textBox->clear();

    portLine    -> setDisabled(false);
    startButton -> setDisabled(false);
    stopButton  -> setDisabled(true);
}

void MyServer::slotReadClient() {

    QAbstractSocket* socket = static_cast<QAbstractSocket*>(sender());

    QString str = QString::fromUtf8(socket->read(256));

    textBox->append(str);

    sendToClient(socket, "server-response:received \"" + str + "\"");
}

//46.0.199.93
//5000
void MyServer::sendToClient(QAbstractSocket *pSocket, const QString &str) {

    pSocket->write(QTime::currentTime().toString(Qt::LocalDate).
                   append(" ").append(str).toLocal8Bit());
}




