#include "Surface.h"

void Surface::initNeighbors() {
    Vertex* vertices = getVertices();
    GLushort* indices = getIndices();
    int numIndices = getNumIndices();

    for (int i=0; i<numIndices; i+=3) {
        GLushort index1 = indices[i];
        GLushort index2 = indices[i+1];
        GLushort index3 = indices[i+2];

        vertices[index1].neighbors.insert(&vertices[index2]);
        vertices[index1].neighbors.insert(&vertices[index3]);

        vertices[index2].neighbors.insert(&vertices[index1]);
        vertices[index2].neighbors.insert(&vertices[index3]);

        vertices[index3].neighbors.insert(&vertices[index1]);
        vertices[index3].neighbors.insert(&vertices[index2]);
    }
}
