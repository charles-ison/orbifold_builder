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
    boundaryArrayBuf1.create();
    boundaryColorBuf1.create();
    boundaryArrayBuf2.create();
    boundaryColorBuf2.create();
}

GeometryEngine::~GeometryEngine() {
    arrayBuf.destroy();
    indexBuf.destroy();
    colorBuf.destroy();
    lineArrayBuf.destroy();
    lineColorBuf.destroy();
    boundaryArrayBuf1.destroy();
    boundaryColorBuf1.destroy();
    boundaryArrayBuf2.destroy();
    boundaryColorBuf2.destroy();
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
    std::set<int> uniqueVertices;
    std::set<std::set<int>> uniqueEdges;
    std::set<std::set<int>> uniqueFaces;

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
        int index0 = triangles[i]->vertexIndices[0];
        int index1 = triangles[i]->vertexIndices[1];
        int index2 = triangles[i]->vertexIndices[2];

        indices.push_back(index0);
        indices.push_back(index1);
        indices.push_back(index2);

        uniqueVertices.insert(index0);
        uniqueVertices.insert(index1);
        uniqueVertices.insert(index2);

        uniqueEdges.insert({index0, index1});
        uniqueEdges.insert({index1, index2});
        uniqueEdges.insert({index2, index0});

        uniqueFaces.insert({index0, index1, index2});
    }

    std::cout << "Number of vertices: " << uniqueVertices.size() << std::endl;
    std::cout << "Number of edges: " << uniqueEdges.size() << std::endl;
    std::cout << "Number of faces: " << uniqueFaces.size() << std::endl;
    std::cout << "Euler characteristic: " << uniqueVertices.size() - uniqueEdges.size() + uniqueFaces.size() << std::endl;

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

std::vector<QVector3D> GeometryEngine::initBoundaryColors(int numVertices, int displaySize) {
    std::vector<QVector3D> colors;
    for (int i=0; i<numVertices; i++) {
        float boundaryColorScale = float(i % displaySize)/float(displaySize-1);
        colors.push_back({boundaryColorScale, boundaryColorScale, boundaryColorScale});
    }
    return colors;
}

void GeometryEngine::initBoundary(std::vector<Vertex*> boundaryVerticesVector1, std::vector<Vertex*> boundaryVerticesVector2, int displaySize1, int displaySize2, bool isBoundary1Loop, bool isBoundary2Loop, bool boundariesAreCombinedLoop, bool boundariesReversed) {
    std::vector<QVector3D> positions1;
    std::vector<QVector3D> positions2;

    std::vector<Vertex*> newBoundaryVerticesVector1 = boundaryVerticesVector1;
    std::vector<Vertex*> newBoundaryVerticesVector2 = boundaryVerticesVector2;
    int newDisplaySize1 = displaySize1;
    int newDisplaySize2 = displaySize2;

    if (isBoundary1Loop) {
        newBoundaryVerticesVector1.push_back(newBoundaryVerticesVector1.front());
        newDisplaySize1 += 1;
    }
    if (isBoundary2Loop) {
        newBoundaryVerticesVector2.push_back(newBoundaryVerticesVector2.front());
        newDisplaySize2 += 1;
    }
    if (boundariesAreCombinedLoop) {
        if (boundariesReversed) {
            newBoundaryVerticesVector1.push_back(newBoundaryVerticesVector2.front());
            newBoundaryVerticesVector2.push_back(newBoundaryVerticesVector1.front());
        } else {
            newBoundaryVerticesVector1.push_back(newBoundaryVerticesVector2.back());
            newBoundaryVerticesVector2.insert(newBoundaryVerticesVector2.begin(), newBoundaryVerticesVector1.front());
        }
        newDisplaySize1 += 1;
        newDisplaySize2 += 1;
    }

    numBoundaryVertices1 = newBoundaryVerticesVector1.size();
    numBoundaryVertices2 = newBoundaryVerticesVector2.size();

    for (Vertex* boundaryVertex : newBoundaryVerticesVector1) {
        positions1.push_back(boundaryVertex->position);
    }

    for (Vertex* boundaryVertex : newBoundaryVerticesVector2) {
        positions2.push_back(boundaryVertex->position);
    }

    std::vector<QVector3D> colors1 = initBoundaryColors(numBoundaryVertices1, newDisplaySize1);
    std::vector<QVector3D> colors2 = initBoundaryColors(numBoundaryVertices2, newDisplaySize2);

    // Transfer position data to VBO 0
    boundaryArrayBuf1.bind();
    boundaryArrayBuf1.allocate(&positions1[0], numBoundaryVertices1 * sizeof(QVector3D));

    // Transfer color scale data to VBO 0
    boundaryColorBuf1.bind();
    boundaryColorBuf1.allocate(&colors1[0], numBoundaryVertices1 * sizeof(QVector3D));

    // Transfer position data to VBO 0
    boundaryArrayBuf2.bind();
    boundaryArrayBuf2.allocate(&positions2[0], numBoundaryVertices2 * sizeof(QVector3D));

    // Transfer color scale data to VBO 0
    boundaryColorBuf2.bind();
    boundaryColorBuf2.allocate(&colors2[0], numBoundaryVertices2 * sizeof(QVector3D));
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
    boundaryArrayBuf2.bind();

    // Tell OpenGL programmable pipeline how to locate position data
    int positionLocation2 = program->attributeLocation("a_position");
    program->enableAttributeArray(positionLocation2);
    program->setAttributeBuffer(positionLocation2, GL_FLOAT, 0, 3, sizeof(QVector3D));

    // Tell OpenGL which VBOs to use
    boundaryColorBuf2.bind();

    // Tell OpenGL programmable pipeline how to locate color data
    int inputBoundaryColorLocation2 = program->attributeLocation("input_color");
    program->enableAttributeArray(inputBoundaryColorLocation2);
    program->setAttributeBuffer(inputBoundaryColorLocation2, GL_FLOAT, 0, 3, sizeof(QVector3D));

    // Draw geometry using indices from VBO 1
    glLineWidth(10.0);
    glDrawArrays(GL_LINE_STRIP, 0, numBoundaryVertices2);

    // Tell OpenGL which VBOs to use
    boundaryArrayBuf1.bind();

    // Tell OpenGL programmable pipeline how to locate position data
    int positionLocation1 = program->attributeLocation("a_position");
    program->enableAttributeArray(positionLocation1);
    program->setAttributeBuffer(positionLocation1, GL_FLOAT, 0, 3, sizeof(QVector3D));

    // Tell OpenGL which VBOs to use
    boundaryColorBuf1.bind();

    // Tell OpenGL programmable pipeline how to locate color data
    int inputBoundaryColorLocation1 = program->attributeLocation("input_color");
    program->enableAttributeArray(inputBoundaryColorLocation1);
    program->setAttributeBuffer(inputBoundaryColorLocation1, GL_FLOAT, 0, 3, sizeof(QVector3D));

    // Draw geometry using indices from VBO 1
    glLineWidth(10.0);
    glDrawArrays(GL_LINE_STRIP, 0, numBoundaryVertices1);
}
