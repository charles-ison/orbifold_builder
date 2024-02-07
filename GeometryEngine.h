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
    void initLine(std::vector<Vertex*> lineVerticesVector);
    void initMesh(Mesh* mesh);
    void initAnimation(Mesh* mesh);

private:
    int numIndices;
    QOpenGLBuffer arrayBuf;
    QOpenGLBuffer indexBuf;

    int numLineVertices;
    QOpenGLBuffer lineArrayBuf;

    static const int animationSpeed = 100;
    int indexToStartAnimation = 0;
    std::vector<Vertex> animationVertices;
};

#endif
