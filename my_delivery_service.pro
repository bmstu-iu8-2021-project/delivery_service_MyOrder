QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ./sources/authorizationtrue.cpp \
    ./sources/couriers.cpp \
    ./sources/end.cpp \
    ./sources/main.cpp \
    ./sources/mainwindow.cpp \
    ./sources/orders.cpp

HEADERS += \
    ./headers/authorizationtrue.h \
    ./headers/couriers.h \
    ./headers/end.h \
    ./headers/mainwindow.h \
    ./headers/orders.h \
    ./headers/picosha2.h

FORMS += \
    ./ui/authorizationtrue.ui \
    ./ui/couriers.ui \
    ./ui/end.ui \
    ./ui/mainwindow.ui \
    ./ui/orders.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    ./resources/MyPictures.qrc \
