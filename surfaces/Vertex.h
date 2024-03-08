#ifndef VERTEX_H
#define VERTEX_H

#include <QOpenGLShaderProgram>
#include <set>
#include "Triangle.h"

struct Vertex {
    QVector3D position;
    std::set<Triangle*> triangles;
    int index;
    QVector3D normal;
};

#endif
