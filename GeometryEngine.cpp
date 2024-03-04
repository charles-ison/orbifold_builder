#include <QtGui/qcolor.h>
#include "GeometryEngine.h"

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
    arrowArrayBuf.create();
    arrowColorBuf.create();
    arrowIndexBuf.create();
    pointToDeleteColorBuf.create();
    pointToDeleteArrayBuf.create();
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
    arrowArrayBuf.destroy();
    arrowColorBuf.destroy();
    arrowIndexBuf.destroy();
    pointToDeleteColorBuf.destroy();
    pointToDeleteArrayBuf.destroy();
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

    // Transfer color data to VBO 0
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

    // Transfer color data to VBO 0
    lineColorBuf.bind();
    lineColorBuf.allocate(&colors[0], numLineVertices * sizeof(QVector3D));
}

void GeometryEngine::initPointToDelete(std::vector<Vertex*> verticesToDelete) {
    std::vector<QVector3D> positions;
    std::vector<QVector3D> colors;
    numPointsToDeleteVertices = verticesToDelete.size();

    for (Vertex* vertexToDelete : verticesToDelete) {
        positions.push_back(vertexToDelete->position);
        colors.push_back({1.0, 1.0, 1.0});
    }

    // Transfer position data to VBO 0
    pointToDeleteArrayBuf.bind();
    pointToDeleteArrayBuf.allocate(&positions[0], sizeof(QVector3D));

    // Transfer color data to VBO 0
    pointToDeleteColorBuf.bind();
    lineColorBuf.allocate(&colors[0], sizeof(QVector3D));
}

std::vector<QVector3D> GeometryEngine::initBoundaryColors(int numVertices, int displaySize) {
    std::vector<QVector3D> colors;
    for (int i=0; i<numVertices; i++) {
        float boundaryColorScale = float(i % displaySize)/float(displaySize-1);
        colors.push_back({boundaryColorScale, boundaryColorScale, boundaryColorScale});
    }
    return colors;
}

void GeometryEngine::initBoundary(std::vector<Vertex*> boundaryVertices1, std::vector<Vertex*> boundaryVertices2, int displaySize1, int displaySize2, bool isBoundary1Loop, bool isBoundary2Loop, bool boundariesAreCombinedLoop, bool boundariesReversed, bool boundariesOverlapping) {
    std::vector<QVector3D> positions1;
    std::vector<QVector3D> positions2;
    std::vector<Vertex*> newBoundaryVertices1 = boundaryVertices1;
    std::vector<Vertex*> newBoundaryVertices2 = boundaryVertices2;
    int newDisplaySize1 = displaySize1;
    int newDisplaySize2 = displaySize2;

    if (isBoundary1Loop) {
        newBoundaryVertices1.push_back(newBoundaryVertices1.front());
        newDisplaySize1 += 1;
    }
    if (isBoundary2Loop) {
        newBoundaryVertices2.push_back(newBoundaryVertices2.front());
        newDisplaySize2 += 1;
    }
    if (boundariesAreCombinedLoop) {
        if (boundariesReversed) {
            newBoundaryVertices1.push_back(newBoundaryVertices2.front());
            newBoundaryVertices2.push_back(newBoundaryVertices1.front());
        } else {
            newBoundaryVertices1.push_back(newBoundaryVertices2.back());
            newBoundaryVertices2.insert(newBoundaryVertices2.begin(), newBoundaryVertices1.front());
        }
        newDisplaySize1 += 1;
        newDisplaySize2 += 1;
    }

    numBoundaryVertices1 = newBoundaryVertices1.size();
    numBoundaryVertices2 = newBoundaryVertices2.size();

    for (Vertex* boundaryVertex : newBoundaryVertices1) {
        positions1.push_back(boundaryVertex->position);
    }

    for (Vertex* boundaryVertex : newBoundaryVertices2) {
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

    initArrows(positions1, colors1, positions2, colors2, boundariesOverlapping, boundariesReversed);
}

std::vector<QVector3D> GeometryEngine::buildArrow(std::vector<QVector3D> positions, int startIndex) {
    std::vector<QVector3D> arrowPositions;
    float arrowDelta = 0.05;
    QVector3D positionsStart = positions[startIndex];
    QVector3D positionsPrevious = positions[startIndex - 1];
    QVector3D positionsMid = (positionsStart + positionsPrevious)/2;
    QVector3D midVector = (positionsPrevious - positionsStart).normalized();
    QVector3D midVectorForCross = {midVector.x()+1, midVector.y()-1, midVector.z()+1};
    QVector3D arrowBase = positionsMid + arrowDelta * midVector;
    QVector3D arrowBaseNormal = QVector3D::crossProduct(midVector, midVectorForCross).normalized();
    QVector3D arrowCorner = arrowBase + 0.35 * arrowDelta * arrowBaseNormal;

    arrowPositions.push_back(positionsMid);
    arrowPositions.push_back(arrowCorner);

    int numArrowCorners = 64;
    for (int i=1; i<numArrowCorners + 1; i++) {
        QMatrix4x4 matrix;
        matrix.translate(arrowBase);
        matrix.rotate(i*(360.0/numArrowCorners), midVector);
        matrix.translate(-arrowBase);
        QVector3D newArrowCorner = matrix.map(arrowCorner);
        arrowPositions.push_back(positionsMid);
        arrowPositions.push_back(newArrowCorner);
    }

    return arrowPositions;
}

void GeometryEngine::initArrows(std::vector<QVector3D> positions1, std::vector<QVector3D> colors1, std::vector<QVector3D> positions2, std::vector<QVector3D> colors2, bool boundariesOverlapping, bool boundariesReversed) {
    std::vector<QVector3D> arrowPositions;
    std::vector<QVector3D> arrowColors;

    if (!positions1.empty()) {
        int startIndex1;
        if (boundariesOverlapping && boundariesReversed) {
            startIndex1 = (1.0 / 3.0) * positions1.size();
        } else {
            startIndex1 = (2.0 / 3.0) * positions1.size();
        }
        QVector3D arrowColor1 = colors1[(2.0 / 3.0) * positions1.size()];
        std::vector<QVector3D> arrowPositions1 = buildArrow(positions1, startIndex1);
        for (QVector3D arrowPosition : arrowPositions1) {
            arrowPositions.push_back(arrowPosition);
            arrowColors.push_back(arrowColor1);
        }
    }

    if (!positions2.empty()) {
        float startIndex2 = (1.0/3.0) * positions2.size();
        QVector3D arrowColor2 = colors2[startIndex2];
        std::vector<QVector3D> arrowPositions2 = buildArrow(positions2, startIndex2);
        for (QVector3D arrowPosition : arrowPositions2) {
            arrowPositions.push_back(arrowPosition);
            arrowColors.push_back(arrowColor2);
        }
    }

    numArrowVertices = arrowPositions.size();

    // Transfer position data to VBO 0
    arrowArrayBuf.bind();
    arrowArrayBuf.allocate(&arrowPositions[0], numArrowVertices * sizeof(QVector3D));

    // Transfer color data to VBO 0
    arrowColorBuf.bind();
    arrowColorBuf.allocate(&arrowColors[0], numArrowVertices * sizeof(QVector3D));
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
    int inputColorLocation = program->attributeLocation("input_color");
    program->enableAttributeArray(inputColorLocation);
    program->setAttributeBuffer(inputColorLocation, GL_FLOAT, 0, 3, sizeof(QVector3D));

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
    int inputColorLocation = program->attributeLocation("input_color");
    program->enableAttributeArray(inputColorLocation);
    program->setAttributeBuffer(inputColorLocation, GL_FLOAT, 0, 3, sizeof(QVector3D));

    // Draw geometry using indices from VBO 1
    glLineWidth(10.0);
    glDrawArrays(GL_LINE_STRIP, 0, numLineVertices);
}

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
    int inputColorLocation2 = program->attributeLocation("input_color");
    program->enableAttributeArray(inputColorLocation2);
    program->setAttributeBuffer(inputColorLocation2, GL_FLOAT, 0, 3, sizeof(QVector3D));

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
    int inputColorLocation1 = program->attributeLocation("input_color");
    program->enableAttributeArray(inputColorLocation1);
    program->setAttributeBuffer(inputColorLocation1, GL_FLOAT, 0, 3, sizeof(QVector3D));

    // Draw geometry using indices from VBO 1
    glLineWidth(10.0);
    glDrawArrays(GL_LINE_STRIP, 0, numBoundaryVertices1);

    drawArrows(program);
}

