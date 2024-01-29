#include "Sphere.h"

Sphere::Sphere() {
    initVertices();
    initTriangles();
    initNeighbors();
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

void Sphere::initTriangles() {
    int faceCounter = 0;
    for (int i=0; i<numVertSteps; i++) {
        for (int j=0; j<numHorSteps; j++) {
            if (i == 0) {
                int index1 = i;
                int index2 = faceCounter + 1;
                int index3;
                if (j == numHorSteps-1) {
                    index3 = 1;
                    faceCounter = 0;
                } else {
                    index3 = faceCounter + 2;
                }
                triangles.push_back({{index1, index2, index3}});
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
                triangles.push_back({{index1, index2, index3}});
            }
            else if (j == numHorSteps-1) {
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
                int index1 = faceCounter + 1;
                int index2 = faceCounter;
                int index3 = faceCounter + 1 + numHorSteps;
                triangles.push_back({{index1, index2, index3}});

                int index4 = faceCounter + numHorSteps;
                int index5 = faceCounter + 1 + numHorSteps;
                int index6 = faceCounter;
                triangles.push_back({{index4, index5, index6}});
            }
            faceCounter += 1;
        }
    }
}

std::vector<Triangle> Sphere::getTriangles() {
    return triangles;
}

Vertex* Sphere::getVertices() {
    return vertices;
}

int Sphere::getNumVertices() {
    return numVertices;
}
