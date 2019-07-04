#include "MyServer.h"
#include <QMessageBox>
#include <QTcpServer>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QLabel>
#include <QDataStream>
#include <QTcpSocket>
#include <QTime>

MyServer::MyServer(int nPort, QWidget* widget /*=0*/) :
                QWidget (widget) {
    tcpServer = new QTcpServer(this);
    if (!tcpServer->listen(QHostAddress::Any, nPort)) {
        QMessageBox::critical(nullptr, "SeverError",
                              "Unnable to start the server:" +
                              tcpServer->errorString());
        tcpServer->close();
        return;
    }
    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(slotNewConnection()));
    txt = new QTextEdit;
    txt->setReadOnly(true);

    //layout setup
    QVBoxLayout* pvbxLayout = new QVBoxLayout;
    pvbxLayout->addWidget(new QLabel("<H1>Server</H1>"));
    pvbxLayout->addWidget(txt);
    setLayout(pvbxLayout);
}

//MyServer::~MyServer(){}

void MyServer::slotNewConnection() {
    QTcpSocket* pClientSocket = tcpServer->nextPendingConnection();
    connect(pClientSocket, SIGNAL(disconnected()), pClientSocket, SLOT(deleteLater()));
    connect(pClientSocket, SIGNAL(readyRead()), this, SLOT(slotReadClient()));
    sendToClient(pClientSocket, "Server Response: Connected!");
}

void MyServer::slotReadClient() {
    QTcpSocket* pClientSocket = (QTcpSocket*)sender();
    char* data = new char[256];
    pClientSocket->read(data,256);
    QString str = QString(data);
    txt->append(str);
    sendToClient(pClientSocket, "Server response: Received \"" + str + "\"");
}

//46.0.199.93
//5000
void MyServer::sendToClient(QTcpSocket *pSocket, const QString &str) {
    pSocket->write(QTime::currentTime().toString(Qt::LocalDate).
                   append(" ").append(str).toStdString().c_str());
}




