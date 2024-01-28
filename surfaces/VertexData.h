#ifndef VERTEXDATA_H
#define VERTEXDATA_H

#include <QOpenGLShaderProgram>
#include <set>

struct VertexData {
    QVector3D position;
    std::set<VertexData*> neighbors;
};

#endif
