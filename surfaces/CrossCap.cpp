#include "CrossCap.h"
#include<cmath>

CrossCap::CrossCap() {
    float horStepSize = 2 * M_PI / numHorSteps;
    float vertStepSize = 2 * M_PI / numVertSteps;

    int verticesCounter = 0;
    for (int i=0; i<=numHorSteps; i++) {
        float horizontalAngle = horStepSize * i;
        for (int j=0; j<=numVertSteps; j++) {
            float verticalAngle = vertStepSize * j;
            float x = radius * sinf(2 * horizontalAngle) * cosf(verticalAngle);
            float y = radius * sinf(2 * horizontalAngle) * sinf(verticalAngle);
            float z = radius * (pow(sinf(horizontalAngle), 2) - pow(cosf(horizontalAngle), 2) * pow(cosf(verticalAngle), 2));
            vertices[verticesCounter] = {QVector3D(x,  y,  z)};
            verticesCounter += 1;
        }
    }

    int indexCounter = 0;
    int quadCounter = 0;
    for (int i=0; i<numHorSteps; i++) {
        for (int j=0; j<=numVertSteps; j++) {
            indices[indexCounter] = quadCounter + 1;
            indices[indexCounter + 1] = quadCounter;
            indices[indexCounter + 2] = quadCounter + 1 + numVertSteps;
            indices[indexCounter + 3] = quadCounter + numVertSteps;
            indices[indexCounter + 4] = quadCounter + 1 + numVertSteps;
            indices[indexCounter + 5] = quadCounter;
            indexCounter += 6;
            quadCounter += 1;
        }
    }
}

VertexData* CrossCap::getVertices() {
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
