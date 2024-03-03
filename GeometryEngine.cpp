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

void GeometryEngine::initBoundary(std::vector<Vertex*> boundaryVertices1, std::vector<Vertex*> boundaryVertices2, int displaySize1, int displaySize2, bool isBoundary1Loop, bool isBoundary2Loop, bool boundariesAreCombinedLoop, bool boundariesReversed) {
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


    /*
    std::vector<QVector3D> arrowPositions;
    std::vector<QVector3D> arrowColors;
    std::vector<GLushort> arrowIndices;

    if (!newBoundaryVertices2.empty()) {
        float arrowDelta = 0.5;
        QVector3D positions2Mid = positions2[numBoundaryVertices2 / 2];
        QVector3D positions2PreviousMid = positions2[(numBoundaryVertices2 / 2) - 1];
        QVector3D midVector2 = (positions2PreviousMid - positions2Mid).normalized();
        QVector3D arrowBase = positions2Mid + arrowDelta * midVector2;
        QVector3D arrowBaseNormal1 = QVector3D::crossProduct(midVector2, {1, 0, 0}).normalized();
        QVector3D arrowCorner1 = arrowBase + arrowDelta * arrowBaseNormal1;

        //QMatrix4x4 matrix;
        //matrix.translate(arrowBase);
        //matrix.rotate(180.0, arrowBaseNormal);
        //matrix.translate(-arrowBase);
        //QVector3D arrowCorner2 = matrix * arrowCorner1;
        QVector3D arrowBaseNormal2 = QVector3D::crossProduct(midVector2, {-1, 0, 0}).normalized();
        QVector3D arrowCorner2 = arrowBase + arrowDelta * arrowBaseNormal2;

        std::cout << "positions2Mid: x: " << positions2Mid.x()  << ", y: " << positions2Mid.y() << ", z: " << positions2Mid.z() << std::endl;
        std::cout << "positions2PreviousMid: x: " << positions2PreviousMid.x()  << ", y: " << positions2PreviousMid.y() << ", z: " << positions2PreviousMid.z() << std::endl;
        std::cout << "midVector2: x: " << midVector2.x()  << ", y: " << midVector2.y() << ", z: " << midVector2.z() << std::endl;
        std::cout << "arrowBase: x: " << arrowBase.x()  << ", y: " << arrowBase.y() << ", z: " << arrowBase.z() << std::endl;
        std::cout << "arrowCorner1: x: " << arrowCorner1.x()  << ", y: " << arrowCorner1.y() << ", z: " << arrowCorner1.z() << std::endl;
        std::cout << "arrowCorner2: x: " << arrowCorner2.x()  << ", y: " << arrowCorner2.y() << ", z: " << arrowCorner2.z() << std::endl;

        //arrowPositions.push_back(positions2Mid);
        //arrowPositions.push_back(arrowBase);
        //arrowPositions.push_back(arrowCorner1);
        //arrowPositions.push_back(arrowCorner2);

        arrowPositions.push_back({1.5, 1.5, 1.5});
        arrowPositions.push_back({-1.5, 1.5, 1.5});
        arrowPositions.push_back({-1.5, 1.5, -1.5});
        arrowPositions.push_back({1.5, 1.5, -1.5});

        arrowColors.push_back(colors2[numBoundaryVertices2 / 2]);
        arrowColors.push_back(colors2[numBoundaryVertices2 / 2]);
        arrowColors.push_back(colors2[numBoundaryVertices2 / 2]);
        arrowColors.push_back(colors2[numBoundaryVertices2 / 2]);

        arrowIndices.push_back(0);
        arrowIndices.push_back(1);
        arrowIndices.push_back(2);
        arrowIndices.push_back(0);
        arrowIndices.push_back(1);
        arrowIndices.push_back(3);
    }

    numArrowIndices = arrowIndices.size();
    int numArrowVertices = arrowPositions.size();
    int numArrowColors = arrowColors.size();

    // Transfer position data to VBO 0
    arrowArrayBuf.bind();
    arrowArrayBuf.allocate(&arrowPositions[0], numArrowVertices * sizeof(QVector3D));

    // Transfer color data to VBO 0
    arrowColorBuf.bind();
    arrowColorBuf.allocate(&arrowColors[0], numArrowColors * sizeof(QVector3D));

    // Transfer index data to VBO 1
    arrowIndexBuf.bind();
    arrowIndexBuf.allocate(&arrowIndices[0], numArrowIndices * sizeof(GLushort));
     */
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



    // Tell OpenGL which VBOs to use
    arrowArrayBuf.bind();
    arrowIndexBuf.bind();

    // Tell OpenGL programmable pipeline how to locate position data
    //int arrowPositionLocation = program->attributeLocation("a_position");
    //program->enableAttributeArray(arrowPositionLocation);
    //program->setAttributeBuffer(arrowPositionLocation, GL_FLOAT, 0, 3, sizeof(QVector3D));

    // Tell OpenGL which VBOs to use
    //arrowColorBuf.bind();

    // Tell OpenGL programmable pipeline how to locate color data
    //int arrowInputColorLocation = program->attributeLocation("input_color");
    //program->enableAttributeArray(arrowInputColorLocation);
    //program->setAttributeBuffer(arrowInputColorLocation, GL_FLOAT, 0, 3, sizeof(QVector3D));

    // Draw geometry using indices from VBO 1
    //glDrawElements(GL_TRIANGLES, numArrowIndices, GL_UNSIGNED_SHORT, nullptr);
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
