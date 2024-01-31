#ifndef VERTEX_H
#define VERTEX_H

#include <QOpenGLShaderProgram>
#include <set>
#include "Triangle.h"

struct Vertex {
    QVector3D position;
    std::set<Vertex*> neighbors;
    std::set<Triangle*> triangles;

    std::string toString(){
        return "x: " + std::to_string(position.x()) + ", y: " + std::to_string(position.y()) + ", z: " + std::to_string(position.z());
    }
};

#endif
