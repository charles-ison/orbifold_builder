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
    void drawLine(QOpenGLShaderProgram *program, QColor color);
    void drawBoundary(QOpenGLShaderProgram *program);
    void initLine(std::vector<Vertex*> lineVerticesVector);
    void initBoundary(std::vector<Vertex*> boundaryVerticesVector1, std::vector<Vertex*> boundaryVerticesVector2);
    void initMesh(Mesh* mesh);
    void initAnimation(Mesh* mesh);

private:
    int numIndices;
    QOpenGLBuffer arrayBuf;
    QOpenGLBuffer indexBuf;

    int numLineVertices;
    QOpenGLBuffer lineArrayBuf;

    int numBoundaryVertices;
    QOpenGLBuffer boundaryArrayBuf;
    QOpenGLBuffer boundaryIndexBuf;

    static const int animationSpeed = 100;
    int originalIndexToEndAnimation = 0;
    int indexToStartAnimation = 0;
    int indexToEndAnimation = originalIndexToEndAnimation;
    std::vector<QVector3D> animationPositions;
};

#endif
