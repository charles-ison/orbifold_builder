#include "Torus.h"

Torus::Torus() {
    initVertices();
    initTriangles();
    initNeighbors();
}

void Torus::initVertices() {
    float horStepSize = 2 * M_PI / numHorSteps;
    float vertStepSize = 2 * M_PI / numVertSteps;

    int vertexCounter = 0;
    for (int i=0; i<numHorSteps; i++) {
        float horizontalAngle = horStepSize * i;
        for (int j=0; j<numVertSteps; j++) {
            float verticalAngle = vertStepSize * j;
            float x = (bigRadius + smallRadius * cosf(verticalAngle)) * cosf(horizontalAngle);
            float y = (bigRadius + smallRadius * cosf(verticalAngle)) * sinf(horizontalAngle);
            float z = smallRadius * sinf(verticalAngle);
            vertices[vertexCounter] = {QVector3D(x,  y,  z)};
            vertexCounter += 1;
        }
    }
}

void Torus::initTriangles() {
    int faceCounter = 0;
    int triangleCounter = 0;
    for (int i=0; i<numHorSteps; i++) {
        for (int j=0; j<numVertSteps; j++) {
            if (j == numVertSteps - 1) {
                int index1 = (faceCounter + 1 - numHorSteps) % numVertices;
                int index2 = faceCounter % numVertices;
                int index3 = (faceCounter + 1) % numVertices;
                triangles[triangleCounter] = {{index1, index2, index3}};

                int index4 = (faceCounter + numHorSteps) % numVertices;
                int index5 = (faceCounter + 1) % numVertices;
                int index6 = faceCounter % numVertices;
                triangles[triangleCounter+1] = {{index4, index5, index6}};
            }
            else {
                int index1 = (faceCounter + 1) % numVertices;
                int index2 = faceCounter % numVertices;
                int index3 = (faceCounter + 1 + numVertSteps) % numVertices;
                triangles[triangleCounter] = {{index1, index2, index3}};

                int index4 = (faceCounter + numVertSteps) % numVertices;
                int index5 = (faceCounter + 1 + numVertSteps) % numVertices;
                int index6 = faceCounter % numVertices;
                triangles[triangleCounter+1] = {{index4, index5, index6}};
            }
            faceCounter += 1;
            triangleCounter += 2;
        }
    }
}

Triangle* Torus::getTriangles() {
    return triangles;
}

Vertex* Torus::getVertices() {
    return vertices;
}

int Torus::getNumVertices() {
    return numVertices;
}

int Torus::getNumTriangles() {
    return numTriangles;
}
