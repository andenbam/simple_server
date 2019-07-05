#include <QApplication>
#include <QtWidgets>
#include "MyServer.h"

int main(int argc, char *argv[]) {

    QApplication app(argc, argv);
    (new MyServer())->show();
    return app.exec();
}
