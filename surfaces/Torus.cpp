#include "Torus.h"

Torus::Torus() {
    float horStepSize = 2 * M_PI / numHorSteps;
    float vertStepSize = 2 * M_PI / numVertSteps;

    int verticesCounter = 0;
    for (int i=0; i<numHorSteps; i++) {
        float horizontalAngle = horStepSize * i;
        for (int j=0; j<numVertSteps; j++) {
            float verticalAngle = vertStepSize * j;
            float x = (bigRadius + smallRadius * cosf(verticalAngle)) * cosf(horizontalAngle);
            float y = (bigRadius + smallRadius * cosf(verticalAngle)) * sinf(horizontalAngle);
            float z = smallRadius * sinf(verticalAngle);
            vertices[verticesCounter] = {QVector3D(x,  y,  z)};
            verticesCounter += 1;
        }
    }

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
}

VertexData* Torus::getVertices() {
    return vertices;
}

GLushort* Torus::getIndices() {
    return indices;
}

int Torus::getNumVertices() {
    return numVertices;
}

int Torus::getNumIndices() {
    return numIndices;
}
