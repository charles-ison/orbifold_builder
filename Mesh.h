#ifndef MESH_H
#define MESH_H

#include <QOpenGLFunctions>
#include "surfaces/Vertex.h"
#include "surfaces/Surface.h"

class Mesh {

public:
    void copySurface(Surface *surface);
    std::vector<GLushort> getIndices();
    std::vector<Vertex*> getVertices();
    void cutVertex(Vertex *vertexToCut);

private:
    int getUpdatedIndex(int deletedIndex, int index);
    std::vector<Vertex*> vertices;
    std::vector<GLushort> indices;
};

#endif
