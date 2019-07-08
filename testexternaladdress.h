#ifndef TESTEXTERNALADDRESS_H
#define TESTEXTERNALADDRESS_H

/*
    Этот класс был взят с официального форума forum.qt.io :
    https://forum.qt.io/topic/86025/qt-get-external-ip-address-using-qnetworkreply/2

    Данный класс представляет собой реализацию способа обнаружения внешнего
    ip-адреса устройства с помощью подключения к интернет-сервису api.ipify.org
    Решает проблему отыскания адреса для осуществления подключения
    к нашему серверу Simple Server.
*/

#include <QJsonDocument>
#include <QJsonObject>
#include <QHostAddress>
#include <QNetworkReply>

class TestExternalAddress : public QObject
{
    Q_OBJECT

public:
    TestExternalAddress();

private slots:
    void gotReply(QNetworkReply* networkReply);
};

#endif // TESTEXTERNALADDRESS_H
