#include "GeometryEngine.h"
#include <QVector2D>
#include <iostream>

struct VertexData {
    QVector3D position;
};

GeometryEngine::GeometryEngine() : indexBuf(QOpenGLBuffer::IndexBuffer) {
    initializeOpenGLFunctions();

    // Generate 2 VBOs
    arrayBuf.create();
    indexBuf.create();

    // Initializes cube geometry and transfers it to VBOs
    //initCubeGeometry();
    initSphereGeometry();
}

GeometryEngine::~GeometryEngine() {
    arrayBuf.destroy();
    indexBuf.destroy();
}

void GeometryEngine::initCubeGeometry() {
    VertexData vertices[] = {
            {QVector3D(1.0f,  -1.0f,  1.0f)}, //v0
            {QVector3D( 1.0f,  1.0f,  1.0f)}, //v1
            {QVector3D(-1.0f,  1.0f, 1.0f)}, //v2
            {QVector3D( -1.0f,  -1.0f, 1.0f)}, //v3
            {QVector3D(1.0f,  -1.0f,  -1.0f)}, //v4
            {QVector3D( 1.0f,  1.0f,  -1.0f)}, //v5
            {QVector3D(-1.0f,  1.0f, -1.0f)}, //v6
            {QVector3D( -1.0f,  -1.0f, -1.0f)} //v7
    };

    GLushort indices[] = {
        0, 1, 2, 2, 3, 0, // F0
        3, 2, 6, 6, 7, 3,// F1
        3, 7, 4, 4, 0, 3, // F2
        4, 7, 6, 6, 5, 4, // F3
        0, 4, 5, 5, 1, 0, // F4
        1, 5, 6, 6, 2, 1 // F5
    };

    // Transfer vertex data to VBO 0
    arrayBuf.bind();
    arrayBuf.allocate(vertices, 8 * sizeof(VertexData));

    // Transfer index data to VBO 1
    indexBuf.bind();
    indexBuf.allocate(indices, 36 * sizeof(GLushort));
}

void GeometryEngine::initSphereGeometry() {
    int numVertSteps = 50;
    int numHorzSteps = 50;
    float radius = 1;

    int numVertices = (numVertSteps+1) * numHorzSteps;
    int numIndices = 6 * (numVertSteps+1) * numHorzSteps;
    VertexData vertices[numVertices];
    GLushort indices[numIndices];

    float vertStepSize = M_PI / numVertSteps;
    float horzStepSize = 2 * M_PI / numHorzSteps;

    int verticesCounter = 0;
    for (int i=0; i<=numVertSteps; i++) {
        float verticalAngle = (M_PI / 2) - vertStepSize * i;
        float z = radius * sinf(verticalAngle);
        for (int j=0; j<numHorzSteps; j++) {
            float horizontalAngle = horzStepSize * j;
            float x = radius * cosf(verticalAngle) * cosf(horizontalAngle);
            float y = radius * cosf(verticalAngle) * sinf(horizontalAngle);
            vertices[verticesCounter] = {QVector3D(x,  y,  z)};
            verticesCounter += 1;
        }
    }

    int indexCounter = 0;
    int quadCounter = 0;
    for (int i=0; i<numVertSteps; i++) {
        for (int j=0; j<numHorzSteps; j++) {
            if (j != numHorzSteps-1) {
                indices[indexCounter] = quadCounter + 1;
                indices[indexCounter + 1] = quadCounter;
                indices[indexCounter + 2] = quadCounter + 1 + numHorzSteps;
                indices[indexCounter + 3] = quadCounter + numHorzSteps;
                indices[indexCounter + 4] = quadCounter + 1 + numHorzSteps;
                indices[indexCounter + 5] = quadCounter;
            } else {
                indices[indexCounter] = quadCounter + 1 - numHorzSteps;
                indices[indexCounter + 1] = quadCounter;
                indices[indexCounter + 2] = quadCounter + 1;
                indices[indexCounter + 3] = quadCounter + numHorzSteps;
                indices[indexCounter + 4] = quadCounter + 1;
                indices[indexCounter + 5] = quadCounter;
            }
            indexCounter += 6;
            quadCounter += 1;
        }
    }

    // Transfer vertex data to VBO 0
    arrayBuf.bind();
    arrayBuf.allocate(vertices, numVertices * sizeof(VertexData));

    // Transfer index data to VBO 1
    indexBuf.bind();
    indexBuf.allocate(indices, numIndices * sizeof(GLushort));
}

void GeometryEngine::drawCubeGeometry(QOpenGLShaderProgram *program) {
    // Tell OpenGL which VBOs to use
    arrayBuf.bind();
    indexBuf.bind();

    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = program->attributeLocation("a_position");
    program->enableAttributeArray(vertexLocation);
    program->setAttributeBuffer(vertexLocation, GL_FLOAT, 0, 3, sizeof(VertexData));

    // Draw cube geometry using indices from VBO 1
    //glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, nullptr);
    glDrawElements(GL_TRIANGLES, 6*50*50, GL_UNSIGNED_SHORT, nullptr);
}
