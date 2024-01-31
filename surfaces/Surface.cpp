#include "Surface.h"

void Surface::initNeighbors() {
    int numTriangles = getNumTriangles();
    Vertex* vertices = getVertices();
    Triangle* triangles = getTriangles();

    for (int i=0; i<numTriangles; i++) {
        int index1 = triangles[i].vertexIndices[0];
        int index2 = triangles[i].vertexIndices[1];
        int index3 = triangles[i].vertexIndices[2];

        vertices[index1].triangles.insert(&triangles[i]);
        vertices[index2].triangles.insert(&triangles[i]);
        vertices[index3].triangles.insert(&triangles[i]);
    }
}
