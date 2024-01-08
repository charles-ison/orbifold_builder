QT += core gui widgets opengl openglwidgets

TARGET = orbifold_builder
TEMPLATE = app

SOURCES += main.cpp \
    mainwidget.cpp \
    geometryengine.cpp \
    mainwindow.cpp \
    diagramitem.cpp \
    arrow.cpp \
    diagramtextitem.cpp \
    diagramscene.cpp

HEADERS += \
    mainwidget.h \
    geometryengine.h \
    mainwindow.h \
    diagramitem.h \
    diagramscene.h \
    arrow.h \
    diagramtextitem.h

# install
target.path = $$[QT_INSTALL_EXAMPLES]/opengl/orbifold_builder
INSTALLS += target
