#ifndef GEOMETRYENGINE_H
#define GEOMETRYENGINE_H

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include "surfaces/Surface.h"

class GeometryEngine : protected QOpenGLFunctions {
public:
    GeometryEngine();
    virtual ~GeometryEngine();
    void drawSurface(QOpenGLShaderProgram *program);
    void drawLine(QOpenGLShaderProgram *program, QColor color);
    void initLine(std::vector<VertexData*> lineVerticesVector);
    void initSurface(Surface* surface);

private:
    int numIndices;
    QOpenGLBuffer arrayBuf;
    QOpenGLBuffer indexBuf;

    int numLineVertices;
    QOpenGLBuffer lineArrayBuf;
};

#endif
