#include "Surface.h"

void Surface::initNeighborsAndNormals() {
    int numTriangles = getNumTriangles();
    int numVertices = getNumVertices();
    Triangle* triangles = getTriangles();
    Vertex* vertices = getVertices();

    for (int i=0; i<numTriangles; i++) {
        Vertex* vertex0 = triangles[i].vertices[0];
        Vertex* vertex1 = triangles[i].vertices[1];
        Vertex* vertex2 = triangles[i].vertices[2];

        vertex0->triangles.insert(&triangles[i]);
        vertex1->triangles.insert(&triangles[i]);
        vertex2->triangles.insert(&triangles[i]);

        QVector3D vector1 = vertex1->position - vertex0->position;
        QVector3D vector2 = vertex2->position - vertex1->position;
        QVector3D faceNormal = QVector3D::crossProduct(vector1, vector2).normalized();
        triangles[i].normal = faceNormal;
    }

    for (int i=0; i<numVertices; i++) {
        for (Triangle* triangle : vertices[i].triangles) {
            vertices[i].normal += triangle->normal;
        }
        vertices[i].normal = vertices[i].normal.normalized();
    }
}
