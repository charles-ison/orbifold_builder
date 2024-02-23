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
#include "surfaces/PlyFile.h"
#include "Mesh.h"

class GeometryEngine;

class ResultsWidget : public QOpenGLWidget, protected QOpenGLFunctions {

    Q_OBJECT

public:
    using QOpenGLWidget::QOpenGLWidget;
    ~ResultsWidget();
    enum Surface {sphere, cube, torus, mobiusStrip, crossCap, kleinBottle, plyFile};
    enum SmoothingAmount {constrained, unconstrained};
    enum DrawingMode {click, drag};
    void addSurface(Surface newSurface);
    void cutSurface();
    void setDrawingColor(QColor newColor);
    void resizeGL(int w, int h) override;
    void toggleShouldDeleteSurface();
    void glue();
    void smooth(SmoothingAmount smoothingAmount);
    void reverseBoundaries();
    void glueAnimation();
    void setDrawingMode(DrawingMode newDrawingMode);

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
    std::tuple<float, std::vector<Vertex*>> getVerticesPathAndDistance(Vertex *startVertex, Vertex *endVertex);
    bool drawnVerticesContainLoop(Vertex *newVertex);
    Vertex* getVertexFromMouseEvent(QMouseEvent *e);
    bool triangleContainsVertex(Vertex *vertex, Triangle *triangle, std::vector<Vertex*> vertices);
    bool rotationDirectionAligns(Triangle* triangle, Vertex* vertex1, Vertex* vertex2, std::vector<Vertex*> vertices);
    float euclideanDistance(Vertex* vertex1, Vertex* vertex2);
    std::vector<Vertex*> findVerticesToSmooth();
    void glueCrossCap();
    void glueTraditional();

    Mesh* mesh;
    Cube* cubeSurface;
    Sphere* sphereSurface;
    Torus* torusSurface;
    MobiusStrip* mobiusStripSurface;
    CrossCap* crossCapSurface;
    KleinBottle* kleinBottleSurface;
    PlyFile* plyFileSurface;
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
    std::vector<Vertex*> oldBoundaries;
    QColor drawingColor;
    DrawingMode drawingMode;
    bool isDrawingEnabled;
    bool shouldPaintGL;
    bool shouldAnimate;
    bool shouldDeleteSurface;
    bool boundariesReversed;
    bool isBoundary1Loop;
    bool isBoundary2Loop;
    bool boundariesAreCombinedLoop;
    int numOpenings;
    int boundary1DisplaySize;
    int boundary2DisplaySize;
    float xyThreshold;
    const float xyThresholdClick = 0.05;
    const float xyThresholdDrag = 0.015;
};

#endif
