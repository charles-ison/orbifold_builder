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
#include "Mesh.h"

class GeometryEngine;

class ResultsWidget : public QOpenGLWidget, protected QOpenGLFunctions {

    Q_OBJECT

public:
    using QOpenGLWidget::QOpenGLWidget;
    ~ResultsWidget();
    enum surface {sphere, cube, torus, mobiusStrip, crossCap, kleinBottle};
    void addSurface(surface newSurface);
    void cutSurface();
    void setLineDrawingColor(QColor newColor);
    void resizeGL(int w, int h) override;
    void toggleShouldDeleteSurface();

protected:
    void mousePressEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void wheelEvent(QWheelEvent *e) override;
    void timerEvent(QTimerEvent *e) override;
    void initializeGL() override;
    void paintGL() override;
    void initShaders();

private:
    void deleteSurface(QMouseEvent *e);
    void addLineVertices(Vertex *newVertex);
    std::vector<Vertex*> getNewVertices(Vertex *newVertex);
    void checkLineVerticesForLoop(Vertex *newVertex);
    Vertex* getVertexFromMouseEvent(QMouseEvent *e);

    Mesh *mesh;
    QBasicTimer timer;
    QOpenGLShaderProgram program;
    GeometryEngine *geometryEngine = nullptr;
    QMatrix4x4 projection;
    QMatrix4x4 mvp_matrix;
    QVector3D rotationAxis;
    qreal angularSpeed = 0;
    QQuaternion rotation;
    std::vector<Vertex*> lineVertices;
    QColor lineDrawingColor;
    bool isDrawingMode;
    bool shouldPaintGL;
    bool shouldDeleteSurface;
};

#endif
