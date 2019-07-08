#include "testexternaladdress.h"
#include "qdebug.h"

/*
    Этот класс был взят с официального форума forum.qt.io :
    https://forum.qt.io/topic/86025/qt-get-external-ip-address-using-qnetworkreply/2

    Данный класс представляет собой реализацию способа обнаружения внешнего
    ip-адреса устройства с помощью подключения к интернет-сервису api.ipify.org
    Решает проблему отыскания адреса для осуществления подключения
    к нашему серверу Simple Server.
*/

TestExternalAddress::TestExternalAddress()
{
    QNetworkAccessManager* manager = new QNetworkAccessManager(this);
    connect(manager,SIGNAL(finished(QNetworkReply*)),SLOT(gotReply(QNetworkReply*)));
    manager->get(QNetworkRequest(QUrl("https://api.ipify.org?format=json")));
}

void TestExternalAddress::gotReply(QNetworkReply* networkReply)
{
    networkReply->deleteLater();
    qDebug() <<  QHostAddress(QJsonDocument::fromJson(networkReply->readAll()).object().value("ip").toString());
}
