#include "Sphere.h"

Sphere::Sphere() {
    initVertices();
    initIndices();
}

void Sphere::initVertices() {
    float vertStepSize = M_PI / numVertSteps;
    float horStepSize = 2 * M_PI / numHorSteps;

    int verticesCounter = 0;
    for (int i=0; i<=numVertSteps; i++) {
        float verticalAngle = (M_PI / 2) - vertStepSize * i;
        float z = radius * sinf(verticalAngle);
        int tempNumHorSteps = (i == 0 || i == numVertSteps) ? 1 : numHorSteps;
        for (int j=0; j<tempNumHorSteps; j++) {
            float horizontalAngle = horStepSize * j;
            float x = radius * cosf(verticalAngle) * cosf(horizontalAngle);
            float y = radius * cosf(verticalAngle) * sinf(horizontalAngle);
            vertices[verticesCounter] = {QVector3D(x,  y,  z)};
            verticesCounter += 1;
        }
    }
}

void Sphere::initIndices() {
    int indexCounter = 0;
    int faceCounter = 0;
    for (int i=0; i<numVertSteps; i++) {
        for (int j=0; j<numHorSteps; j++) {
            if (i == 0) {
                indices[indexCounter] = i;
                indices[indexCounter + 1] = faceCounter + 1;
                if (j == numHorSteps-1) {
                    indices[indexCounter + 2] = 1;
                    faceCounter = 0;
                } else {
                    indices[indexCounter + 2] = faceCounter + 2;
                }
                indexCounter += 3;
            }
            else if (i == numVertSteps - 1) {
                indices[indexCounter] = numVertices - 1;
                indices[indexCounter + 1] = faceCounter;
                if (j == numHorSteps-1) {
                    indices[indexCounter + 2] = faceCounter + 1 - numHorSteps ;
                } else {
                    indices[indexCounter + 2] = faceCounter + 1;
                }
                indexCounter += 3;
            }
            else if (j == numHorSteps-1) {
                indices[indexCounter] = faceCounter + 1 - numHorSteps;
                indices[indexCounter + 1] = faceCounter;
                indices[indexCounter + 2] = faceCounter + 1;
                indices[indexCounter + 3] = faceCounter + numHorSteps;
                indices[indexCounter + 4] = faceCounter + 1;
                indices[indexCounter + 5] = faceCounter;
                indexCounter += 6;
            }
            else {
                indices[indexCounter] = faceCounter + 1;
                indices[indexCounter + 1] = faceCounter;
                indices[indexCounter + 2] = faceCounter + 1 + numHorSteps;
                indices[indexCounter + 3] = faceCounter + numHorSteps;
                indices[indexCounter + 4] = faceCounter + 1 + numHorSteps;
                indices[indexCounter + 5] = faceCounter;
                indexCounter += 6;
            }
            faceCounter += 1;
        }
    }
}

VertexData* Sphere::getVertices() {
    return vertices;
}

GLushort* Sphere::getIndices() {
    return indices;
}

int Sphere::getNumVertices() {
    return numVertices;
}

int Sphere::getNumIndices() {
    return numIndices;
}
