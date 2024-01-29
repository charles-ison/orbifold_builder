#include "CrossCap.h"
#include <cmath>

CrossCap::CrossCap() {
    initVertices();
    initIndices();
    initTriangles();
    initNeighbors();
}

void CrossCap::initVertices() {
    float horStepSize = 2 * M_PI / numHorSteps;
    float vertStepSize = M_PI / (2 * numVertSteps);

    int verticesCounter = 0;
    for (int i=0; i<numHorSteps; i++) {
        float horizontalAngle = horStepSize * i;
        for (int j=0; j<=numVertSteps; j++) {
            float verticalAngle = vertStepSize * j;
            float x = radius * cosf(horizontalAngle) * sinf(2 * verticalAngle);
            float y = radius * sinf(horizontalAngle) * sinf(2 * verticalAngle);
            float z = radius * (pow(cosf(verticalAngle), 2) - pow(cosf(horizontalAngle), 2) * pow(sinf(verticalAngle), 2));

            vertices[verticesCounter] = {QVector3D(x,  y,  z)};
            verticesCounter += 1;
        }
    }
}

void CrossCap::initIndices() {
    int indexCounter = 0;
    int faceCounter = 0;
    for (int i=0; i<numHorSteps; i++) {
        for (int j=0; j<=numVertSteps; j++) {
            indices[indexCounter] = (faceCounter + 1) % numVertices;
            indices[indexCounter + 1] = faceCounter % numVertices;
            indices[indexCounter + 2] = (faceCounter + 1 + numVertSteps) % numVertices;
            indices[indexCounter + 3] = (faceCounter + numVertSteps) % numVertices;
            indices[indexCounter + 4] = (faceCounter + 1 + numVertSteps) % numVertices;
            indices[indexCounter + 5] = faceCounter % numVertices;
            indexCounter += 6;
            faceCounter += 1;
        }
    }
}

void CrossCap::initTriangles() {
    int faceCounter = 0;
    for (int i=0; i<numHorSteps; i++) {
        for (int j=0; j<=numVertSteps; j++) {
            int index1 = (faceCounter + 1) % numVertices;
            int index2 = faceCounter % numVertices;
            int index3 = (faceCounter + 1 + numVertSteps) % numVertices;
            triangles.push_back({{index1, index2, index3}});

            int index4 = (faceCounter + numVertSteps) % numVertices;
            int index5 = (faceCounter + 1 + numVertSteps) % numVertices;
            int index6 = faceCounter % numVertices;
            triangles.push_back({{index4, index5, index6}});

            faceCounter += 1;
        }
    }
}

std::vector<Triangle> CrossCap::getTriangles() {
    return triangles;
}

Vertex* CrossCap::getVertices() {
    return vertices;
}

GLushort* CrossCap::getIndices() {
    return indices;
}

int CrossCap::getNumVertices() {
    return numVertices;
}

int CrossCap::getNumIndices() {
    return numIndices;
}
