#include "Mesh.h"

void Mesh::resetSurface(Surface *surface) {
    vertices.clear();
    triangles.clear();
    addSurface(surface);
}

void Mesh::addSurface(Surface *surface) {
    int numSurfaceVertices = surface->getNumVertices();
    Vertex* surfaceVertices = surface->getVertices();
    for (int i=0; i<numSurfaceVertices; i++) {
        vertices.push_back(&surfaceVertices[i]);
    }

    int numSurfaceTriangles = surface->getNumTriangles();
    Triangle* surfaceTriangles = surface->getTriangles();
    for (int i=0; i<numSurfaceTriangles; i++) {
        triangles.push_back(&surfaceTriangles[i]);
    }

    updateVertexNeighbors();
    updateTriangles();
}

void Mesh::updateVertexNeighbors() {
    for (int i=0; i<triangles.size(); i++) {
        Vertex* vertex0 = triangles[i]->vertices[0];
        Vertex* vertex1 = triangles[i]->vertices[1];
        Vertex* vertex2 = triangles[i]->vertices[2];

        vertex0->triangles.insert(triangles[i]);
        vertex1->triangles.insert(triangles[i]);
        vertex2->triangles.insert(triangles[i]);
    }
}

std::vector<double> Mesh::computeAngles(Vertex* vertex0, Vertex* vertex1, Vertex* vertex2) {
    double edge01 = euclideanDistance(vertex0, vertex1);
    double edge12 = euclideanDistance(vertex1, vertex2);
    double edge20 = euclideanDistance(vertex2, vertex0);
    double angle0 = acos((pow(edge01, 2) + pow(edge20, 2) - pow(edge12, 2)) / (2 * edge01 * edge20));
    double angle1 = acos((pow(edge01, 2) + pow(edge12, 2) - pow(edge20, 2)) / (2 * edge01 * edge12));
    double angle2 = acos((pow(edge12, 2) + pow(edge20, 2) - pow(edge01, 2)) / (2 * edge12 * edge20));
    return {angle0, angle1, angle2};
}

void Mesh::updateTriangles() {
    for (int i=0; i<triangles.size(); i++) {
        Vertex* vertex0 = triangles[i]->vertices[0];
        Vertex* vertex1 = triangles[i]->vertices[1];
        Vertex* vertex2 = triangles[i]->vertices[2];

        QVector3D vector1 = vertex1->position - vertex0->position;
        QVector3D vector2 = vertex2->position - vertex1->position;
        QVector3D faceNormal = QVector3D::crossProduct(vector1, vector2).normalized();
        triangles[i]->normal = faceNormal;
        triangles[i]->angles = computeAngles(vertex0, vertex1, vertex2);
    }

    for (int i=0; i<vertices.size(); i++) {
        for (Triangle* triangle : vertices[i]->triangles) {
            vertices[i]->normal += triangle->normal;
        }
        vertices[i]->normal = vertices[i]->normal.normalized();
    }
}

std::vector<Vertex*> Mesh::getVertices() {
    return vertices;
}

std::vector<Triangle*> Mesh::getTriangles() {
    return triangles;
}

void Mesh::addVertex(Vertex *vertexToAdd) {
    vertices.push_back(vertexToAdd);
}

void Mesh::deleteTriangleReferences(Triangle* triangleToDelete) {
    for (Vertex* vertex : triangleToDelete->vertices) {
        std::set<Triangle *> newTriangles;
        for (Triangle *vertexTriangle: vertex->triangles) {
            if (vertexTriangle != triangleToDelete) {
                newTriangles.insert(vertexTriangle);
            }
        }
        vertex->triangles = newTriangles;
    }
}

void Mesh::deleteVerticesReferences(std::unordered_set<Vertex*> verticesToDelete) {
    std::vector<Triangle*> newTriangles;
    for (Triangle* triangle : triangles) {
        Vertex* vertex1 = triangle->vertices[0];
        Vertex* vertex2 = triangle->vertices[1];
        Vertex* vertex3 = triangle->vertices[2];

        if (verticesToDelete.find(vertex1) != verticesToDelete.end() || verticesToDelete.find(vertex2) != verticesToDelete.end() || verticesToDelete.find(vertex3) != verticesToDelete.end()) {
            deleteTriangleReferences(triangle);
        } else {
            newTriangles.push_back(triangle);
        }
    }
    triangles = newTriangles;
}

void Mesh::deleteVertices(std::unordered_set<Vertex*> verticesToDelete) {
    int delta = 0;
    auto itr = vertices.begin();
    while(itr != vertices.end()) {
        Vertex* vertex = *itr;
        if(verticesToDelete.find(vertex) != verticesToDelete.end()) {
            itr = vertices.erase(itr);
            delta += 1;
        } else {
            vertex->index -= delta;
            itr ++;
        }
    }
}

void Mesh::deleteOutdatedTriangles() {
    std::vector<Triangle*> newTriangles;
    for (Triangle* triangle : triangles) {
        Vertex* vertex1 = triangle->vertices[0];
        Vertex* vertex2 = triangle->vertices[1];
        Vertex* vertex3 = triangle->vertices[2];
        std::set<Vertex*> triangleVertices = {vertex1, vertex2, vertex3};

        if (triangleVertices.size() != 3) {
            deleteTriangleReferences(triangle);
        } else {
            newTriangles.push_back(triangle);
        }
    }
    triangles = newTriangles;
}
