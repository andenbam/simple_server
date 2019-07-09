#include "MyServer.h"
#include "testexternaladdress.h"
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
#include <QNetworkInterface>
#include <QHostInfo>

MyServer::MyServer() : QWidget () {

    textBox     = new QTextEdit();
    linePort    = new QLineEdit();
    buttonStart = new QPushButton("&Start");
    buttonStop  = new QPushButton("&Stop");
    lineUsers      = new QLineEdit();
    lineUsers     -> setText("0");
    lineUsers     -> setDisabled(true);
    textBox    -> setReadOnly(true);
    linePort   -> setPlaceholderText("Port num.");
    linePort   -> setText("5005");
    buttonStop -> setDisabled(true);

    connect(buttonStart, &QPushButton::pressed,
                   this, &MyServer::slotStart);
    connect(buttonStop,  &QPushButton::pressed,
                   this, &MyServer::slotStop);

    mainLayout = new QVBoxLayout();
    QHBoxLayout* hPanel = new QHBoxLayout();
    QHBoxLayout* lPanel = new QHBoxLayout();

    hPanel -> addWidget(new QLabel("<H3>Local Server</H3> "));
    QLabel* usrs = new QLabel("<H3>users online:</H3>");
    hPanel -> addWidget(usrs);

    usrs->setAlignment(Qt::AlignmentFlag::AlignVCenter);

    hPanel -> addWidget(lineUsers);
    lPanel -> addWidget(linePort);
    lPanel -> addWidget(buttonStart);
    lPanel -> addWidget(buttonStop);

    mainLayout -> addLayout(hPanel);
    mainLayout -> addWidget(textBox);
    mainLayout -> addLayout(lPanel);

    setLayout(mainLayout);
}

void MyServer::show()
{   
    QWidget::show();

    clearConsole();

    int fontSize = linePort->font().pointSize() > 12 ? 24 : 12;

    int totalHeight = mainLayout->geometry().height();
    int totalWidth  = mainLayout->geometry().width();
    QFont font      = QFont(linePort->font().family(), fontSize);

    linePort    -> setMinimumHeight(totalHeight / 10);
    buttonStart -> setMinimumHeight(totalHeight / 10);
    buttonStop  -> setMinimumHeight(totalHeight / 10);
    lineUsers   -> setMinimumHeight(totalHeight / 12);
    lineUsers   -> setMaximumWidth(totalWidth / 6);
    linePort    -> setMaximumWidth(totalWidth / 4);
    linePort    -> setFont(font);
    textBox     -> setFont(font);
    lineUsers   -> setFont(font);
    buttonStart -> setFont(font);
    buttonStop  -> setFont(font);
}

void MyServer::gotExternalAddress(QString address) {

    textBox -> append(QString("External address: ").append(address));
}

void MyServer::slotStart() {

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
    clientsNamesMap = new QMap<QAbstractSocket*, QString>();

    connect(server, &QTcpServer::newConnection,
              this, &MyServer::slotNewConnection);

    textBox -> append("SERVER IS LISTENING...");

    QList<QHostAddress> list = QNetworkInterface::allAddresses();
    QHostAddress address;
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

    linePort    -> setDisabled(false);
    buttonStart -> setDisabled(false);
    buttonStop  -> setDisabled(true);
}

void MyServer::slotNewConnection(){

    QTcpSocket* clientSocket = server->nextPendingConnection();

    clientsList    -> push_back(clientSocket);
    qintptr desc = clientSocket->socketDescriptor();
    QString newName = QString(namesBuffer->at((285 * QTime::currentTime().msec() * desc) % 16))
            .append(QString(QString::number((592 * int(QTime::currentTime().msec() * desc) % 999))));
    clientsNamesMap -> insert(clientSocket, newName);

    connect(clientSocket, &QAbstractSocket::disconnected,
                      this, &MyServer::slotDisconnected);

    connect(clientSocket, &QAbstractSocket::readyRead,
                    this, &MyServer::slotReadClient);

    textBox-> append(QString("user \"")
                     .append(clientsNamesMap->value(clientSocket, ""))
                     .append("\" connected"));

    lineUsers->setText(QString::number(clientsList->size()));

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

            textBox        -> append(QString("user \"")
                             .append(clientsNamesMap->value(clientSocket, ""))
                             .append("\" disconnected"));

            clientsNamesMap -> remove(clientSocket);
        }
    }

    lineUsers -> setText(QString::number(clientsList->size()));
}

void MyServer::slotReadClient() {

    QAbstractSocket* clientSocket = static_cast <QAbstractSocket*> (sender());

    QString incomMessage = QString::fromUtf8(clientSocket->read(256));

    textBox -> append(QString(QTime::currentTime().toString(Qt::LocalDate)
              .append(" | ").append(clientsNamesMap->value(clientSocket))
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
                if(address.ip().protocol() == QAbstractSocket::IPv4Protocol)
                    textBox -> append(QString("Local address: ").append(address.ip().toString()));
            }
        }
    }

    TestExternalAddress* tea = new TestExternalAddress();
    connect(tea, &TestExternalAddress::gotAddress, this, &MyServer::gotExternalAddress);
}

void MyServer::sendToClient(QAbstractSocket *clientSocket, const QString &message) {

    clientSocket -> write(message.toUtf8());
}

void MyServer::broadcastFrom(QAbstractSocket * sender, const QString & msg)
{
    QString desc = QString::number(sender->socketDescriptor());

    for (int i = 0; i < clientsList->size(); i++){

        QString message = QString(
                    (clientsList->at(i) != sender) ?
                    QString(clientsNamesMap->value(sender))
                    : "you")
                    .append(" : ").append(msg);

        sendToClient(clientsList->at(i), message);
    }
}


