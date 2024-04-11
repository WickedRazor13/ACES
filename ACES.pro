QT       += core gui bluetooth

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ACES.cpp \
    bluetoothmanager.cpp \
    differenceItem.cpp \
    differenceLocations.cpp \
    imagescene.cpp \
    main.cpp \
    measurementModule.cpp \
    memory.cpp \
    readyForm.cpp \
    spotTheDiff.cpp \
    widget.cpp

HEADERS += \
    ACES.h \
    bluetoothmanager.h \
    differenceItem.h \
    differenceLocations.h \
    imagescene.h \
    measurementModule.h \
    memory.h \
    readyForm.h \
    spotTheDiff.h \
    widget.h

FORMS += \
    ACES.ui \
    memory.ui \
    readyForm.ui \
    spotTheDiff.ui \
    widget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
