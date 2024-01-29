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

void GeometryEngine::initMesh(Mesh* mesh) {
    std::vector<Vertex*> surfaceVertices = mesh->getVertices();
    int numVertices = surfaceVertices.size();
    Vertex vertices[numVertices];
    for (int i=0; i<numVertices; i++) {
        vertices[i] = *surfaceVertices[i];
    }

    // Transfer vertex data to VBO 0
    arrayBuf.bind();
    arrayBuf.allocate(vertices, numVertices * sizeof(Vertex));

    //std::vector<GLushort> surfaceIndices = mesh->getIndices();
    //numIndices = surfaceIndices.size();
    //GLushort indices[numIndices];
    //for (int i=0; i<numIndices; i++) {
    //    indices[i] = surfaceIndices[i];
    //}

    std::vector<Triangle> triangles = mesh->getTriangles();
    int numTriangles = triangles.size();
    numIndices = 3*numTriangles;
    GLushort indices[numIndices];
    for (int i=0; i<numTriangles; i++) {
        indices[3*i] = triangles[i].vertexIndices[0];
        indices[3*i+1] = triangles[i].vertexIndices[1];
        indices[3*i+2] = triangles[i].vertexIndices[2];
    }

    // Transfer index data to VBO 1
    indexBuf.bind();
    indexBuf.allocate(indices, numIndices * sizeof(GLushort));
}

void GeometryEngine::initLine(std::vector<Vertex*> lineVerticesVector) {
    numLineVertices = lineVerticesVector.size();
    Vertex lineVertices[numLineVertices];

    for (int i=0; i<numLineVertices; i++) {
        lineVertices[i] = *lineVerticesVector[i];
    }

    // Transfer vertex data to VBO 0
    lineArrayBuf.bind();
    lineArrayBuf.allocate(lineVertices, numLineVertices * sizeof(Vertex));
}

void GeometryEngine::drawMesh(QOpenGLShaderProgram *program) {
    // Tell OpenGL which VBOs to use
    arrayBuf.bind();
    indexBuf.bind();

    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = program->attributeLocation("a_position");
    program->enableAttributeArray(vertexLocation);
    program->setAttributeBuffer(vertexLocation, GL_FLOAT, 0, 3, sizeof(Vertex));

    program->setUniformValue("use_line_color", (GLfloat)0.0);

    // Draw geometry using indices from VBO 1
    glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, nullptr);
}

void GeometryEngine::drawLine(QOpenGLShaderProgram *program, QColor color) {
    // Tell OpenGL which VBOs to use
    lineArrayBuf.bind();

    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = program->attributeLocation("a_position");
    program->enableAttributeArray(vertexLocation);
    program->setAttributeBuffer(vertexLocation, GL_FLOAT, 0, 3, sizeof(Vertex));

    QRgb rgb = color.rgb();
    program->setUniformValue("line_color", qRed(rgb), qGreen(rgb), qBlue(rgb));
    program->setUniformValue("use_line_color", (GLfloat)1.0);

    // Draw geometry using indices from VBO 1
    glLineWidth(5.0);
    glDrawArrays(GL_LINE_STRIP, 0, numLineVertices);
}
