#include "KleinBottle.h"

KleinBottle::KleinBottle() {
    initVertices();
    //initIndices();
    initTriangles();
    initNeighbors();
}

void KleinBottle::initVertices() {
    float horStepSize = M_PI / numHorSteps;
    float vertStepSize = 2 * M_PI / numVertSteps;

    int verticesCounter = 0;
    for (int i=0; i<numHorSteps; i++) {
        float horizontalAngle = horStepSize * i;
        for (int j=0; j<numVertSteps; j++) {
            float verticalAngle = vertStepSize * j;
            float r = c * (1 - (cosf(horizontalAngle) / 2));
            float x = (a * (1 + sinf(horizontalAngle)) + r * cosf(verticalAngle)) * cosf(horizontalAngle);
            float y = (b + r * cosf(verticalAngle)) * sinf(horizontalAngle);
            float z = r * sinf(verticalAngle);
            vertices[verticesCounter] = {QVector3D(x,  y,  z)};
            verticesCounter += 1;
        }
    }

    for (int i=0; i<numHorSteps; i++) {
        float horizontalAngle = M_PI + horStepSize * i;
        for (int j=0; j<numVertSteps; j++) {
            float verticalAngle = vertStepSize * j;
            float r = c * (1 - (cosf(horizontalAngle) / 2));
            float x = a * (1 + sinf(horizontalAngle)) * cosf(horizontalAngle) - r * cosf(verticalAngle);
            float y = b * sinf(horizontalAngle);
            float z = r * sinf(verticalAngle);
            vertices[verticesCounter] = {QVector3D(x,  y,  z)};
            verticesCounter += 1;
        }
    }
}

void KleinBottle::initIndices() {
    int indexCounter = 0;
    int faceCounter = 0;
    for (int i=0; i<2*numHorSteps; i++) {
        for (int j=0; j<numVertSteps; j++) {
            if (i == 2 * numHorSteps - 1 and j == numVertSteps - 1) {
                indices[indexCounter] = (faceCounter + 1 - numHorSteps) % numVertices;
                indices[indexCounter + 1] = faceCounter % numVertices;
                int halfRotation = floor(numVertSteps / 2);
                indices[indexCounter + 2] = floorMod((faceCounter + halfRotation + numVertSteps) % numVertices - 2 * j, numVertSteps);
                indices[indexCounter + 3] = floorMod((faceCounter + halfRotation + numVertSteps) % numVertices - 2 * j, numVertSteps);
                indices[indexCounter + 4] = floorMod((faceCounter + halfRotation + 1 + numVertSteps) % numVertices - 2 * j, numVertSteps);
                indices[indexCounter + 5] = faceCounter % numVertices;
            }
            else if (i == 2 * numHorSteps - 1) {
                indices[indexCounter] = (faceCounter + 1) % numVertices;
                indices[indexCounter + 1] = faceCounter % numVertices;
                int halfRotation = floor(numVertSteps / 2);
                indices[indexCounter + 2] = floorMod((faceCounter + halfRotation + numVertSteps) % numVertices - 2 * j, numVertSteps);
                indices[indexCounter + 3] = floorMod((faceCounter + halfRotation + numVertSteps) % numVertices - 2 * j, numVertSteps);
                indices[indexCounter + 4] = floorMod((faceCounter + halfRotation + 1 + numVertSteps) % numVertices - 2 * j, numVertSteps);
                indices[indexCounter + 5] = faceCounter % numVertices;
            }
            else if (j == numVertSteps - 1) {
                indices[indexCounter] = faceCounter + 1 - numHorSteps;
                indices[indexCounter + 1] = faceCounter;
                indices[indexCounter + 2] = faceCounter + 1;
                indices[indexCounter + 3] = faceCounter + numHorSteps;
                indices[indexCounter + 4] = faceCounter + 1;
                indices[indexCounter + 5] = faceCounter;
            }
            else {
                indices[indexCounter] = faceCounter + 1;
                indices[indexCounter + 1] = faceCounter;
                indices[indexCounter + 2] = faceCounter + 1 + numVertSteps;
                indices[indexCounter + 3] = faceCounter + numVertSteps;
                indices[indexCounter + 4] = faceCounter + 1 + numVertSteps;
                indices[indexCounter + 5] = faceCounter;
            }

            indexCounter += 6;
            faceCounter += 1;
        }
    }
}

void KleinBottle::initTriangles() {
    int faceCounter = 0;
    for (int i=0; i<2*numHorSteps; i++) {
        for (int j=0; j<numVertSteps; j++) {
            if (i == 2 * numHorSteps - 1 and j == numVertSteps - 1) {
                int index1 = (faceCounter + 1 - numHorSteps) % numVertices;
                int index2 = faceCounter % numVertices;
                int halfRotation = floor(numVertSteps / 2);
                int index3 = floorMod((faceCounter + halfRotation + numVertSteps) % numVertices - 2 * j, numVertSteps);
                triangles.push_back({{index1, index2, index3}});

                int index4 = floorMod((faceCounter + halfRotation + numVertSteps) % numVertices - 2 * j, numVertSteps);
                int index5 = floorMod((faceCounter + halfRotation + 1 + numVertSteps) % numVertices - 2 * j, numVertSteps);
                int index6 = faceCounter % numVertices;
                triangles.push_back({{index4, index5, index6}});
            }
            else if (i == 2 * numHorSteps - 1) {
                int index1 = (faceCounter + 1) % numVertices;
                int index2 = faceCounter % numVertices;
                int halfRotation = floor(numVertSteps / 2);
                int index3 = floorMod((faceCounter + halfRotation + numVertSteps) % numVertices - 2 * j, numVertSteps);
                triangles.push_back({{index1, index2, index3}});

                int index4 = floorMod((faceCounter + halfRotation + numVertSteps) % numVertices - 2 * j, numVertSteps);
                int index5 = floorMod((faceCounter + halfRotation + 1 + numVertSteps) % numVertices - 2 * j, numVertSteps);
                int index6 = faceCounter % numVertices;
                triangles.push_back({{index4, index5, index6}});
            }
            else if (j == numVertSteps - 1) {
                int index1 = faceCounter + 1 - numHorSteps;
                int index2 = faceCounter;
                int index3 = faceCounter + 1;
                triangles.push_back({{index1, index2, index3}});

                int index4 = faceCounter + numHorSteps;
                int index5 = faceCounter + 1;
                int index6 = faceCounter;
                triangles.push_back({{index4, index5, index6}});
            }
            else {
                int index1  = faceCounter + 1;
                int index2 = faceCounter;
                int index3 = faceCounter + 1 + numVertSteps;
                triangles.push_back({{index1, index2, index3}});

                int index4 = faceCounter + numVertSteps;
                int index5 = faceCounter + 1 + numVertSteps;
                int index6 = faceCounter;
                triangles.push_back({{index4, index5, index6}});
            }
            faceCounter += 1;
        }
    }
}

std::vector<Triangle> KleinBottle::getTriangles() {
    return triangles;
}

int KleinBottle::floorMod(int a, int n) {
    int q = (int) floor((double)a / n);
    return a - n * q;
}

Vertex* KleinBottle::getVertices() {
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
