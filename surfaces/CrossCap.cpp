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
    for (int i=0; i<numVertSteps; i++) {
        float verticalAngle = vertStepSize * i;
        int tempNumHorSteps = (i == 0) ? 1 : numHorSteps;
        tempNumHorSteps = (i == numVertSteps-1) ? numHorSteps/2 : tempNumHorSteps;
        for (int j=0; j<tempNumHorSteps; j++) {
            float horizontalAngle = horStepSize * j;

            float x = centerPosition.x() + sizeX * cosf(horizontalAngle) * sinf(2 * verticalAngle);
            float y = centerPosition.y() + sizeY * sinf(horizontalAngle) * sinf(2 * verticalAngle);
            float z = centerPosition.z() + sizeZ * (pow(cosf(verticalAngle), 2) - pow(cosf(horizontalAngle), 2) * pow(sinf(verticalAngle), 2));

            vertices.push_back({.index = (int) vertices.size(), .position = QVector3D(x,  y,  z)});
        }
    }
}

int CrossCap::getCrossCapFoldIndex(int oldIndex) {
    if (oldIndex >= numVertices) {
        return (numVertices - numHorSteps/2) + (oldIndex % numVertices);
    }
    else {
        return oldIndex;
    }
}

void CrossCap::initTriangles() {
    int faceCounter = 0;
    for (int i=0; i<numVertSteps-1; i++) {
        for (int j=0; j<numHorSteps; j++) {
            if (i == 0) {
                int index1 = 0;
                int index2 = faceCounter + 1;
                int index3;
                if (j == numHorSteps-1) {
                    index3 = 1;
                    faceCounter = 0;
                } else {
                    index3 = faceCounter + 2;
                }
                triangles.push_back({{&vertices[index1], &vertices[index2], &vertices[index3]}});
            }
            else if (j == numHorSteps - 1) {
                int index1 = getCrossCapFoldIndex((faceCounter + 1 - numVertSteps));
                int index2 = getCrossCapFoldIndex(faceCounter);
                int index3 = getCrossCapFoldIndex((faceCounter + 1));
                triangles.push_back({{&vertices[index1], &vertices[index2], &vertices[index3]}});

                int index4 = getCrossCapFoldIndex((faceCounter + numVertSteps));
                int index5 = getCrossCapFoldIndex((faceCounter + 1));
                int index6 = getCrossCapFoldIndex(faceCounter);
                triangles.push_back({{&vertices[index4], &vertices[index5], &vertices[index6]}});
            }
            else {
                int index1 = getCrossCapFoldIndex((faceCounter + 1));
                int index2 = getCrossCapFoldIndex(faceCounter);
                int index3 = getCrossCapFoldIndex((faceCounter + 1 + numHorSteps));
                triangles.push_back({{&vertices[index1], &vertices[index2], &vertices[index3]}});

                int index4 = getCrossCapFoldIndex((faceCounter + numHorSteps));
                int index5 = getCrossCapFoldIndex((faceCounter + 1 + numHorSteps));
                int index6 = getCrossCapFoldIndex(faceCounter);
                triangles.push_back({{&vertices[index4], &vertices[index5], &vertices[index6]}});
            }

            faceCounter += 1;
        }
    }
}

Triangle* CrossCap::getTriangles() {
    return &triangles[0];
}

Vertex* CrossCap::getVertices() {
    return &vertices[0];
}

int CrossCap::getNumVertices() {
    return vertices.size();
}

int CrossCap::getNumTriangles() {
    return triangles.size();
}
