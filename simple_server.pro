QT += core gui widgets network
CONFIG += app_bundle static
DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        main.cpp \
        SslServer.cpp \
        myserver.cpp \
        testexternaladdress.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    SslServer.h \
    myserver.h \
    testexternaladdress.h

DISTFILES += \
    genSSL.sh \
    server.crt \
    server.key
