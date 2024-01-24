#ifndef VERTEXDATA_H
#define VERTEXDATA_H

#include <QOpenGLShaderProgram>
#include <unordered_set>

struct VertexData {
    QVector3D position;
    std::unordered_set<VertexData*> neighbors;
};

#endif
