#ifndef VERTEXDATA_H
#define VERTEXDATA_H

#include <QOpenGLShaderProgram>
#include <set>

struct VertexData {
    QVector3D position;
    std::set<VertexData*> neighbors;

    std::string toString(){
        return "x: " + std::to_string(position.x()) + ", y: " + std::to_string(position.y()) + ", z: " + std::to_string(position.z());
    }
};

#endif
