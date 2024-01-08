#include <QApplication>
#include <QLabel>
#include <QSurfaceFormat>
#include "mainwindow.h"

#ifndef QT_NO_OPENGL
    #include "mainwidget.h"
#endif

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    QSurfaceFormat::setDefaultFormat(format);

    app.setApplicationName("manifold_builder");
    app.setApplicationVersion("0.1");

    // Diagram scene code
    MainWindow mainWindow;
    mainWindow.setGeometry(100, 100, 800, 500);
    mainWindow.show();

#ifndef QT_NO_OPENGL
    MainWidget widget;
    widget.show();
#else
    QLabel note("OpenGL Support required");
    note.show();
#endif
    return app.exec();
}
