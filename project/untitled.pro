QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#INCLUDEPATH += C:/Program Files/OpenSSL-Win64/include
##include($$PWD/../Qt-Secret/src/Qt-Secret.pri)

#LIBS += -LC:‪/Program Files/OpenSSL-Win64/lib/ -llibssl
#LIBS += -LC:‪/Program Files/OpenSSL-Win64/lib -llibcrypto

SOURCES += \
    admininterface.cpp \
    main.cpp \
    mainwindow.cpp \
    userinterface.cpp \
    usersdata.cpp \
    C_crc16.cpp \


HEADERS += \
    admininterface.h \
    mainwindow.h \
    userinterface.h \
    usersdata.h \
    C_crc16.h \


## Default rules for deployment.
#qnx: target.path = /tmp/$${TARGET}/bin
#else: unix:!android: target.path = /opt/$${TARGET}/bin
#!isEmpty(target.path): INSTALLS += target

CONFIG(debug,debug|release){
      TARGET = "../../run/untitled_dbg" #дебажный экзешник
}else{TARGET = "../../run/untitled"     #релизный
}


RESOURCES +=


