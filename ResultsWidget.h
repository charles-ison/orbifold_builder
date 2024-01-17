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
    void setShouldPaintGL(bool newShouldPaintGL);
    void resizeGL(int w, int h) override;

protected:
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void wheelEvent(QWheelEvent *e) override;
    void timerEvent(QTimerEvent *e) override;
    void initializeGL() override;
    void paintGL() override;
    void initShaders();

private:
    Cube *cube;
    Sphere *sphere;
    Torus *torus;
    MobiusStrip *mobiusStrip;
    KleinBottle *kleinBottle;
    CrossCap *crossCap;
    QBasicTimer timer;
    QOpenGLShaderProgram program;
    GeometryEngine *cubeGeometryEngine = nullptr;
    GeometryEngine *sphereGeometryEngine = nullptr;
    GeometryEngine *torusGeometryEngine = nullptr;
    GeometryEngine *mobiusStripEngine = nullptr;
    GeometryEngine *kleinBottleEngine = nullptr;
    GeometryEngine *crossCapEngine = nullptr;
    GeometryEngine *geometryEngine = nullptr;
    QMatrix4x4 projection;
    QVector2D mousePressPosition;
    QVector3D rotationAxis;
    qreal angularSpeed = 0;
    QQuaternion rotation;
    bool shouldPaintGL = false;
};

#endif
