#include "Torus.h"

Torus::Torus() {
    initVertices();
    initIndices();
    initTriangles();
    initNeighbors();
}

void Torus::initVertices() {
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
}

void Torus::initIndices() {
    int indexCounter = 0;
    int faceCounter = 0;
    for (int i=0; i<numHorSteps; i++) {
        for (int j=0; j<numVertSteps; j++) {
            if (j == numVertSteps - 1) {
                indices[indexCounter] = (faceCounter + 1 - numHorSteps) % numVertices;
                indices[indexCounter + 1] = faceCounter % numVertices;
                indices[indexCounter + 2] = (faceCounter + 1) % numVertices;
                indices[indexCounter + 3] = (faceCounter + numHorSteps) % numVertices;
                indices[indexCounter + 4] = (faceCounter + 1) % numVertices;
                indices[indexCounter + 5] = faceCounter % numVertices;
            }
            else {
                indices[indexCounter] = (faceCounter + 1) % numVertices;
                indices[indexCounter + 1] = faceCounter % numVertices;
                indices[indexCounter + 2] = (faceCounter + 1 + numVertSteps) % numVertices;
                indices[indexCounter + 3] = (faceCounter + numVertSteps) % numVertices;
                indices[indexCounter + 4] = (faceCounter + 1 + numVertSteps) % numVertices;
                indices[indexCounter + 5] = faceCounter % numVertices;
            }

            indexCounter += 6;
            faceCounter += 1;
        }
    }
}

void Torus::initTriangles() {
    int faceCounter = 0;
    for (int i=0; i<numHorSteps; i++) {
        for (int j=0; j<numVertSteps; j++) {
            if (j == numVertSteps - 1) {
                int index1 = (faceCounter + 1 - numHorSteps) % numVertices;
                int index2 = faceCounter % numVertices;
                int index3 = (faceCounter + 1) % numVertices;
                triangles.push_back({{index1, index2, index3}});

                int index4 = (faceCounter + numHorSteps) % numVertices;
                int index5 = (faceCounter + 1) % numVertices;
                int index6 = faceCounter % numVertices;
                triangles.push_back({{index4, index5, index6}});
            }
            else {
                int index1 = (faceCounter + 1) % numVertices;
                int index2 = faceCounter % numVertices;
                int index3 = (faceCounter + 1 + numVertSteps) % numVertices;
                triangles.push_back({{index1, index2, index3}});

                int index4 = (faceCounter + numVertSteps) % numVertices;
                int index5 = (faceCounter + 1 + numVertSteps) % numVertices;
                int index6 = faceCounter % numVertices;
                triangles.push_back({{index4, index5, index6}});
            }
            faceCounter += 1;
        }
    }
}

std::vector<Triangle> Torus::getTriangles() {
    return triangles;
}

Vertex* Torus::getVertices() {
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
