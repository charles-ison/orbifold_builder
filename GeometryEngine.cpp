#include "GeometryEngine.h"
#include <QVector2D>
#include <iostream>

GeometryEngine::GeometryEngine() : indexBuf(QOpenGLBuffer::IndexBuffer) {
    initializeOpenGLFunctions();

    // Generate 2 VBOs
    arrayBuf.create();
    indexBuf.create();

    cube = new Cube();
    sphere = new Sphere();

    // Initializes cube geometry and transfers it to VBOs
    //initCubeGeometry();
    initSphereGeometry();
}

GeometryEngine::~GeometryEngine() {
    arrayBuf.destroy();
    indexBuf.destroy();
}

void GeometryEngine::initCubeGeometry() {
    int numVertices = cube->getNumVertices();
    VertexData vertices[numVertices];
    VertexData *verticesPointer = cube->getVertices();
    for (int i=0; i<numVertices; i++) {
        vertices[i] = verticesPointer[i];
    }

    // Transfer vertex data to VBO 0
    arrayBuf.bind();
    arrayBuf.allocate(vertices, numVertices * sizeof(VertexData));

    numIndices = cube->getNumIndices();
    GLushort indices[numIndices];
    GLushort *indicesPointer = cube->getIndices();
    for (int i=0; i<numIndices; i++) {
        indices[i] = indicesPointer[i];
    }

    // Transfer index data to VBO 1
    indexBuf.bind();
    indexBuf.allocate(indices, numIndices * sizeof(GLushort));
}

void GeometryEngine::initSphereGeometry() {
    int numVertices = sphere->getNumVertices();
    VertexData vertices[numVertices];
    VertexData *verticesPointer = sphere->getVertices();
    for (int i=0; i<numVertices; i++) {
        vertices[i] = verticesPointer[i];
    }

    // Transfer vertex data to VBO 0
    arrayBuf.bind();
    arrayBuf.allocate(vertices, numVertices * sizeof(VertexData));

    numIndices = sphere->getNumIndices();
    GLushort indices[numIndices];
    GLushort *indicesPointer = sphere->getIndices();
    for (int i=0; i<numIndices; i++) {
        indices[i] = indicesPointer[i];
    }

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
    glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, nullptr);
}
