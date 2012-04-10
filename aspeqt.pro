# -------------------------------------------------
# Project created by QtCreator 2009-11-22T14:13:00
# -------------------------------------------------
DEFINES += VERSION=\\\"0.8.1\\\"
TARGET = aspeqt
TEMPLATE = app
CONFIG += qt
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
    docdisplaywindow.cpp
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
    docdisplaywindow.h
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
    docdisplaywindow.ui
RESOURCES += icons.qrc \
    atarifiles.qrc \
    i18n.qrc
OTHER_FILES += todo.txt \
    readme.txt \
    license.txt \
    compile.txt \
    history.txt \
    atascii_read_me.txt \
    atari/aspecl/aspecl.asm \
    AspeQt.rc
TRANSLATIONS = i18n/aspeqt_pl.ts \
               i18n/aspeqt_tr.ts \
               i18n/aspeqt_ru.ts \
               i18n/qt_pl.ts
RC_FILE = AspeQt.rc \
