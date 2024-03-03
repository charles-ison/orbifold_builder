#ifndef GEOMETRYENGINE_H
#define GEOMETRYENGINE_H

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include "Mesh.h"

class GeometryEngine : protected QOpenGLFunctions {
public:
    GeometryEngine();
    virtual ~GeometryEngine();
    void drawMesh(QOpenGLShaderProgram *program);
    void drawLine(QOpenGLShaderProgram *program);
    void drawBoundary(QOpenGLShaderProgram *program);
    void drawPointToDelete(QOpenGLShaderProgram *program);
    void initLine(std::vector<Vertex*> lineVerticesVector, QColor color);
    void initBoundary(std::vector<Vertex*> boundaryVertices1, std::vector<Vertex*> boundaryVertices2, int displaySize1, int displaySize2, bool isBoundary1Loop, bool isBoundary2Loop, bool boundariesAreCombinedLoop, bool boundariesReversed);
    void initMesh(Mesh* mesh);
    void initPointToDelete(std::vector<Vertex*> verticesToDelete);
    void initAnimation(Mesh* mesh);

private:
    std::vector<QVector3D> initBoundaryColors(int numVertices, int displaySize);
    void drawArrows(QOpenGLShaderProgram *program);
    void initArrows(std::vector<QVector3D> positions2, std::vector<QVector3D> colors2);
    std::vector<QVector3D> buildArrow(int numBoundaryVertices, std::vector<QVector3D> positions);

    int numIndices;
    QOpenGLBuffer arrayBuf;
    QOpenGLBuffer indexBuf;
    QOpenGLBuffer colorBuf;

    int numLineVertices;
    QOpenGLBuffer lineColorBuf;
    QOpenGLBuffer lineArrayBuf;

    int numPointsToDeleteVertices;
    QOpenGLBuffer pointToDeleteColorBuf;
    QOpenGLBuffer pointToDeleteArrayBuf;

    int numBoundaryVertices1;
    QOpenGLBuffer boundaryColorBuf1;
    QOpenGLBuffer boundaryArrayBuf1;

    int numBoundaryVertices2;
    QOpenGLBuffer boundaryColorBuf2;
    QOpenGLBuffer boundaryArrayBuf2;

    int numArrowVertices;
    QOpenGLBuffer arrowColorBuf;
    QOpenGLBuffer arrowIndexBuf;
    QOpenGLBuffer arrowArrayBuf;

    static const int animationSpeed = 100;
    int originalIndexToEndAnimation = 0;
    int indexToStartAnimation = 0;
    int indexToEndAnimation = originalIndexToEndAnimation;
    std::vector<QVector3D> animationPositions;
};

#endif
