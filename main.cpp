#include <QApplication>
#include <QtWidgets>
#include "MyServer.h"

int main(int argc, char *argv[]) {

    QApplication a(argc, argv);

    MyServer server(5005);
    server.show();

    return a.exec();
}
