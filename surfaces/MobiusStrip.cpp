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
    for (int i=0; i<numHorSteps; i++) {
        float horizontalAngle = -M_PI + horStepSize * i;
        for (int j=0; j<numVertSteps; j++) {
            float verticalStep = -0.5 + vertStepSize * j;
            float x = cosf(horizontalAngle) * (radius + verticalStep * cosf(horizontalAngle/2));
            float y = sinf(horizontalAngle) * (radius + verticalStep * cosf(horizontalAngle/2));
            float z = verticalStep * sinf(horizontalAngle/2);

            vertices.push_back({.index = vertexCounter, .position = QVector3D(x,  y,  z)});
            vertexCounter += 1;
        }
    }
}

void MobiusStrip::initTriangles() {
    int indexCounter = 0;
    for (int i=0; i<numHorSteps; i++) {
        for (int j=0; j<numVertSteps-1; j++) {
            if (i < numHorSteps-1) {
                int index1 = indexCounter % numVertices;
                int index2 = (indexCounter + numVertSteps) % numVertices;
                int index3 = (indexCounter + 1) % numVertices;
                triangles.push_back({{index1, index2, index3}});

                int index4 = (indexCounter + 1) % numVertices;
                int index5 = (indexCounter + numVertSteps) % numVertices;
                int index6 = (indexCounter + numVertSteps + 1) % numVertices;
                triangles.push_back({{index4, index5, index6}});
            } else {
                int index1 = indexCounter % numVertices;
                int index2 = (indexCounter + numVertSteps) % numVertices;
                if (index2 < numVertSteps) {
                    index2 = numVertSteps - index2 - 1;
                }
                int index3 = (indexCounter + 1) % numVertices;
                triangles.push_back({{index1, index2, index3}});

                int index4 = (indexCounter + 1) % numVertices;
                int index5 = (indexCounter + numVertSteps) % numVertices;
                if (index5 < numVertSteps) {
                    index5 = numVertSteps - index5 - 1;
                }
                int index6 = (indexCounter + numVertSteps + 1) % numVertices;
                if (index6 < numVertSteps) {
                    index6 = numVertSteps - index6 - 1;
                }
                triangles.push_back({{index4, index5, index6}});
            }

            if (indexCounter % numVertSteps == numVertSteps-2 && indexCounter != 0) {
                indexCounter += 2;
            } else {
                indexCounter += 1;
            }
        }
    }
}

Triangle* MobiusStrip::getTriangles() {
    return &triangles[0];
}

Vertex* MobiusStrip::getVertices() {
    return &vertices[0];
}

int MobiusStrip::getNumVertices() {
    return numVertices;
}

int MobiusStrip::getNumTriangles() {
    return triangles.size();
}
