#include <QApplication>
#include <QSurfaceFormat>
#include "MainWindow.h"
#include "ResultsWidget.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("manifold_builder");
    app.setApplicationVersion("0.1");

    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    QSurfaceFormat::setDefaultFormat(format);

    MainWindow mainWindow;
    mainWindow.setGeometry(100, 100, 800, 500);
    mainWindow.show();

    //ResultsWidget resultsWidget;
    //resultsWidget.show();

    return app.exec();
}
