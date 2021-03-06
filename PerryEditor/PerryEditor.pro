#-------------------------------------------------
#
# Project created by QtCreator 2011-08-29T21:51:56
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PerryEditor
TEMPLATE = app

# engine
INCLUDEPATH += ../Engine

Release:LIBS += -L../Engine-build/Release
Debug:LIBS += -L../Engine-build/debug


LIBS += -lEngine
LIBS += -lopengl32 -lglu32

#GLEW
INCLUDEPATH += D:/Lavoro/DOCS/glew-1.6.0/include
LIBS += d:/Lavoro/DOCS/glew-1.6.0/lib/glew32.lib
LIBS += d:/Lavoro/DOCS/glew-1.6.0/bin/glew32.dll
# LIBS += -Ld:/Lavoro/DOCS/glew-1.6.0/lib -lglew32

# OBJ Loader
INCLUDEPATH += ../ObjLoader
Release:LIBS += -L../ObjLoader-build/Release
Debug:LIBS += -L../ObjLoader-build/Debug
LIBS += -lObjLoader

# ---
SOURCES += main.cpp \
		mainwindow.cpp \
		perryview.cpp \
    editorcamera.cpp \
    qlog.cpp \
    renderingoptions.cpp \
    mouseactions.cpp \
    mouseselect.cpp \
    selection.cpp \
    standardmouseactions.cpp \
    mousemove.cpp \
    mouserotate.cpp \
    qimageloader.cpp \
	savescreenshot.cpp\
	materialeditor.cpp \ 
    colorcombobox.cpp \
    slidercombobox.cpp \
    undoredo.cpp \
	undoredoactions.cpp

HEADERS  += mainwindow.h \
		perryview.h \
    editorcamera.h \
    qlog.h \
    renderingoptions.h \
    mouseactions.h \
    mouseaction.h \
    mouseselect.h \
    selection.h \
    standardmouseactions.h \
    mousemove.h \
    mouserotate.h \
    qimageloader.h \
    savescreenshot.h \
	materialeditor.h \ 
    colorcombobox.h \
    slidercombobox.h \
    undoredo.h \
	undoredoactions.h

FORMS    += mainwindow.ui \
    qlog.ui \
    renderingoptions.ui \
    savescreenshot.ui \
	materialeditor.ui

OTHER_FILES += \
    icons/file_icon.png \
    icons/select_icon.png \
    icons/scale_icon.png \
    icons/rotate_icon.png \
    icons/move_icon.png \
    icons/file_icon.png \
    icons/door_icon.png \
    icons/document_icon.png \
    icons/diskette_icon.png

RESOURCES += \
    Icons.qrc \
    Cursors.qrc

linux-g++{
    QMAKE_POST_LINK += cp ../Engine/ExeDir/*.* .
}

win32 {
    QMAKE_POST_LINK += copy ..\\Engine\\ExeDir\\*.* .
}
