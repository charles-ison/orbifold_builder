#include "Sphere.h"

VertexData* Sphere::getVertices() {
    VertexData vertices[numVertices];

    float vertStepSize = M_PI / numVertSteps;
    float horStepSize = 2 * M_PI / numHorSteps;

    int verticesCounter = 0;
    for (int i=0; i<=numVertSteps; i++) {
        float verticalAngle = (M_PI / 2) - vertStepSize * i;
        float z = radius * sinf(verticalAngle);
        for (int j=0; j<numHorSteps; j++) {
            float horizontalAngle = horStepSize * j;
            float x = radius * cosf(verticalAngle) * cosf(horizontalAngle);
            float y = radius * cosf(verticalAngle) * sinf(horizontalAngle);
            vertices[verticesCounter] = {QVector3D(x,  y,  z)};
            verticesCounter += 1;
        }
    }

    return vertices;
}

GLushort* Sphere::getIndices() {
    GLushort indices[numIndices];
    int indexCounter = 0;
    int quadCounter = 0;
    for (int i=0; i<numVertSteps; i++) {
        for (int j=0; j<numHorSteps; j++) {
            if (j != numHorSteps-1) {
                indices[indexCounter] = quadCounter + 1;
                indices[indexCounter + 1] = quadCounter;
                indices[indexCounter + 2] = quadCounter + 1 + numHorSteps;
                indices[indexCounter + 3] = quadCounter + numHorSteps;
                indices[indexCounter + 4] = quadCounter + 1 + numHorSteps;
                indices[indexCounter + 5] = quadCounter;
            } else {
                indices[indexCounter] = quadCounter + 1 - numHorSteps;
                indices[indexCounter + 1] = quadCounter;
                indices[indexCounter + 2] = quadCounter + 1;
                indices[indexCounter + 3] = quadCounter + numHorSteps;
                indices[indexCounter + 4] = quadCounter + 1;
                indices[indexCounter + 5] = quadCounter;
            }
            indexCounter += 6;
            quadCounter += 1;
        }
    }
    return indices;
}

int Sphere::getNumVertices() {
    return numVertices;
}

int Sphere::getNumIndices() {
    return numIndices;
}
