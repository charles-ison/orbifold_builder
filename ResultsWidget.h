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
    enum DrawingMode {drag, click};
    void addSurface(Surface newSurface);
    void cutSurface();
    void setDrawingColor(QColor newColor);
    void resizeGL(int w, int h) override;
    void toggleDeleteSurface();
    void toggleSmoothSurface();
    void glue();
    void reset();
    void reverseBoundaries();
    void toggleGenerators();
    void setDrawingMode(DrawingMode newDrawingMode);
    std::string getResultsAttributesLabelText();

protected:
    void mousePressEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void wheelEvent(QWheelEvent *e) override;
    void timerEvent(QTimerEvent *e) override;
    void initializeGL() override;
    void paintGL() override;
    void initShaders();

private:
    void smooth();
    void deleteSurface(Vertex *vertexToDelete);
    void addDrawnVertices(Vertex *newVertex);
    bool drawnVerticesContainLoop(Vertex *newVertex);
    Vertex* getVertexFromMouseEvent(QMouseEvent *e);
    void connectVertices();
    bool rotationDirectionAligns(Triangle* triangle, Vertex* vertex1, Vertex* vertex2);
    void findVerticesToSmooth(Vertex *vertexToSmooth);
    void setSelectedPoints(Vertex *vertex);
    std::vector<Vertex*> connectFirstAndLastVertices(std::vector<Vertex*> boundary);
    std::vector<Vertex*> connectMiddleVertices(std::vector<Vertex*> boundary);
    std::vector<Vertex*> findSecondGenerator(std::unordered_map<Vertex*, Triangle*> orientationBoundary);

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
    QMatrix4x4 mvpMatrix;
    QVector3D rotationAxis;
    qreal angularSpeed = 0;
    QQuaternion rotation;
    std::vector<Vertex*> generatorVertices;
    std::vector<Vertex*> drawnVertices;
    std::vector<Vertex*> boundaryVertices1;
    std::vector<Vertex*> boundaryVertices2;
    std::vector<Vertex*> selectedPoints;
    std::vector<Vertex*> verticesToSmooth;
    std::map<Vertex*, int> verticesToSmoothMap;

    const float xyThreshold = 0.03;
    const QVector3D cameraPosition = QVector3D(0.0, 0.0, -5.0);

    QColor drawingColor;
    DrawingMode drawingMode;
    bool showGenerators;
    bool isDrawingEnabled;
    bool shouldPaintGL;
    bool findVertexToDelete;
    bool findVertexToSmooth;
    bool boundariesReversed;
    bool boundariesOverlapping;
    bool isBoundary1Loop;
    bool isBoundary2Loop;
    bool boundariesAreCombinedLoop;
    int numOpenings;
    int boundary1DisplaySize;
    int boundary2DisplaySize;
    int numSmoothingSteps;
    int numSmoothingStepsSoFar;
};

#endif