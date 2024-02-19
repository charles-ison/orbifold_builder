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
    void initLine(std::vector<Vertex*> lineVerticesVector, QColor color);
    void initBoundary(std::vector<Vertex*> boundaryVerticesVector1, std::vector<Vertex*> boundaryVerticesVector2, bool boundariesReversed);
    void initMesh(Mesh* mesh);
    void initAnimation(Mesh* mesh);

private:
    std::vector<QVector3D> initBoundaryColorForward(int size);
    std::vector<QVector3D> initBoundaryColorsBackward(int size);

    int numIndices;
    QOpenGLBuffer arrayBuf;
    QOpenGLBuffer indexBuf;
    QOpenGLBuffer colorBuf;

    int numLineVertices;
    QOpenGLBuffer lineColorBuf;
    QOpenGLBuffer lineArrayBuf;

    int numBoundaryVertices1;
    QOpenGLBuffer boundaryColorBuf1;
    QOpenGLBuffer boundaryArrayBuf1;

    int numBoundaryVertices2;
    QOpenGLBuffer boundaryColorBuf2;
    QOpenGLBuffer boundaryArrayBuf2;

    static const int animationSpeed = 100;
    int originalIndexToEndAnimation = 0;
    int indexToStartAnimation = 0;
    int indexToEndAnimation = originalIndexToEndAnimation;
    std::vector<QVector3D> animationPositions;
};

#endif
