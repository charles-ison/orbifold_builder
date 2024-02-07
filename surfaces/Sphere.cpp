#include "Sphere.h"

Sphere::Sphere(float centerX, float centerY, float centerZ) {
    initVertices(centerX, centerY, centerZ);
    initTriangles();
    initNeighbors();
}

void Sphere::initVertices(float centerX, float centerY, float centerZ) {
    float vertStepSize = M_PI / numVertSteps;
    float horStepSize = 2 * M_PI / numHorSteps;

    int vertexCounter = 0;
    for (int i=0; i<=numVertSteps; i++) {
        float verticalAngle = (M_PI / 2) - vertStepSize * i;
        float z = z_size * sinf(verticalAngle);
        int tempNumHorSteps = (i == 0 || i == numVertSteps) ? 1 : numHorSteps;
        for (int j=0; j<tempNumHorSteps; j++) {
            float horizontalAngle = horStepSize * j;
            float x = x_size * cosf(verticalAngle) * cosf(horizontalAngle);
            float y = y_size * cosf(verticalAngle) * sinf(horizontalAngle);
            vertices[vertexCounter] = {QVector3D(x,  y,  z)};
            vertexCounter += 1;
        }
    }
}

void Sphere::initTriangles() {
    int faceCounter = 0;
    int triangleCounter = 0;
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
                triangles[triangleCounter] = {{index1, index2, index3}};
                triangleCounter += 1;
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
                triangles[triangleCounter] = {{index3, index2, index1}};
                triangleCounter += 1;
            }
            else if (j == numHorSteps-1) {
                int index1 = faceCounter + 1 - numHorSteps;
                int index2 = faceCounter;
                int index3 = faceCounter + 1;
                triangles[triangleCounter] = {{index1, index2, index3}};

                int index4 = faceCounter + numHorSteps;
                int index5 = faceCounter + 1;
                int index6 = faceCounter;
                triangles[triangleCounter+1] = {{index4, index5, index6}};

                triangleCounter += 2;
            }
            else {
                int index1 = faceCounter + 1;
                int index2 = faceCounter;
                int index3 = faceCounter + 1 + numHorSteps;
                triangles[triangleCounter] = {{index1, index2, index3}};

                int index4 = faceCounter + numHorSteps;
                int index5 = faceCounter + 1 + numHorSteps;
                int index6 = faceCounter;
                triangles[triangleCounter+1] = {{index4, index5, index6}};

                triangleCounter += 2;
            }
            faceCounter += 1;
        }
    }
}

Triangle* Sphere::getTriangles() {
    return triangles;
}

Vertex* Sphere::getVertices() {
    return vertices;
}

int Sphere::getNumVertices() {
    return numVertices;
}

int Sphere::getNumTriangles() {
    return numTriangles;
}
