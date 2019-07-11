#include "myserver.h"
#include "testexternaladdress.h"
#include <QMessageBox>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QLabel>
#include <QDataStream>
#include <QTime>
#include <QLineEdit>
#include <QPushButton>
#include <QNetworkInterface>
#include <QHostInfo>
#include <QGroupBox>

MyServer::MyServer() : QWidget () {

    textBox     = new QTextEdit();
    linePort    = new QLineEdit();
    buttonStart = new QPushButton("&Start");
    buttonStop  = new QPushButton("&Stop");
    lineUsers      = new QLineEdit();
    lineUsers     -> setText("0");
    lineUsers     -> setModified(false);
    textBox    -> setReadOnly(true);
    linePort   -> setPlaceholderText("#port");
    linePort   -> setText("5005");
    buttonStop -> setDisabled(true);

    connect(linePort, &QLineEdit::returnPressed,
                buttonStart, &QPushButton::click);

    connect(buttonStart, &QPushButton::pressed,
                   this, &MyServer::slotStart);
    connect(buttonStop,  &QPushButton::pressed,
                   this, &MyServer::slotStop);
}

void MyServer::show() {

    connect(new TestExternalAddress(), &TestExternalAddress::gotAddress,
                                 this, &MyServer::gotExternalAddress);

    mainLayout          = new QVBoxLayout();
    QHBoxLayout* hPanel = new QHBoxLayout();

    hPanel -> addWidget(linePort);
    hPanel -> addWidget(buttonStart);
    hPanel -> addWidget(buttonStop);
    hPanel -> addWidget(new QLabel("users online:"));
    hPanel -> addWidget(lineUsers);

    QGroupBox* gBox = new QGroupBox("Server Settings");
    QGroupBox* oBox = new QGroupBox("Simple Server Output");

    QVBoxLayout* vboxlayout = new QVBoxLayout();
    vboxlayout->addWidget(textBox);

    gBox   -> setLayout(hPanel);
    oBox   -> setLayout(vboxlayout);
    mainLayout -> addWidget(gBox);
    mainLayout -> addWidget(oBox);

    setLayout(mainLayout);

    QWidget::show();

    clearConsole();
}

void MyServer::gotExternalAddress(QString address) {

    externalAddress = address;
    textBox -> append(QString("External address: ").append(externalAddress));
}

void MyServer::slotStart() {

    linePort    -> setDisabled(true);
    buttonStart -> setDisabled(true);
    buttonStop  -> setDisabled(false);

    server = new SslServer(this);
    server->setSslLocalCertificate("server.crt");
    server->setSslPrivateKey("server.key", QSsl::Rsa,QSsl::Pem);
    server->setSslProtocol(QSsl::TlsV1_2);

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

    clientsList    = new QList<QSslSocket*>();
    clientsNamesMap = new QMap<qintptr, QString>();

    connect(server, &QTcpServer::newConnection,
              this, &MyServer::slotNewConnection);

 //   QThread* thread = new ServerThread(server);
  //  thread->start();

    textBox -> append("SERVER IS LISTENING...");
}

void MyServer::slotStop(){

    if (server){

        disconnect(server, &QTcpServer::newConnection,
                     this, &MyServer::slotNewConnection);

        server -> close();
        delete server;
        server = nullptr;
    }

    clearConsole();
    clientsList -> clear();
    lineUsers   -> setText(QString::number(clientsList->size()));
    linePort    -> setDisabled(false);
    buttonStart -> setDisabled(false);
    buttonStop  -> setDisabled(true);
}

void MyServer::slotNewConnection(){

    QSslSocket* clientSocket = dynamic_cast<QSslSocket*> (server->nextPendingConnection());
    if (clientsList->contains(clientSocket)) return;

    clientsList    -> push_back(clientSocket);

    QString newName = QString(namesBuffer->at((757 * QTime::currentTime().msec()) % 16))
            .append(QString(QString::number((2053 * int(QTime::currentTime().msec()) % 1000))));

    clientsNamesMap -> insert(clientSocket->socketDescriptor(), newName);

    connect(clientSocket, &QSslSocket::disconnected,
                      this, &MyServer::slotDisconnected);

    connect(clientSocket, &QSslSocket::readyRead,
                    this, &MyServer::slotReadClient);

    textBox-> append(QString("user \"")
                     .append(clientsNamesMap->value(clientSocket->socketDescriptor()))
                     .append("\" connected"));

    lineUsers->setText(QString::number(clientsList->size()));

    sendToClient(clientSocket, "CONNECTED");
}

void MyServer::slotDisconnected(){

    int disconn = -1;

    for (int i = 0; i < clientsList->size(); i++){
        if (clientsList->at(i)->state() == QSslSocket::SocketState::UnconnectedState){

            disconn = i;
            break;
        }
    }

    QSslSocket* clientSocket = clientsList->at(disconn);

    disconnect(clientSocket, &QSslSocket::disconnected,
                       this, &MyServer::slotDisconnected);

    disconnect(clientSocket, &QSslSocket::readyRead,
                       this, &MyServer::slotReadClient);

    clientsList    -> removeAll(clientSocket);

    textBox        -> append(QString("user \"")
                     .append(clientsNamesMap->value(clientSocket->socketDescriptor(), ""))
                     .append("\" disconnected"));

    clientsNamesMap -> remove(clientSocket->socketDescriptor());

    lineUsers -> setText(QString::number(clientsList->size()));
}

void MyServer::slotReadClient() {

    QSslSocket* clientSocket = static_cast <QSslSocket*> (sender());

    QString incomMessage = QString(clientSocket->readAll());

    textBox -> append(QString(QTime::currentTime().toString(Qt::LocalDate)
              .append(" | ").append(clientsNamesMap->value(clientSocket->socketDescriptor()))
              .append(" : ").append(incomMessage)));

    broadcastFrom(clientSocket, incomMessage);
}

//FORS host = 46.0.199.93 : 5000
void MyServer::clearConsole()
{
    textBox->clear();

    foreach (const QNetworkInterface &netInterface, QNetworkInterface::allInterfaces()) {
        QNetworkInterface::InterfaceFlags flags = netInterface.flags();
        if( bool((flags & QNetworkInterface::IsRunning)) && !bool(flags & QNetworkInterface::IsLoopBack)){
            foreach (const QNetworkAddressEntry &address, netInterface.addressEntries()) {
                if(address.ip().protocol() == QSslSocket::IPv4Protocol)
                    textBox -> append(QString("Local address: ").append(address.ip().toString()));
            }
        }
    }
    if (externalAddress != "")
        textBox -> append(QString("External address: ").append(externalAddress));
}

void MyServer::sendToClient(QSslSocket* socket, const QString &message) {

    socket -> write(message.toUtf8());
}

void MyServer::broadcastFrom(QSslSocket* socket, const QString & msg)
{
    for (int i = 0; i < clientsList->size(); i++){

        QString message = QString(
                    (clientsList->at(i) != socket) ?
                    QString(clientsNamesMap->value(socket->socketDescriptor()))
                    : "you")
                    .append(" : ").append(msg);

        sendToClient(clientsList->at(i), message);
    }
}


