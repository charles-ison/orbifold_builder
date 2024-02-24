#include "KleinBottle.h"

KleinBottle::KleinBottle(QVector3D centerPosition, QVector3D scale) {
    initVertices(centerPosition, scale);
    initTriangles();
    initNeighbors();
}

void KleinBottle::initVertices(QVector3D centerPosition, QVector3D scale) {
    float sizeX = scale.x();
    float sizeY = scale.y();
    float sizeRadius = scale.z();
    float horStepSize = M_PI / numHorSteps;
    float vertStepSize = 2 * M_PI / numVertSteps;

    int vertexCounter = 0;
    for (int i=0; i<numHorSteps; i++) {
        float horizontalAngle = horStepSize * i;
        for (int j=0; j<numVertSteps; j++) {
            float verticalAngle = vertStepSize * j;
            float r = sizeRadius * (1 - (cosf(horizontalAngle) / 2));
            float x = (sizeX * (1 + sinf(horizontalAngle)) + r * cosf(verticalAngle)) * cosf(horizontalAngle);
            float y = (sizeY + r * cosf(verticalAngle)) * sinf(horizontalAngle);
            float z = r * sinf(verticalAngle);
            vertices.push_back({.index = vertexCounter, .position = QVector3D(x,  y,  z)});
            vertexCounter += 1;
        }
    }

    for (int i=0; i<numHorSteps; i++) {
        float horizontalAngle = M_PI + horStepSize * i;
        for (int j=0; j<numVertSteps; j++) {
            float verticalAngle = vertStepSize * j;
            float r = sizeRadius * (1 - (cosf(horizontalAngle) / 2));
            float x = sizeX * (1 + sinf(horizontalAngle)) * cosf(horizontalAngle) - r * cosf(verticalAngle);
            float y = sizeY * sinf(horizontalAngle);
            float z = r * sinf(verticalAngle);
            vertices.push_back({.index = vertexCounter, .position = QVector3D(x,  y,  z)});
            vertexCounter += 1;
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

Triangle* KleinBottle::getTriangles() {
    return &triangles[0];
}

int KleinBottle::floorMod(int a, int n) {
    int q = (int) floor((double)a / n);
    return a - n * q;
}

Vertex* KleinBottle::getVertices() {
    return &vertices[0];
}

int KleinBottle::getNumVertices() {
    return vertices.size();
}

int KleinBottle::getNumTriangles() {
    return triangles.size();
}
