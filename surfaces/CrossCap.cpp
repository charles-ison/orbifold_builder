#include "CrossCap.h"
#include <cmath>

CrossCap::CrossCap() {
    initVertices();
    initTriangles();
    initNeighbors();
}

void CrossCap::initVertices() {
    //float horStepSize = 2 * M_PI / numHorSteps;
    //float vertStepSize = M_PI / (2 * numVertSteps);
    float horStepSize = M_PI / numHorSteps;
    float vertStepSize = M_PI / (numVertSteps);

    int vertexCounter = 0;
    for (int i=0; i<numHorSteps; i++) {
        float horizontalAngle = horStepSize * i;
        for (int j=0; j<=numVertSteps; j++) {
            float verticalAngle = vertStepSize * j;
            //float x = radius * cosf(horizontalAngle) * sinf(2 * verticalAngle);
            //float y = radius * sinf(horizontalAngle) * sinf(2 * verticalAngle);
            //float z = radius * (pow(cosf(verticalAngle), 2) - pow(cosf(horizontalAngle), 2) * pow(sinf(verticalAngle), 2));

            //float x = radius * pow(cosf(horizontalAngle), 2) * sinf(2 * verticalAngle);
            //float y = radius * sinf(2 * horizontalAngle) * cosf(verticalAngle);
            //float z = (-1) * radius * pow(cosf(horizontalAngle), 2) * cosf(2 * verticalAngle);

            //float x = radius * sinf(horizontalAngle) * sinf(2 * verticalAngle);
            //float y = radius * sinf(2 * horizontalAngle) * pow(sinf(verticalAngle), 2);
            //float z = radius * cosf(2 * horizontalAngle) * pow(sinf(verticalAngle), 2);

            float x = radius * sinf(2 * horizontalAngle) * sinf(verticalAngle);
            float y = radius * pow(cosf(horizontalAngle), 2) * sinf(2 * verticalAngle);
            float z = radius * (pow(sinf(horizontalAngle), 2) - pow(cosf(horizontalAngle), 2) * pow(sinf(verticalAngle), 2));

            vertices[vertexCounter] = {QVector3D(x,  y,  z)};
            vertexCounter += 1;
        }
    }
}

void CrossCap::initTriangles() {
    int faceCounter = 0;
    int triangleCounter = 0;
    for (int i=0; i<numHorSteps; i++) {
        for (int j=0; j<=numVertSteps; j++) {
            int index1 = (faceCounter + 1) % numVertices;
            int index2 = faceCounter % numVertices;
            int index3 = (faceCounter + 1 + numVertSteps) % numVertices;
            triangles[triangleCounter] = {{index1, index2, index3}};

            int index4 = (faceCounter + numVertSteps) % numVertices;
            int index5 = (faceCounter + 1 + numVertSteps) % numVertices;
            int index6 = faceCounter % numVertices;
            triangles[triangleCounter+1] = {{index4, index5, index6}};

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
