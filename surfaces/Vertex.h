#ifndef VERTEX_H
#define VERTEX_H

#include <QOpenGLShaderProgram>
#include <set>

struct Vertex {
    QVector3D position;
    std::set<Vertex*> neighbors;

    std::string toString(){
        return "x: " + std::to_string(position.x()) + ", y: " + std::to_string(position.y()) + ", z: " + std::to_string(position.z());
    }
};

#endif
