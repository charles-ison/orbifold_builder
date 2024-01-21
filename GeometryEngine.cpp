#include "GeometryEngine.h"

GeometryEngine::GeometryEngine() : indexBuf(QOpenGLBuffer::IndexBuffer) {
    initializeOpenGLFunctions();

    // Generate VBOs
    arrayBuf.create();
    indexBuf.create();
    lineArrayBuf.create();
}

GeometryEngine::~GeometryEngine() {
    arrayBuf.destroy();
    indexBuf.destroy();
    lineArrayBuf.destroy();
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

void GeometryEngine::initLine(std::vector<VertexData> lineVerticesVector) {
    numLineVertices = lineVerticesVector.size();
    VertexData lineVertices[numLineVertices];
    for (int i=0; i<numLineVertices; i++) {
        lineVertices[i] = lineVerticesVector[i];
    }

    // Transfer vertex data to VBO 0
    lineArrayBuf.bind();
    lineArrayBuf.allocate(lineVertices, numLineVertices * sizeof(VertexData));
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
}

void GeometryEngine::drawLine(QOpenGLShaderProgram *program) {
    // Tell OpenGL which VBOs to use
    lineArrayBuf.bind();

    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = program->attributeLocation("a_position");
    program->enableAttributeArray(vertexLocation);
    program->setAttributeBuffer(vertexLocation, GL_FLOAT, 0, 3, sizeof(VertexData));

    // Draw geometry using indices from VBO 1
    glDrawArrays(GL_LINE_STRIP, 0, numLineVertices);
}
