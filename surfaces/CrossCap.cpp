#include "CrossCap.h"
#include <cmath>

CrossCap::CrossCap(QVector3D centerPosition, QVector3D scale) {
    initVertices(centerPosition, scale);
    initTriangles();
    initNeighbors();
}

void CrossCap::initVertices(QVector3D centerPosition, QVector3D scale) {
    float sizeX = scale.x();
    float sizeY = scale.y();
    float sizeZ = scale.z();
    float horStepSize = 2 * M_PI / numHorSteps;
    float vertStepSize = M_PI / (2 * numVertSteps);

    int vertexCounter = 0;
    for (int i=0; i<=numVertSteps; i++) {
        float verticalAngle = vertStepSize * i;
        for (int j=0; j<numHorSteps; j++) {
            float horizontalAngle = horStepSize * j;

            float x = centerPosition.x() + sizeX * cosf(horizontalAngle) * sinf(2 * verticalAngle);
            float y = centerPosition.y() + sizeY * sinf(horizontalAngle) * sinf(2 * verticalAngle);
            float z = centerPosition.z() + sizeZ * (pow(cosf(verticalAngle), 2) - pow(cosf(horizontalAngle), 2) * pow(sinf(verticalAngle), 2));

            vertices[vertexCounter] = {.index = vertexCounter, .position = QVector3D(x,  y,  z)};
            vertexCounter += 1;
        }
    }
}

void CrossCap::initTriangles() {
    int faceCounter = 0;
    int triangleCounter = 0;
    for (int i=0; i<=numVertSteps; i++) {
        for (int j=0; j<numHorSteps; j++) {
            if (j == numHorSteps - 1) {
                int index1 = (faceCounter + 1 - numVertSteps) % numVertices;
                int index2 = faceCounter % numVertices;
                int index3 = (faceCounter + 1) % numVertices;
                triangles[triangleCounter] = {{index1, index2, index3}};

                int index4 = (faceCounter + numVertSteps) % numVertices;
                int index5 = (faceCounter + 1) % numVertices;
                int index6 = faceCounter % numVertices;
                triangles[triangleCounter+1] = {{index4, index5, index6}};
            }
            else {
                int index1 = (faceCounter + 1) % numVertices;
                int index2 = faceCounter % numVertices;
                int index3 = (faceCounter + 1 + numHorSteps) % numVertices;
                triangles[triangleCounter] = {{index1, index2, index3}};

                int index4 = (faceCounter + numHorSteps) % numVertices;
                int index5 = (faceCounter + 1 + numHorSteps) % numVertices;
                int index6 = faceCounter % numVertices;
                triangles[triangleCounter+1] = {{index4, index5, index6}};
            }

            faceCounter += 1;
            triangleCounter += 2;
        }
    }
}

Triangle* CrossCap::getTriangles() {
    return triangles;
}

Vertex* CrossCap::getVertices() {
    return vertices;
}

int CrossCap::getNumVertices() {
    return numVertices;
}

int CrossCap::getNumTriangles() {
    return numTriangles;
}