void GeometryEngine::drawArrows(QOpenGLShaderProgram *program) {
    // Tell OpenGL which VBOs to use
    arrowArrayBuf.bind();

    // Tell OpenGL programmable pipeline how to locate position data
    int positionLocation = program->attributeLocation("a_position");
    program->enableAttributeArray(positionLocation);
    program->setAttributeBuffer(positionLocation, GL_FLOAT, 0, 3, sizeof(QVector3D));

    // Tell OpenGL which VBOs to use
    arrowColorBuf.bind();

    // Tell OpenGL programmable pipeline how to locate color data
    int inputColorLocation = program->attributeLocation("input_color");
    program->enableAttributeArray(inputColorLocation);
    program->setAttributeBuffer(inputColorLocation, GL_FLOAT, 0, 3, sizeof(QVector3D));

    // Draw geometry using indices from VBO 1
    glLineWidth(5.0);
    glDrawArrays(GL_LINES, 0, numArrowVertices);
}

void GeometryEngine::drawPointToDelete(QOpenGLShaderProgram *program) {
    // Tell OpenGL which VBOs to use
    pointToDeleteArrayBuf.bind();

    // Tell OpenGL programmable pipeline how to locate position data
    int positionLocation = program->attributeLocation("a_position");
    program->enableAttributeArray(positionLocation);
    program->setAttributeBuffer(positionLocation, GL_FLOAT, 0, 3, sizeof(QVector3D));

    // Tell OpenGL which VBOs to use
    pointToDeleteColorBuf.bind();

    // Tell OpenGL programmable pipeline how to locate color data
    int inputBoundaryColorLocation = program->attributeLocation("input_color");
    program->enableAttributeArray(inputBoundaryColorLocation);
    program->setAttributeBuffer(inputBoundaryColorLocation, GL_FLOAT, 0, 3, sizeof(QVector3D));

    // Draw geometry using indices from VBO 1
    glPointSize(15.0);
    glDrawArrays(GL_POINTS, 0, numPointsToDeleteVertices);
}
