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
    linePort    = new QLineEdit();
    buttonStart = new QPushButton("&Start");
    buttonStop  = new QPushButton("&Stop");
    usersAmountField      = new QLineEdit();
    usersAmountField     -> setText("0");
    usersAmountField     -> setDisabled(true);
    textBox    -> setReadOnly(true);
    linePort   -> setPlaceholderText("Port num.");
    linePort   -> setText("5005");
    buttonStop -> setDisabled(true);

    connect(buttonStart, &QPushButton::pressed,
                   this, &MyServer::slotStart);
    connect(buttonStop,  &QPushButton::pressed,
                   this, &MyServer::slotStop);

    QVBoxLayout* layout = new QVBoxLayout();
    QHBoxLayout* hPanel = new QHBoxLayout();
    QHBoxLayout* lPanel = new QHBoxLayout();

    hPanel -> addWidget(new QLabel("<H1>Local Server</H1>"));
    hPanel -> addWidget(new QLabel("users online:"));
    hPanel -> addWidget(usersAmountField);
    lPanel -> addWidget(linePort);
    lPanel -> addWidget(buttonStart);
    lPanel -> addWidget(buttonStop);

    layout -> addLayout(hPanel);
    layout -> addWidget(textBox);
    layout -> addLayout(lPanel);

    setLayout(layout);
}

void MyServer::slotStart() {

    textBox->append("*starting*");

    linePort    -> setDisabled(true);
    buttonStart -> setDisabled(true);
    buttonStop  -> setDisabled(false);

    server = new QTcpServer(this);

    if (!server->listen(QHostAddress::Any, quint16(linePort->text().toInt()))) {

        QMessageBox::critical(nullptr, "SeverError",
                     "Unnable to start the server:" + server->errorString());

        server -> close();
        server = nullptr;

        linePort    -> setDisabled(false);
        buttonStart -> setDisabled(false);
        buttonStop  -> setDisabled(true);

        return;
    }

    clientsList    = new QList<QAbstractSocket*>();
    clientsDescMap = new QMap<QAbstractSocket*, qintptr>();

    connect(server, &QTcpServer::newConnection,
              this, &MyServer::slotNewConnection);

    textBox -> append("#server is on...");
    textBox -> append(QString("host is ").append(server->serverAddress().toString()));
  }

void MyServer::slotStop() {

    if (server){

        disconnect(server, &QTcpServer::newConnection,
                     this, &MyServer::slotNewConnection);

        server -> close();
        delete server;
        server = nullptr;
    }

    textBox->clear();

    linePort    -> setDisabled(false);
    buttonStart -> setDisabled(false);
    buttonStop  -> setDisabled(true);
}

void MyServer::slotNewConnection() {

    QTcpSocket* clientSocket = server->nextPendingConnection();

    clientsList    -> push_back(clientSocket);
    clientsDescMap -> insert(clientSocket, clientSocket->socketDescriptor());

    connect(clientSocket, &QAbstractSocket::disconnected,
                      this, &MyServer::slotDisconnected);

    connect(clientSocket, &QAbstractSocket::readyRead,
                    this, &MyServer::slotReadClient);

    textBox->append(QString("user[")
                     .append(QString::number(clientSocket->socketDescriptor()))
                     .append("] connected"));

    usersAmountField->setText(QString::number(clientsList->size()));

    sendToClient(clientSocket, "CONNECTED");
}

void MyServer::slotDisconnected(){

    for (int i = 0; i < clientsList->size(); i++){

        if (clientsList->at(i)->state() == QAbstractSocket::SocketState::UnconnectedState){

            QAbstractSocket* clientSocket = clientsList->at(i);

            disconnect(clientSocket, &QAbstractSocket::disconnected,
                               this, &MyServer::slotDisconnected);

            disconnect(clientSocket, &QAbstractSocket::readyRead,
                               this, &MyServer::slotReadClient);

            clientsList    -> removeAll(clientSocket);

            textBox->append(QString("user[")
                             .append(QString::number(clientsDescMap->value(clientSocket)))
                             .append("] disconnected"));

            clientsDescMap -> remove(clientSocket);
        }
    }

    usersAmountField->setText(QString::number(clientsList->size()));
}

void MyServer::slotReadClient() {

    QAbstractSocket* clientSocket =static_cast<QAbstractSocket*>(sender());

    qintptr desc = -1;

    for (int i = 0; i < clientsList->size(); i++) {
        if (clientsList->at(i) == clientSocket){
            desc = clientSocket->socketDescriptor();
            break;
        }
    }

    QString incomMessage = QString::fromUtf8(clientSocket->read(256));

    textBox -> append(QString(QTime::currentTime().toString(Qt::LocalDate)
              .append(" ").append("client[").append(QString::number(desc)).append("]: ")
              .append(incomMessage)));

    broadcast(int(desc), incomMessage);
}

//FORS host = 46.0.199.93 : 5000
void MyServer::sendToClient(QAbstractSocket *client, const QString &message) {

    client->write(message.toUtf8());
}

void MyServer::broadcast(const int user, const QString & msg)
{
    for (int i = 0; i < clientsList->size(); i++){

        QString message = QString("user[").append(QString::number(user)).append("]:\n").append(msg);

        sendToClient(clientsList->at(i), message);
    }
}




