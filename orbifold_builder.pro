QT += core gui widgets opengl openglwidgets

TARGET = orbifold_builder
TEMPLATE = app

SOURCES += main.cpp \
    ResultsWidget.cpp \
    GeometryEngine.cpp \
    MainWindow.cpp \
    DiagramItem.cpp \
    Arrow.cpp \
    DiagramTextItem.cpp \
    DiagramScene.cpp

HEADERS += \
    ResultsWidget.h \
    GeometryEngine.h \
    MainWindow.h \
    DiagramItem.h \
    DiagramScene.h \
    Arrow.h \
    DiagramTextItem.h

# install
target.path = $$[QT_INSTALL_EXAMPLES]/opengl/orbifold_builder
INSTALLS += target
