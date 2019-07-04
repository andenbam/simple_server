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
    connect(stopButton,  &QPushButton::pressed,
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

    clientsMap = new QMap<qintptr, QAbstractSocket*>();
    clientsDescritors = new QList<qintptr>;

    connect(server, &QTcpServer::newConnection,
              this, &MyServer::slotNewConnection);

    textBox -> append("#server is on...");
    textBox -> append(QString("host is ").append(server->serverAddress().toString()));
  }

void MyServer::slotStop() {

    for (int i=0; i < clientsDescritors->size(); i++) {

        QAbstractSocket* client = clientsMap->value(clientsDescritors->at(i));
        sendToClient(client, "DISCONNECTING");
        client->close();
    }

    if (server){

        disconnect(server, &QTcpServer::newConnection,
                     this, &MyServer::slotNewConnection);

        server -> close();
        delete server;
        server = nullptr;
    }

    textBox->clear();

    portLine    -> setDisabled(false);
    startButton -> setDisabled(false);
    stopButton  -> setDisabled(true);
}

void MyServer::slotNewConnection() {

    QTcpSocket* clientSocket = server->nextPendingConnection();
    qintptr ptr = clientSocket->socketDescriptor();
    if (clientsMap->value(ptr, nullptr) == nullptr){

        clientsDescritors -> push_back(ptr);
        clientsMap        -> insert(ptr, clientSocket);
        textBox->append(QString("new user [").append(QString::number(ptr)).append("] connected"));
    } else {

        textBox->append(QString("user [").append(QString::number(ptr)).append("] reconnected"));
    }

    connect(clientSocket, &QAbstractSocket::disconnected,
                          &QAbstractSocket::deleteLater);
    connect(clientSocket, &QAbstractSocket::readyRead,
                    this, &MyServer::slotReadClient);

    sendToClient(clientSocket, "CONNECTED");
}

void MyServer::slotReadClient() {

    QAbstractSocket* clientSocket = static_cast<QAbstractSocket*>(sender());

    qintptr clientIndex = -1;

    for(int i = 0; i < clientsDescritors->size(); i++){

        if (clientsDescritors->takeAt(i) == clientSocket->socketDescriptor()){

            clientIndex = clientSocket->socketDescriptor();
            break;
        }
    }

    QString incomMessage = QString::fromUtf8(clientSocket->read(256));

    textBox -> append(QString(QTime::currentTime().toString(Qt::LocalDate)
              .append(" ").append("client[").append(QString::number(clientIndex)).append("]: ")
              .append(incomMessage)));

    sendToClient(clientSocket, QString("RECEIVED {").append(incomMessage).append("}"));
}

//46.0.199.93
//5000
void MyServer::sendToClient(QAbstractSocket *client, const QString &message) {

    client->write(message.toUtf8());
}




