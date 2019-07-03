#include <QCoreApplication>
#include "MyServer.h"
#include <QtWidgets>

int main(int argc, char *argv[]) {

    QCoreApplication a(argc, argv);

    MyServer server(5005);
    //lolkek
    server->startServer();
    return a.exec();
}
