#ifndef GEOMETRYENGINE_H
#define GEOMETRYENGINE_H

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include "surfaces/Surface.h"

class GeometryEngine : protected QOpenGLFunctions {
public:
    GeometryEngine(Surface* surface);
    virtual ~GeometryEngine();
    void drawGeometry(QOpenGLShaderProgram *program);

private:
    void initGeometry(Surface* surface);

    int numIndices;
    Surface* surface;
    QOpenGLBuffer arrayBuf;
    QOpenGLBuffer indexBuf;
};

#endif
