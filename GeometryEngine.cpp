#include <QtGui/qcolor.h>
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

void GeometryEngine::initSurface(TestSurface* surface) {
    int numVertices = surface->getNumVertices();
    VertexData vertices[numVertices];
    std::vector<VertexData*> surfaceVertices = surface->getVertices();
    for (int i=0; i<numVertices; i++) {
        vertices[i] = *surfaceVertices[i];
    }

    // Transfer vertex data to VBO 0
    arrayBuf.bind();
    arrayBuf.allocate(vertices, numVertices * sizeof(VertexData));

    numIndices = surface->getNumIndices();
    GLushort indices[numIndices];
    std::vector<GLushort> surfaceIndices = surface->getIndices();
    for (int i=0; i<numIndices; i++) {
        indices[i] = surfaceIndices[i];
    }

    // Transfer index data to VBO 1
    indexBuf.bind();
    indexBuf.allocate(indices, numIndices * sizeof(GLushort));
}

void GeometryEngine::initLine(std::vector<VertexData*> lineVerticesVector) {
    numLineVertices = lineVerticesVector.size();
    VertexData lineVertices[numLineVertices];

    for (int i=0; i<numLineVertices; i++) {
        lineVertices[i] = *lineVerticesVector[i];
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

    program->setUniformValue("use_line_color", (GLfloat)0.0);

    // Draw geometry using indices from VBO 1
    glDrawElements(GL_TRIANGLE_STRIP, numIndices, GL_UNSIGNED_SHORT, nullptr);
}

void GeometryEngine::drawLine(QOpenGLShaderProgram *program, QColor color) {
    // Tell OpenGL which VBOs to use
    lineArrayBuf.bind();

    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = program->attributeLocation("a_position");
    program->enableAttributeArray(vertexLocation);
    program->setAttributeBuffer(vertexLocation, GL_FLOAT, 0, 3, sizeof(VertexData));

    QRgb rgb = color.rgb();
    program->setUniformValue("line_color", qRed(rgb), qGreen(rgb), qBlue(rgb));
    program->setUniformValue("use_line_color", (GLfloat)1.0);

    // Draw geometry using indices from VBO 1
    glLineWidth(10.0);
    glDrawArrays(GL_LINE_STRIP, 0, numLineVertices);
}
