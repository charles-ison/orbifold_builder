#ifndef RESULTSWIDGET_H
#define RESULTSWIDGET_H

#include "GeometryEngine.h"
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QMatrix4x4>
#include <QQuaternion>
#include <QVector2D>
#include <QBasicTimer>
#include <QOpenGLShaderProgram>
#include "surfaces/Cube.h"
#include "surfaces/Sphere.h"
#include "surfaces/Torus.h"
#include "surfaces/MobiusStrip.h"
#include "surfaces/KleinBottle.h"
#include "surfaces/CrossCap.h"

class GeometryEngine;

class ResultsWidget : public QOpenGLWidget, protected QOpenGLFunctions {

    Q_OBJECT

public:
    using QOpenGLWidget::QOpenGLWidget;
    ~ResultsWidget();
    enum surface {sphere, cube, torus, mobiusStrip, crossCap, kleinBottle};
    void addSurface(surface newSurface);
    void resizeGL(int w, int h) override;

protected:
    void mousePressEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void wheelEvent(QWheelEvent *e) override;
    void timerEvent(QTimerEvent *e) override;
    void initializeGL() override;
    void paintGL() override;
    void initShaders();

private:
    Surface *currentSurface;
    Cube *cubeSurface;
    Sphere *sphereSurface;
    Torus *torusSurface;
    MobiusStrip *mobiusStripSurface;
    KleinBottle *kleinBottleSurface;
    CrossCap *crossCapSurface;
    QBasicTimer timer;
    QOpenGLShaderProgram program;
    GeometryEngine *geometryEngine = nullptr;
    QMatrix4x4 projection;
    QVector2D mousePressPosition;
    QVector3D rotationAxis;
    qreal angularSpeed = 0;
    QQuaternion rotation;
    bool shouldPaintGL = false;
    std::vector<VertexData> lineVertices;
    int initialParentWidth;
    int initialParentHeight;
};

#endif
