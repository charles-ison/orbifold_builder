#include "Surface.h"

void Surface::initNeighbors() {
    int numTriangles = getNumTriangles();
    Triangle* triangles = getTriangles();

    for (int i=0; i<numTriangles; i++) {
        Vertex* vertex0 = triangles[i].vertices[0];
        Vertex* vertex1 = triangles[i].vertices[1];
        Vertex* vertex2 = triangles[i].vertices[2];

        vertex0->triangles.insert(&triangles[i]);
        vertex1->triangles.insert(&triangles[i]);
        vertex2->triangles.insert(&triangles[i]);
    }
}
