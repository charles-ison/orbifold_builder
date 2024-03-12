#include "Sphere.h"

Sphere::Sphere(QVector3D centerPosition, QVector3D scale) {
    initVertices(centerPosition, scale);
    initTriangles();
}

void Sphere::initVertices(QVector3D centerPosition, QVector3D scale) {
    float sizeX = scale.x();
    float sizeY = scale.y();
    float sizeZ = scale.z();
    float vertStepSize = M_PI / numVertSteps;
    float horStepSize = 2 * M_PI / numHorSteps;

    for (int i=0; i<=numVertSteps; i++) {
        float verticalAngle = (M_PI / 2) - vertStepSize * i;
        float z = sizeZ * sinf(verticalAngle);
        int tempNumHorSteps = (i == 0 || i == numVertSteps) ? 1 : numHorSteps;
        for (int j=0; j<tempNumHorSteps; j++) {
            float horizontalAngle = horStepSize * j;
            float x = sizeX * cosf(verticalAngle) * cosf(horizontalAngle);
            float y = sizeY * cosf(verticalAngle) * sinf(horizontalAngle);
            vertices.push_back({.index = (int) vertices.size(), .position = QVector3D(x,  y,  z)});
        }
    }
}

void Sphere::initTriangles() {
    int faceCounter = 0;
    for (int i=0; i<numVertSteps; i++) {
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
            else if (i == numVertSteps - 1) {
                int index1 = numVertices - 1;
                int index2 = faceCounter;
                int index3;
                if (j == numHorSteps-1) {
                    index3 = faceCounter + 1 - numHorSteps ;
                } else {
                    index3 = faceCounter + 1;
                }
                triangles.push_back({{&vertices[index3], &vertices[index2], &vertices[index1]}});
            }
            else if (j == numHorSteps-1) {
                int index1 = faceCounter + 1 - numHorSteps;
                int index2 = faceCounter;
                int index3 = faceCounter + 1;
                triangles.push_back({{&vertices[index1], &vertices[index2], &vertices[index3]}});

                int index4 = faceCounter + numHorSteps;
                int index5 = faceCounter + 1;
                int index6 = faceCounter;
                triangles.push_back({{&vertices[index4], &vertices[index5], &vertices[index6]}});
            }
            else {
                int index1 = faceCounter + 1;
                int index2 = faceCounter;
                int index3 = faceCounter + 1 + numHorSteps;
                triangles.push_back({{&vertices[index1], &vertices[index2], &vertices[index3]}});

                int index4 = faceCounter + numHorSteps;
                int index5 = faceCounter + 1 + numHorSteps;
                int index6 = faceCounter;
                triangles.push_back({{&vertices[index4], &vertices[index5], &vertices[index6]}});
            }
            faceCounter += 1;
        }
    }
}

Triangle* Sphere::getTriangles() {
    return &triangles[0];
}

Vertex* Sphere::getVertices() {
    return &vertices[0];
}

int Sphere::getNumVertices() {
    return vertices.size();
}

int Sphere::getNumTriangles() {
    return triangles.size();
}
