#ifndef VERTEX_H
#define VERTEX_H

#include <QOpenGLShaderProgram>
#include <set>
#include "Triangle.h"

struct Vertex {
    QVector3D position;
    std::set<Triangle*> triangles;
    int index;

    std::string toString(){
        return "x: " + std::to_string(position.x()) + ", y: " + std::to_string(position.y()) + ", z: " + std::to_string(position.z());
    }
};

#endif
