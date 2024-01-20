#include "GeometryEngine.h"

GeometryEngine::GeometryEngine(Surface* surface) : indexBuf(QOpenGLBuffer::IndexBuffer) {
    initializeOpenGLFunctions();

    // Generate 2 VBOs
    arrayBuf.create();
    indexBuf.create();

    // Initializes surface geometry and transfers it to VBOs
    initSurface(surface);
}

GeometryEngine::~GeometryEngine() {
    arrayBuf.destroy();
    indexBuf.destroy();
}

void GeometryEngine::initSurface(Surface* surface) {
    int numVertices = surface->getNumVertices();
    VertexData vertices[numVertices];
    VertexData *verticesPointer = surface->getVertices();
    for (int i=0; i<numVertices; i++) {
        vertices[i] = verticesPointer[i];
    }

    // Transfer vertex data to VBO 0
    arrayBuf.bind();
    arrayBuf.allocate(vertices, numVertices * sizeof(VertexData));

    numIndices = surface->getNumIndices();
    GLushort indices[numIndices];
    GLushort *indicesPointer = surface->getIndices();
    for (int i=0; i<numIndices; i++) {
        indices[i] = indicesPointer[i];
    }

    // Transfer index data to VBO 1
    indexBuf.bind();
    indexBuf.allocate(indices, numIndices * sizeof(GLushort));
}

void GeometryEngine::drawSurface(QOpenGLShaderProgram *program) {
    // Tell OpenGL which VBOs to use
    arrayBuf.bind();
    indexBuf.bind();

    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = program->attributeLocation("a_position");
    program->enableAttributeArray(vertexLocation);
    program->setAttributeBuffer(vertexLocation, GL_FLOAT, 0, 3, sizeof(VertexData));

    // Draw geometry using indices from VBO 1
    glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, nullptr);
    //glDrawElements(GL_POINTS, numIndices, GL_UNSIGNED_SHORT, nullptr);
    //glDrawElements(GL_LINES, numIndices, GL_UNSIGNED_SHORT, nullptr);
}
