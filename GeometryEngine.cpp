#include <QtGui/qcolor.h>
#include "GeometryEngine.h"
#include <iostream>

GeometryEngine::GeometryEngine() : indexBuf(QOpenGLBuffer::IndexBuffer) {
    initializeOpenGLFunctions();

    // Generate VBOs
    arrayBuf.create();
    indexBuf.create();
    colorBuf.create();
    lineArrayBuf.create();
    lineColorBuf.create();
    boundaryArrayBuf.create();
    boundaryColorBuf.create();
    boundaryPointBuf.create();
    boundaryPointColorBuf.create();
}

GeometryEngine::~GeometryEngine() {
    arrayBuf.destroy();
    indexBuf.destroy();
    colorBuf.destroy();
    lineArrayBuf.destroy();
    lineColorBuf.destroy();
    boundaryArrayBuf.destroy();
    boundaryColorBuf.destroy();
    boundaryPointBuf.destroy();
    boundaryPointColorBuf.destroy();
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
    std::vector<QVector3D> colors;
    std::vector<Vertex*> meshVertices = mesh->getVertices();
    int numVertices = meshVertices.size();

    for (Vertex* vertexPointer : meshVertices) {
        positions.push_back(vertexPointer->position);
        colors.push_back(vertexPointer->position);
    }

    // Transfer position data to VBO 0
    arrayBuf.bind();
    arrayBuf.allocate(&positions[0], numVertices * sizeof(QVector3D));

    // Transfer position data to VBO 0
    colorBuf.bind();
    colorBuf.allocate(&colors[0], numVertices * sizeof(QVector3D));

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

void GeometryEngine::initLine(std::vector<Vertex*> lineVerticesVector, QColor color) {
    std::vector<QVector3D> positions;
    std::vector<QVector3D> colors;
    numLineVertices = lineVerticesVector.size();

    QRgb rgb = color.rgb();
    QVector3D vecColor = {(float) qRed(rgb), (float) qGreen(rgb), (float) qBlue(rgb)};

    for (Vertex* lineVertex : lineVerticesVector) {
        positions.push_back(lineVertex->position);
        colors.push_back(vecColor);
    }

    // Transfer position data to VBO 0
    lineArrayBuf.bind();
    lineArrayBuf.allocate(&positions[0], numLineVertices * sizeof(QVector3D));

    // Transfer position data to VBO 0
    lineColorBuf.bind();
    lineColorBuf.allocate(&colors[0], numLineVertices * sizeof(QVector3D));
}

void GeometryEngine::initBoundary(std::vector<Vertex*> boundaryVerticesVector1, std::vector<Vertex*> boundaryVerticesVector2) {
    std::vector<QVector3D> positions;
    std::vector<QVector3D> colors;
    std::vector<QVector3D> points;
    std::vector<QVector3D> pointColors;
    numBoundaryVertices = boundaryVerticesVector1.size() + boundaryVerticesVector2.size();

    for (Vertex* boundaryVertex : boundaryVerticesVector1) {
        positions.push_back(boundaryVertex->position);
    }

    for (Vertex* boundaryVertex : boundaryVerticesVector2) {
        positions.push_back(boundaryVertex->position);
    }

    for (int i=0; i<boundaryVerticesVector1.size(); i++) {
        float boundaryColorScale = float(i)/float(boundaryVerticesVector1.size());
        colors.push_back({boundaryColorScale, boundaryColorScale, boundaryColorScale});
    }

    for (float i=boundaryVerticesVector2.size(); i>0; i--) {
        float boundaryColorScale = float(i)/float(boundaryVerticesVector2.size());
        colors.push_back({boundaryColorScale, boundaryColorScale, boundaryColorScale});
    }

    if (boundaryVerticesVector1.size() > 2) {
        numBoundaryPoints = 2;
        points.push_back(boundaryVerticesVector1.front()->position);
        points.push_back(boundaryVerticesVector1.back()->position);
        pointColors.push_back({1, 1, 1});
        pointColors.push_back({0, 0, 0});
    } else {
        numBoundaryPoints = 0;
    }

    // Transfer position data to VBO 0
    boundaryArrayBuf.bind();
    boundaryArrayBuf.allocate(&positions[0], numBoundaryVertices * sizeof(QVector3D));

    // Transfer color scale data to VBO 0
    boundaryColorBuf.bind();
    boundaryColorBuf.allocate(&colors[0], numBoundaryVertices * sizeof(QVector3D));

    // Transfer point data to VBO 0
    boundaryPointBuf.bind();
    boundaryPointBuf.allocate(&points[0], points.size() * sizeof(QVector3D));

    boundaryPointColorBuf.bind();
    boundaryPointColorBuf.allocate(&pointColors[0], pointColors.size() * sizeof(QVector3D));
}

void GeometryEngine::drawMesh(QOpenGLShaderProgram *program) {
    // Tell OpenGL which VBOs to use
    arrayBuf.bind();
    indexBuf.bind();

    // Tell OpenGL programmable pipeline how to locate position data
    int positionLocation = program->attributeLocation("a_position");
    program->enableAttributeArray(positionLocation);
    program->setAttributeBuffer(positionLocation, GL_FLOAT, 0, 3, sizeof(QVector3D));

    // Tell OpenGL which VBOs to use
    colorBuf.bind();

    // Tell OpenGL programmable pipeline how to locate color data
    int inputBoundaryColorLocation = program->attributeLocation("input_color");
    program->enableAttributeArray(inputBoundaryColorLocation);
    program->setAttributeBuffer(inputBoundaryColorLocation, GL_FLOAT, 0, 3, sizeof(QVector3D));

    // Draw geometry using indices from VBO 1
    glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, nullptr);
}

