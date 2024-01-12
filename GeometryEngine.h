#ifndef GEOMETRYENGINE_H
#define GEOMETRYENGINE_H

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include "surfaces/Cube.h"
#include "surfaces/Sphere.h"
#include "surfaces/Surface.h"

class GeometryEngine : protected QOpenGLFunctions {
public:
    GeometryEngine();
    virtual ~GeometryEngine();

    void drawCubeGeometry(QOpenGLShaderProgram *program);

private:
    int numIndices;
    void initGeometry(Surface& surface);

    Cube* cube;
    Sphere* sphere;
    QOpenGLBuffer arrayBuf;
    QOpenGLBuffer indexBuf;
};

#endif
