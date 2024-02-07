#include "MobiusStrip.h"

MobiusStrip::MobiusStrip(QVector3D centerPosition, QVector3D scale) {
    initVertices(centerPosition, scale);
    initTriangles();
    initNeighbors();
}

void MobiusStrip::initVertices(QVector3D centerPosition, QVector3D scale) {
    float radius = scale.x();
    float horStepSize = 2 * M_PI / numHorSteps;
    float vertStepSize = 1.0 / numVertSteps;

    int vertexCounter = 0;
    for (int i=0; i<=numHorSteps; i++) {
        float horizontalAngle = -M_PI + horStepSize * i;
        for (int j=0; j<=numVertSteps; j++) {
            float verticalStep = -0.5 + vertStepSize * j;
            float x = cosf(horizontalAngle) * (radius + verticalStep * cosf(horizontalAngle/2));
            float y = sinf(horizontalAngle) * (radius + verticalStep * cosf(horizontalAngle/2));
            float z = verticalStep * sinf(horizontalAngle/2);

            vertices[vertexCounter] = {QVector3D(x,  y,  z)};
            vertexCounter += 1;
        }
    }
}

void MobiusStrip::initTriangles() {
    int quadCounter = 0;
    int triangleCounter = 0;
    for (int i=0; i<numHorSteps; i++) {
        for (int j=0; j<=numVertSteps; j++) {
            int index1 = quadCounter + 1;
            int index2 = quadCounter;
            int index3 = quadCounter + 1 + numVertSteps;
            triangles[triangleCounter] = {{index1, index2, index3}};

            int index4 = quadCounter + numVertSteps;
            int index5 = quadCounter + 1 + numVertSteps;
            int index6 = quadCounter;
            triangles[triangleCounter+1] = {{index4, index5, index6}};

            quadCounter += 1;
            triangleCounter += 2;
        }
    }
}

Triangle* MobiusStrip::getTriangles() {
    return triangles;
}

Vertex* MobiusStrip::getVertices() {
    return vertices;
}

int MobiusStrip::getNumVertices() {
    return numVertices;
}

int MobiusStrip::getNumTriangles() {
    return numTriangles;
}
