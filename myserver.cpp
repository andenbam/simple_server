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

    textBox     = new QTextEdit();
    portLine    = new QLineEdit();
    startButton = new QPushButton("&Start");
    stopButton  = new QPushButton("&Stop");

    textBox    -> setReadOnly(true);
    portLine   -> setPlaceholderText("Port num.");
    portLine   -> setText("5005");
    stopButton -> setDisabled(true);

    connect(startButton, &QPushButton::pressed,
                   this, &MyServer::slotStart);
    connect(stopButton, &QPushButton::pressed,
                  this, &MyServer::slotStop);


    QVBoxLayout* layout = new QVBoxLayout();
    QHBoxLayout* lPanel = new QHBoxLayout();

    lPanel -> addWidget(portLine);
    lPanel -> addWidget(startButton);
    lPanel -> addWidget(stopButton);

    layout -> addWidget(new QLabel("<H1>Local Server</H1>"));
    layout -> addWidget(textBox);

    layout -> addLayout(lPanel);

    setLayout(layout);
}

void MyServer::slotNewConnection() {

    QTcpSocket* pClientSocket = server->nextPendingConnection();

    connect(pClientSocket, &QAbstractSocket::disconnected,
                           &QAbstractSocket::deleteLater);
    connect(pClientSocket, &QAbstractSocket::readyRead,
                     this, &MyServer::slotReadClient);

    sendToClient(pClientSocket, "server-response:connected");
}

void MyServer::slotStart() {

    textBox->append("*starting*");

    portLine    -> setDisabled(true);
    startButton -> setDisabled(true);
    stopButton  -> setDisabled(false);

    server = new QTcpServer(this);

    if (!server->listen(QHostAddress::Any, quint16(portLine->text().toInt()))) {

        QMessageBox::critical(nullptr, "SeverError",
                     "Unnable to start the server:" + server->errorString());

        server -> close();
        server = nullptr;

        portLine    -> setDisabled(false);
        startButton -> setDisabled(false);
        stopButton  -> setDisabled(true);

        return;
    }

    connect(server, &QTcpServer::newConnection,
              this, &MyServer::slotNewConnection);

    textBox -> append("#server is on...");
    textBox -> append(QString("host is ").append(server->serverAddress().toString()));

  }

void MyServer::slotStop() {

    if (server){

        disconnect(server, &QTcpServer::newConnection,
                        this, &MyServer::slotNewConnection);

        if (server->isListening())
            server -> close();

        server = nullptr;
    }

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
                   append(" ").append(str).toUtf8());
}




