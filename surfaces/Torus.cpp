#include "Torus.h"

Torus::Torus(QVector3D centerPosition, QVector3D scale) {
    initVertices(centerPosition, scale);
    initTriangles();
    initNeighbors();
}

void Torus::initVertices(QVector3D centerPosition, QVector3D scale) {
    float horStepSize = 2 * M_PI / numHorSteps;
    float vertStepSize = 2 * M_PI / numVertSteps;

    float bigRadius = scale.x();
    float smallRadius = scale.y();

    for (int i=0; i<numHorSteps; i++) {
        float horizontalAngle = horStepSize * i;
        for (int j=0; j<numVertSteps; j++) {
            float verticalAngle = vertStepSize * j;
            float x = (bigRadius + smallRadius * cosf(verticalAngle)) * cosf(horizontalAngle);
            float y = (bigRadius + smallRadius * cosf(verticalAngle)) * sinf(horizontalAngle);
            float z = smallRadius * sinf(verticalAngle);
            vertices.push_back({.index = (int) vertices.size(), .position = QVector3D(x,  y,  z)});
        }
    }
}

void Torus::initTriangles() {
    int faceCounter = 0;
    for (int i=0; i<numHorSteps; i++) {
        for (int j=0; j<numVertSteps; j++) {
            if (j == numVertSteps - 1) {
                int index1 = (faceCounter + 1 - numHorSteps) % numVertices;
                int index2 = faceCounter % numVertices;
                int index3 = (faceCounter + 1) % numVertices;
                triangles.push_back({{&vertices[index1], &vertices[index2], &vertices[index3]}});

                int index4 = (faceCounter + numHorSteps) % numVertices;
                int index5 = (faceCounter + 1) % numVertices;
                int index6 = faceCounter % numVertices;
                triangles.push_back({{&vertices[index4], &vertices[index5], &vertices[index6]}});
            }
            else {
                int index1 = (faceCounter + 1) % numVertices;
                int index2 = faceCounter % numVertices;
                int index3 = (faceCounter + 1 + numVertSteps) % numVertices;
                triangles.push_back({{&vertices[index1], &vertices[index2], &vertices[index3]}});

                int index4 = (faceCounter + numVertSteps) % numVertices;
                int index5 = (faceCounter + 1 + numVertSteps) % numVertices;
                int index6 = faceCounter % numVertices;
                triangles.push_back({{&vertices[index4], &vertices[index5], &vertices[index6]}});
            }
            faceCounter += 1;
        }
    }
}

Triangle* Torus::getTriangles() {
    return &triangles[0];
}

Vertex* Torus::getVertices() {
    return &vertices[0];
}

int Torus::getNumVertices() {
    return vertices.size();
}

int Torus::getNumTriangles() {
    return triangles.size();
}
