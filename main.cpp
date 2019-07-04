#include <QApplication>
#include <QtWidgets>
#include "MyServer.h"

int main(int argc, char *argv[]) {

    QApplication a(argc, argv);
    (new MyServer())->show();
    return a.exec();
}
