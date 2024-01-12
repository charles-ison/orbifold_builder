#include "KleinBottle.h"

KleinBottle::KleinBottle() {
    float horStepSize = M_PI / numHorSteps;
    float vertStepSize = 2 * M_PI / numVertSteps;

    int verticesCounter = 0;
    for (int i=0; i<=numHorSteps; i++) {
        float horizontalAngle = horStepSize * i;
        for (int j=0; j<=numVertSteps; j++) {
            float verticalAngle = vertStepSize * j;
            float r = c * (1 - (cosf(horizontalAngle) / 2));
            float x = (a * (1 + sinf(horizontalAngle)) + r * cosf(verticalAngle)) * cosf(horizontalAngle);
            float y = (b + r * cosf(verticalAngle)) * sinf(horizontalAngle);
            float z = r * sinf(verticalAngle);
            vertices[verticesCounter] = {QVector3D(x,  y,  z)};
            verticesCounter += 1;
        }
    }

    for (int i=0; i<=numHorSteps; i++) {
        float horizontalAngle = M_PI + horStepSize * i;
        for (int j=0; j<=numVertSteps; j++) {
            float verticalAngle = vertStepSize * j;
            float r = c * (1 - (cosf(horizontalAngle) / 2));
            float x = a * (1 + sinf(horizontalAngle)) * cosf(horizontalAngle) - r * cosf(verticalAngle);
            float y = b * sinf(horizontalAngle);
            float z = r * sinf(verticalAngle);
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

    for (int i=0; i<=numHorSteps; i++) {
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

VertexData* KleinBottle::getVertices() {
    return vertices;
}

GLushort* KleinBottle::getIndices() {
    return indices;
}

int KleinBottle::getNumVertices() {
    return numVertices;
}

int KleinBottle::getNumIndices() {
    return numIndices;
}
