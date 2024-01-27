#ifndef TEST_SURFACE_H
#define TEST_SURFACE_H

#include <QOpenGLFunctions>
#include "VertexData.h"
#include "Surface.h"

class TestSurface {

public:
    void copySurface(Surface *surface);
    std::vector<GLushort> getIndices();
    std::vector<VertexData*> getVertices();
    int getNumVertices();
    int getNumIndices();
    void cutVertex(VertexData *vertexToCut);

private:
    std::vector<VertexData*> vertices;
    std::vector<GLushort> indices;
};

#endif
