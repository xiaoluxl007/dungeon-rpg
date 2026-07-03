QT       += core gui widgets

TARGET = dungeon-rpg
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        main.cpp \
        mainwindow.cpp \
        battlewindow.cpp \
        character.cpp \
        skill.cpp \
        item.cpp \
        equipment.cpp \
        dungeon.cpp \
        savemanager.cpp \
        shop.cpp

HEADERS += \
        mainwindow.h \
        battlewindow.h \
        character.h \
        skill.h \
        item.h \
        equipment.h \
        dungeon.h \
        savemanager.h \
        shop.h

CONFIG += c++11

QT_MAJOR_VERSION = $$[QT_VERSION_MAJOR]
equals(QT_MAJOR_VERSION, 6) {
    QT += core5compat
}
