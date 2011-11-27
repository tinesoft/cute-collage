#-------------------------------------------------
#
# Project created by QtCreator 2011-01-20T22:08:43
#
#-------------------------------------------------

QT       += core gui

TARGET = CuteCollage
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    photolister.cpp \
    imageutils.cpp \
    pixmaplistwidgetitem.cpp \
    collagecreator.cpp \
    characterwidget.cpp \
    imagelabel.cpp \
    textcollageshape.cpp \
    clickablelabel.cpp \
    graphicsview.cpp \
    drawingarea.cpp \
    customcollageshape.cpp \
    application.cpp

HEADERS  += mainwindow.h \
    photolister.h \
    imageutils.h \
    pixmaplistwidgetitem.h \
    collagecreator.h \
    characterwidget.h \
    imagelabel.h \
    textcollageshape.h \
    clickablelabel.h \
    graphicsview.h \
    drawingarea.h \
    customcollageshape.h \
    application.h

FORMS    += mainwindow.ui \
    photolister.ui \
    collagecreator.ui \
    textcollageshape.ui \
    customcollageshape.ui

TRANSLATIONS = translations/cutecollage_fr.ts \
    translations/cutecollage_en.ts

RESOURCES += \
    res.qrc
