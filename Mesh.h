#ifndef MESH_H
#define MESH_H

#include <QOpenGLFunctions>
#include "surfaces/VertexData.h"
#include "surfaces/Surface.h"

class Mesh {

public:
    void copySurface(Surface *surface);
    std::vector<GLushort> getIndices();
    std::vector<VertexData*> getVertices();
    void cutVertex(VertexData *vertexToCut);

private:
    std::vector<VertexData*> vertices;
    std::vector<GLushort> indices;
};

#endif
