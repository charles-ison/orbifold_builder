#include <QtGui/qcolor.h>
#include "GeometryEngine.h"

GeometryEngine::GeometryEngine() : indexBuf(QOpenGLBuffer::IndexBuffer) {
    initializeOpenGLFunctions();

    // Generate VBOs
    arrayBuf.create();
    indexBuf.create();
    lineArrayBuf.create();
    boundaryArrayBuf.create();
    boundaryIndexBuf.create();
}

GeometryEngine::~GeometryEngine() {
    arrayBuf.destroy();
    indexBuf.destroy();
    lineArrayBuf.destroy();
    boundaryArrayBuf.destroy();
    boundaryIndexBuf.destroy();
}

void GeometryEngine::initAnimation(Mesh* mesh) {
    std::vector<Vertex*> meshVertices = mesh->getVertices();
    for (int i=indexToStartAnimation; i<indexToEndAnimation; i++) {
        animationPositions.push_back(meshVertices[i]->position);
    }

    // Transfer vertex data to VBO 0
    arrayBuf.bind();
    arrayBuf.allocate(&animationPositions[0], indexToEndAnimation * sizeof(QVector3D));

    std::vector<GLushort> indices;
    std::vector<Triangle*> triangles = mesh->getTriangles();
    numIndices = 0;
    for (int i=0; i<triangles.size(); i++) {
        int index1 = triangles[i]->vertexIndices[0];
        int index2 = triangles[i]->vertexIndices[1];
        int index3 = triangles[i]->vertexIndices[2];

        if (index1 < indexToEndAnimation && index2 < indexToEndAnimation && index3 < indexToEndAnimation) {
            indices.push_back(index1);
            indices.push_back(index2);
            indices.push_back(index3);
            numIndices += 3;
        }
    }

    // Transfer index data to VBO 1
    indexBuf.bind();
    indexBuf.allocate(&indices[0], numIndices * sizeof(GLushort));

    if (indexToEndAnimation + animationSpeed > meshVertices.size()) {
        indexToStartAnimation = 0;
        indexToEndAnimation = originalIndexToEndAnimation;
        animationPositions.clear();
    } else {
        indexToStartAnimation = indexToEndAnimation;
        indexToEndAnimation += animationSpeed;
    }
}

void GeometryEngine::initMesh(Mesh* mesh) {
    std::vector<QVector3D> positions;
    std::vector<Vertex*> meshVertices = mesh->getVertices();
    int numVertices = meshVertices.size();

    for (Vertex* vertexPointer : meshVertices) {
        positions.push_back(vertexPointer->position);
    }

    // Transfer position data to VBO 0
    arrayBuf.bind();
    arrayBuf.allocate(&positions[0], numVertices * sizeof(QVector3D));

    std::vector<GLushort> indices;
    std::vector<Triangle*> triangles = mesh->getTriangles();
    int numTriangles = triangles.size();
    numIndices = 3*numTriangles;

    for (int i=0; i<numTriangles; i++) {
        indices.push_back(triangles[i]->vertexIndices[0]);
        indices.push_back(triangles[i]->vertexIndices[1]);
        indices.push_back(triangles[i]->vertexIndices[2]);
    }

    // Transfer index data to VBO 1
    indexBuf.bind();
    indexBuf.allocate(&indices[0], numIndices * sizeof(GLushort));
}

void GeometryEngine::initLine(std::vector<Vertex*> lineVerticesVector) {
    std::vector<QVector3D> positions;
    numLineVertices = lineVerticesVector.size();

    for (Vertex* lineVertex : lineVerticesVector) {
        positions.push_back(lineVertex->position);
    }

    // Transfer position data to VBO 0
    lineArrayBuf.bind();
    lineArrayBuf.allocate(&positions[0], numLineVertices * sizeof(QVector3D));
}

void GeometryEngine::initBoundary(std::vector<Vertex*> boundaryVerticesVector1, std::vector<Vertex*> boundaryVerticesVector2) {
    std::vector<QVector3D> positions;
    std::vector<GLushort> boundaryIndices;
    numBoundaryVertices = boundaryVerticesVector1.size() + boundaryVerticesVector2.size();

    for (Vertex* boundaryVertex : boundaryVerticesVector1) {
        positions.push_back(boundaryVertex->position);
    }

    for (Vertex* boundaryVertex : boundaryVerticesVector2) {
        positions.push_back(boundaryVertex->position);
    }

    for (int i=0; i<numBoundaryVertices; i++) {
        boundaryIndices.push_back(i);
    }

    for (int i=numBoundaryVertices; i>0; i--) {
        boundaryIndices.push_back(i);
    }

    // Transfer position data to VBO 0
    boundaryArrayBuf.bind();
    boundaryArrayBuf.allocate(&positions[0], numBoundaryVertices * sizeof(QVector3D));

    // Transfer color scale data to VBO 0
    boundaryIndexBuf.bind();
    boundaryIndexBuf.allocate(&boundaryIndices[0], numBoundaryVertices * sizeof(GLushort));
}

void GeometryEngine::drawMesh(QOpenGLShaderProgram *program) {
    // Tell OpenGL which VBOs to use
    arrayBuf.bind();
    indexBuf.bind();

    // Tell OpenGL programmable pipeline how to locate position data
    int positionLocation = program->attributeLocation("a_position");
    program->enableAttributeArray(positionLocation);
    program->setAttributeBuffer(positionLocation, GL_FLOAT, 0, 3, sizeof(QVector3D));

    program->setUniformValue("input_color_flag", (GLfloat)0.0);

    // Draw geometry using indices from VBO 1
    glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, nullptr);
}

void GeometryEngine::drawLine(QOpenGLShaderProgram *program, QColor color) {
    // Tell OpenGL which VBOs to use
    lineArrayBuf.bind();

    // Tell OpenGL programmable pipeline how to locate position data
    int positionLocation = program->attributeLocation("a_position");
    program->enableAttributeArray(positionLocation);
    program->setAttributeBuffer(positionLocation, GL_FLOAT, 0, 3, sizeof(QVector3D));

    QRgb rgb = color.rgb();
    program->setUniformValue("line_color", qRed(rgb), qGreen(rgb), qBlue(rgb));
    program->setUniformValue("input_color_flag", (GLfloat)1.0);

    // Draw geometry using indices from VBO 1
    glLineWidth(10.0);
    glDrawArrays(GL_LINE_STRIP, 0, numLineVertices);
}

// Actually drawLine
void GeometryEngine::drawBoundary(QOpenGLShaderProgram *program) {
    // Tell OpenGL which VBOs to use
    boundaryArrayBuf.bind();
    //boundaryIndexBuf.bind();

    // Tell OpenGL programmable pipeline how to locate position data
    int positionLocation = program->attributeLocation("a_position");
    program->enableAttributeArray(positionLocation);
    program->setAttributeBuffer(positionLocation, GL_FLOAT, 0, 3, sizeof(QVector3D));

    // Tell OpenGL programmable pipeline how to locate position data
    //int colorScaleLocation = program->attributeLocation("input_color_scale");
    //program->enableAttributeArray(colorScaleLocation);
    //program->setAttributeBuffer(colorScaleLocation, GL_FLOAT, 0, 3, sizeof(GLushort));

    program->setUniformValue("input_color_flag", (GLfloat) 2.0);
    program->setUniformValue("input_boundary_count", (GLfloat) numBoundaryVertices/2);

    // Draw geometry using indices from VBO 1
    glLineWidth(10.0);
    glDrawArrays(GL_LINE_STRIP, 0, numBoundaryVertices);
}
