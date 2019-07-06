#include <QApplication>
#include <QtWidgets>
#include "MyServer.h"

int main(int argc, char *argv[]) {

    QApplication app(argc, argv);
    MyServer* server = new MyServer();
    server -> show();
    return app.exec();
}
