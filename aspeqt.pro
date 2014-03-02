# -------------------------------------------------
# Project created by QtCreator 2009-11-22T14:13:00
# Last Update: March 10, 2013
# -------------------------------------------------
DEFINES += VERSION=\\\"0.8.8\\\"
TARGET = aspeqt
TEMPLATE = app
CONFIG += qt
QT += core gui network
CONFIG += mobility
MOBILITY = bearer
SOURCES += main.cpp \
    mainwindow.cpp \
    sioworker.cpp \
    optionsdialog.cpp \
    aboutdialog.cpp \
    diskimage.cpp \
    diskimagepro.cpp \
    diskimageatx.cpp \
    folderimage.cpp \
    miscdevices.cpp \
    createimagedialog.cpp \
    diskeditdialog.cpp \
    aspeqtsettings.cpp \
    autoboot.cpp \
    autobootdialog.cpp \
    atarifilesystem.cpp \
    miscutils.cpp \
    textprinterwindow.cpp \
    cassettedialog.cpp \
    docdisplaywindow.cpp \
    bootoptionsdialog.cpp \
    network.cpp
win32:LIBS += -lwinmm
unix:LIBS += -lz
win32:SOURCES += serialport-win32.cpp
unix:SOURCES += serialport-unix.cpp
HEADERS += mainwindow.h \
    serialport.h \
    sioworker.h \
    optionsdialog.h \
    aboutdialog.h \
    diskimage.h \
    diskimagepro.h \
    diskimageatx.h \
    folderimage.h \
    miscdevices.h \
    createimagedialog.h \
    diskeditdialog.h \
    aspeqtsettings.h \
    autoboot.h \
    autobootdialog.h \
    atarifilesystem.h \
    miscutils.h \
    textprinterwindow.h \
    cassettedialog.h \
    docdisplaywindow.h \
    bootoptionsdialog.h \
    network.h
win32:HEADERS += serialport-win32.h
unix:HEADERS += serialport-unix.h
FORMS += mainwindow.ui \
    optionsdialog.ui \
    aboutdialog.ui \
    createimagedialog.ui \
    diskeditdialog.ui \
    autobootdialog.ui \
    textprinterwindow.ui \
    cassettedialog.ui \
    helpcontentwindow.ui \
    docdisplaywindow.ui \
    bootoptionsdialog.ui
RESOURCES += icons.qrc \
    atarifiles.qrc \
    i18n.qrc \
    documentation.qrc \
    images.qrc
OTHER_FILES += \
    license.txt \
    history.txt \
    atascii_read_me.txt \
    atari/aspecl/aspecl.asm \
    AspeQt.rc \
    AspeQt User Manual.html \
    about.html
TRANSLATIONS = i18n/aspeqt_pl.ts \
               i18n/aspeqt_tr.ts \
               i18n/aspeqt_ru.ts \
               i18n/aspeqt_sk.ts \
               i18n/aspeqt_de.ts \
               i18n/aspeqt_es.ts \
               i18n/qt_pl.ts \
               i18n/qt_tr.ts \
               i18n/qt_ru.ts \
               i18n/qt_sk.ts \
               i18n/qt_de.ts \
               i18n/qt_es.ts

RC_FILE = AspeQt.rc \




