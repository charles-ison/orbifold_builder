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
    void setDrawingColor(QColor newColor);
    void resizeGL(int w, int h) override;
    void toggleShouldDeleteSurface();
    void glue();
    void glueAnimation();

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
    void addDrawnVertices(Vertex *newVertex);
    std::vector<Vertex*> getNewVerticesPath(Vertex *newVertex);
    bool drawnVerticesContainLoop(Vertex *newVertex);
    Vertex* getVertexFromMouseEvent(QMouseEvent *e);
    bool triangleContainsVertex(Vertex *vertex, Triangle *triangle, std::vector<Vertex*> vertices);
    bool rotationDirectionAligns(Triangle* triangle, Vertex* vertex1, Vertex* vertex2, std::vector<Vertex*> vertices);
    int getNewFoldingIndex(int index);
    float calculateDistance(float oldDistance, Vertex* vertex1, Vertex* vertex2);

    Mesh* mesh;
    Cube* cubeSurface;
    Sphere* sphereSurface;
    Torus* torusSurface;
    MobiusStrip* mobiusStripSurface;
    CrossCap* crossCapSurface;
    KleinBottle* kleinBottleSurface;
    QBasicTimer timer;
    QOpenGLShaderProgram program;
    GeometryEngine *geometryEngine = nullptr;
    QMatrix4x4 projection;
    QMatrix4x4 mvp_matrix;
    QVector3D rotationAxis;
    qreal angularSpeed = 0;
    QQuaternion rotation;
    std::vector<Vertex*> drawnVertices;
    std::vector<Vertex*> boundaryVertices1;
    std::vector<Vertex*> boundaryVertices2;
    QColor drawingColor;
    bool isDrawingMode;
    bool shouldPaintGL;
    bool shouldAnimate;
    bool shouldDeleteSurface;
};

#endif
