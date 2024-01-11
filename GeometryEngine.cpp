#include "GeometryEngine.h"
#include <QVector2D>

struct VertexData {
    QVector3D position;
};

GeometryEngine::GeometryEngine() : indexBuf(QOpenGLBuffer::IndexBuffer) {
    initializeOpenGLFunctions();

    // Generate 2 VBOs
    arrayBuf.create();
    indexBuf.create();

    // Initializes cube geometry and transfers it to VBOs
    initCubeGeometry();
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

void GeometryEngine::drawCubeGeometry(QOpenGLShaderProgram *program) {
    // Tell OpenGL which VBOs to use
    arrayBuf.bind();
    indexBuf.bind();

    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = program->attributeLocation("a_position");
    program->enableAttributeArray(vertexLocation);
    program->setAttributeBuffer(vertexLocation, GL_FLOAT, 0, 3, sizeof(VertexData));

    // Draw cube geometry using indices from VBO 1
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, nullptr);
}