void GeometryEngine::drawLine(QOpenGLShaderProgram *program) {
    // Tell OpenGL which VBOs to use
    lineArrayBuf.bind();

    // Tell OpenGL programmable pipeline how to locate position data
    int positionLocation = program->attributeLocation("a_position");
    program->enableAttributeArray(positionLocation);
    program->setAttributeBuffer(positionLocation, GL_FLOAT, 0, 3, sizeof(QVector3D));

    // Tell OpenGL which VBOs to use
    lineColorBuf.bind();

    // Tell OpenGL programmable pipeline how to locate color data
    int inputBoundaryColorLocation = program->attributeLocation("input_color");
    program->enableAttributeArray(inputBoundaryColorLocation);
    program->setAttributeBuffer(inputBoundaryColorLocation, GL_FLOAT, 0, 3, sizeof(QVector3D));

    // Draw geometry using indices from VBO 1
    glLineWidth(10.0);
    glDrawArrays(GL_LINE_STRIP, 0, numLineVertices);
}

// Actually drawLine
void GeometryEngine::drawBoundary(QOpenGLShaderProgram *program) {
    // Tell OpenGL which VBOs to use
    boundaryArrayBuf.bind();

    // Tell OpenGL programmable pipeline how to locate position data
    int positionLocation = program->attributeLocation("a_position");
    program->enableAttributeArray(positionLocation);
    program->setAttributeBuffer(positionLocation, GL_FLOAT, 0, 3, sizeof(QVector3D));

    // Tell OpenGL which VBOs to use
    boundaryColorBuf.bind();

    // Tell OpenGL programmable pipeline how to locate color data
    int inputBoundaryColorLocation = program->attributeLocation("input_color");
    program->enableAttributeArray(inputBoundaryColorLocation);
    program->setAttributeBuffer(inputBoundaryColorLocation, GL_FLOAT, 0, 3, sizeof(QVector3D));

    // Draw geometry using indices from VBO 1
    glLineWidth(10.0);
    glDrawArrays(GL_LINE_STRIP, 0, numBoundaryVertices);

    // Tell OpenGL which VBOs to use
    boundaryPointBuf.bind();

    // Tell OpenGL programmable pipeline how to locate position data
    int pointLocation = program->attributeLocation("a_position");
    program->enableAttributeArray(pointLocation);
    program->setAttributeBuffer(pointLocation, GL_FLOAT, 0, 3, sizeof(QVector3D));

    // Tell OpenGL which VBOs to use
    boundaryPointColorBuf.bind();

    // Tell OpenGL programmable pipeline how to locate color data
    int inputBoundaryPointColorLocation = program->attributeLocation("input_color");
    program->enableAttributeArray(inputBoundaryPointColorLocation);
    program->setAttributeBuffer(inputBoundaryPointColorLocation, GL_FLOAT, 0, 3, sizeof(QVector3D));

    glPointSize(12.0);
    glEnable(GL_POINT_SMOOTH);
    glDrawArrays(GL_POINTS, 0, numBoundaryPoints);
}
