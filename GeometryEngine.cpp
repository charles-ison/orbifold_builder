#include <QtGui/qcolor.h>
#include "GeometryEngine.h"
#include <queue>
#include <iostream>

GeometryEngine::GeometryEngine() : indexBuf(QOpenGLBuffer::IndexBuffer) {
    initializeOpenGLFunctions();

    // Generate VBOs
    arrayBuf.create();
    indexBuf.create();
    colorBuf.create();
    lineArrayBuf.create();
    lineColorBuf.create();
    generatorsArrayBuf.create();
    generatorsColorBuf.create();
    boundaryArrayBuf1.create();
    boundaryColorBuf1.create();
    boundaryArrayBuf2.create();
    boundaryColorBuf2.create();
    arrowArrayBuf.create();
    arrowColorBuf.create();
    arrowIndexBuf.create();
    pointColorBuf.create();
    pointArrayBuf.create();
    normalBuf.create();
    lineNormalBuf.create();
    generatorsNormalBuf.create();
    pointNormalBuf.create();
    boundaryNormalBuf1.create();
    boundaryNormalBuf2.create();
    arrowNormalBuf.create();
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
    pointColorBuf.destroy();
    pointArrayBuf.destroy();
    normalBuf.destroy();
    lineNormalBuf.destroy();
    pointNormalBuf.destroy();
    boundaryNormalBuf1.destroy();
    boundaryNormalBuf2.destroy();
    arrowNormalBuf.destroy();
}

void GeometryEngine::initMesh(Mesh* mesh) {
    std::vector<QVector3D> positions;
    std::vector<QVector3D> colors;
    std::vector<QVector3D> normals;
    std::vector<QVector3D> oppositeNormals;
    std::vector<Vertex*> meshVertices = mesh->getVertices();
    int numVertices = 2*meshVertices.size();

    for (Vertex* vertexPointer : meshVertices) {
        positions.push_back(vertexPointer->position);
        normals.push_back(vertexPointer->normal);
        colors.push_back({0.0, 0.87, 0.87});
    }
    for (Vertex* vertexPointer : meshVertices) {
        positions.push_back(vertexPointer->position);
        normals.push_back(-vertexPointer->normal);
        colors.push_back({0.0, 0.87, 0.87});
    }

    // Transfer position data to VBO 0
    arrayBuf.bind();
    arrayBuf.allocate(&positions[0], numVertices * sizeof(QVector3D));

    // Transfer color data to VBO 0
    colorBuf.bind();
    colorBuf.allocate(&colors[0], numVertices * sizeof(QVector3D));

    // Transfer normal data to VBO 0
    normalBuf.bind();
    normalBuf.allocate(&normals[0], numVertices * sizeof(QVector3D));

    std::vector<GLushort> indices;
    std::vector<Triangle*> triangles = mesh->getTriangles();
    int numTriangles = triangles.size();
    numIndices = 2 * 3 * numTriangles;

    std::unordered_map<Triangle*, std::unordered_set<Vertex*>> problemTriangleVertices;
    for (int i=0; i<meshVertices.size(); i++) {
        Vertex* vertex = meshVertices[i];
        int orientationFlag = 1;
        std::unordered_set<std::string> oldEdges;
        for (Triangle* triangle: mesh->getOrderedTriangles(vertex)) {
            Vertex *vertex0 = triangle->vertices[0];
            Vertex *vertex1 = triangle->vertices[1];
            Vertex *vertex2 = triangle->vertices[2];
            std::string edge0 = std::to_string(vertex0->index) + "-" + std::to_string(vertex1->index);
            std::string edge1 = std::to_string(vertex1->index) + "-" + std::to_string(vertex2->index);
            std::string edge2 = std::to_string(vertex2->index) + "-" + std::to_string(vertex0->index);
            bool edge0Found = oldEdges.find(edge0) != oldEdges.end();
            bool edge1Found = oldEdges.find(edge1) != oldEdges.end();
            bool edge2Found = oldEdges.find(edge2) != oldEdges.end();
            if (edge0Found || edge1Found || edge2Found) {
                orientationFlag *= -1;
            }
            oldEdges.clear();
            oldEdges.insert(edge0);
            oldEdges.insert(edge1);
            oldEdges.insert(edge2);

            if (orientationFlag == -1) {
                if (problemTriangleVertices.find(triangle) == problemTriangleVertices.end()) {
                    std::unordered_set<Vertex*> newProblemVertices = {vertex};
                    problemTriangleVertices.insert({triangle, newProblemVertices});
                } else {
                    problemTriangleVertices.at(triangle).insert(vertex);
                }
            }
        }
    }

    for (int i=0; i<numTriangles; i++) {
        if (problemTriangleVertices.find(triangles[i]) == problemTriangleVertices.end()) {
            int index0 = triangles[i]->vertices[0]->index;
            int index1 = triangles[i]->vertices[1]->index;
            int index2 = triangles[i]->vertices[2]->index;
            indices.push_back(index0);
            indices.push_back(index1);
            indices.push_back(index2);
            indices.push_back(meshVertices.size() + index2);
            indices.push_back(meshVertices.size() + index1);
            indices.push_back(meshVertices.size() + index0);
        } else {
            std::vector<int> indices1;
            std::vector<int> indices2;
            std::unordered_set problemVertices = problemTriangleVertices.at(triangles[i]);
            for (Vertex* vertex : triangles[i]->vertices) {
                if (problemVertices.find(vertex) == problemVertices.end()) {
                    indices1.push_back(vertex->index);
                    indices2.push_back(meshVertices.size() + vertex->index);
                } else {
                    indices1.push_back(meshVertices.size() + vertex->index);
                    indices2.push_back(vertex->index);
                }
            }
            for (int index : indices1) {
                indices.push_back(index);
            }
            std::reverse(indices2.begin(), indices2.end());
            for (int index : indices2) {
                indices.push_back(index);
            }
        }
    }

    // Transfer index data to VBO 1
    indexBuf.bind();
    indexBuf.allocate(&indices[0], numIndices * sizeof(GLushort));
}

void GeometryEngine::initLine(std::vector<Vertex*> lineVerticesVector, QColor color) {
    std::vector<QVector3D> positions;
    std::vector<QVector3D> colors;
    std::vector<QVector3D> normals;
    numLineVertices = lineVerticesVector.size();

    QRgb rgb = color.rgb();
    QVector3D vecColor = {(float) qRed(rgb), (float) qGreen(rgb), (float) qBlue(rgb)};

    for (Vertex* lineVertex : lineVerticesVector) {
        positions.push_back(lineVertex->position);
        colors.push_back(vecColor);
        normals.push_back(lineVertex->normal);
    }

    // Transfer position data to VBO 0
    lineArrayBuf.bind();
    lineArrayBuf.allocate(&positions[0], numLineVertices * sizeof(QVector3D));

    // Transfer color data to VBO 0
    lineColorBuf.bind();
    lineColorBuf.allocate(&colors[0], numLineVertices * sizeof(QVector3D));

    // Transfer normal data to VBO 0
    lineNormalBuf.bind();
    lineNormalBuf.allocate(&normals[0], numLineVertices * sizeof(QVector3D));
}

void GeometryEngine::initGenerators(std::vector<Vertex*> generatorsVerticesVector, QColor color) {
    std::vector<QVector3D> positions;
    std::vector<QVector3D> colors;
    std::vector<QVector3D> normals;
    numGeneratorVertices = generatorsVerticesVector.size();

    QRgb rgb = color.rgb();
    QVector3D vecColor = {(float) qRed(rgb), (float) qGreen(rgb), (float) qBlue(rgb)};

    for (Vertex* generatorVertex : generatorsVerticesVector) {
        positions.push_back(generatorVertex->position);
        colors.push_back(vecColor);
        normals.push_back(generatorVertex->normal);
    }

    // Transfer position data to VBO 0
    generatorsArrayBuf.bind();
    generatorsArrayBuf.allocate(&positions[0], numGeneratorVertices * sizeof(QVector3D));

    // Transfer color data to VBO 0
    generatorsColorBuf.bind();
    generatorsColorBuf.allocate(&colors[0], numGeneratorVertices * sizeof(QVector3D));

    // Transfer normal data to VBO 0
    generatorsNormalBuf.bind();
    generatorsNormalBuf.allocate(&normals[0], numGeneratorVertices * sizeof(QVector3D));
}

void GeometryEngine::initPoints(std::vector<Vertex*> vertices) {
    std::vector<QVector3D> positions;
    std::vector<QVector3D> colors;
    std::vector<QVector3D> normals;
    numPointVertices = vertices.size();

    for (Vertex* vertex : vertices) {
        positions.push_back(vertex->position);
        normals.push_back(vertex->normal);
        colors.push_back({1.0, 1.0, 1.0});
    }

    // Transfer position data to VBO 0
    pointArrayBuf.bind();
    pointArrayBuf.allocate(&positions[0], sizeof(QVector3D));

    // Transfer color data to VBO 0
    pointColorBuf.bind();
    lineColorBuf.allocate(&colors[0], sizeof(QVector3D));

    // Transfer normal data to VBO 0
    pointNormalBuf.bind();
    pointNormalBuf.allocate(&normals[0], sizeof(QVector3D));
}

std::vector<QVector3D> GeometryEngine::initBoundaryColors(int numVertices, int displaySize) {
    std::vector<QVector3D> colors;
    for (int i=0; i<numVertices; i++) {
        float boundaryColorScale = float(i % displaySize)/float(displaySize-1);
        colors.push_back({boundaryColorScale, boundaryColorScale, boundaryColorScale});
    }
    return colors;
}

void GeometryEngine::initBoundary(std::vector<Vertex*> boundaryVertices1, std::vector<Vertex*> boundaryVertices2, int displaySize1, int displaySize2, bool isBoundary1Loop, bool isBoundary2Loop, bool boundariesAreCombinedLoop, bool boundariesReversed, bool boundariesOverlapping, int numOpenings) {
    std::vector<QVector3D> positions1;
    std::vector<QVector3D> positions2;
    std::vector<QVector3D> normals1;
    std::vector<QVector3D> normals2;
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

    if (numOpenings == 2 && !isBoundary2Loop && boundariesReversed) {
        std::reverse(newBoundaryVertices2.begin(), newBoundaryVertices2.begin() + newBoundaryVertices2.size() / 2);
        std::reverse(newBoundaryVertices2.begin() + newBoundaryVertices2.size() / 2, newBoundaryVertices2.end());
    }

    numBoundaryVertices1 = newBoundaryVertices1.size();
    numBoundaryVertices2 = newBoundaryVertices2.size();

    for (Vertex* boundaryVertex : newBoundaryVertices1) {
        positions1.push_back(boundaryVertex->position);
        normals1.push_back(boundaryVertex->normal);
    }

    for (Vertex* boundaryVertex : newBoundaryVertices2) {
        positions2.push_back(boundaryVertex->position);
        normals2.push_back(boundaryVertex->normal);
    }

    std::vector<QVector3D> colors1 = initBoundaryColors(numBoundaryVertices1, newDisplaySize1);
    std::vector<QVector3D> colors2 = initBoundaryColors(numBoundaryVertices2, newDisplaySize2);

    // Transfer position data to VBO 0
    boundaryArrayBuf1.bind();
    boundaryArrayBuf1.allocate(&positions1[0], numBoundaryVertices1 * sizeof(QVector3D));

    // Transfer color scale data to VBO 0
    boundaryColorBuf1.bind();
    boundaryColorBuf1.allocate(&colors1[0], numBoundaryVertices1 * sizeof(QVector3D));

    // Transfer normal data to VBO 0
    boundaryNormalBuf1.bind();
    boundaryNormalBuf1.allocate(&normals1[0], numBoundaryVertices1 * sizeof(QVector3D));

    // Transfer position data to VBO 0
    boundaryArrayBuf2.bind();
    boundaryArrayBuf2.allocate(&positions2[0], numBoundaryVertices2 * sizeof(QVector3D));

    // Transfer color scale data to VBO 0
    boundaryColorBuf2.bind();
    boundaryColorBuf2.allocate(&colors2[0], numBoundaryVertices2 * sizeof(QVector3D));

    // Transfer normal data to VBO 0
    boundaryNormalBuf2.bind();
    boundaryNormalBuf2.allocate(&normals2[0], numBoundaryVertices2 * sizeof(QVector3D));

    initArrows(positions1, colors1, positions2, colors2, boundariesOverlapping, boundariesReversed, numOpenings);
}

std::vector<Vertex> GeometryEngine::buildArrow(std::vector<QVector3D> positions, int startIndex) {
    std::vector<Vertex> arrowVertices;
    float arrowDelta = 0.05;
    QVector3D positionsStart = positions[startIndex];
    QVector3D positionsPrevious = positions[startIndex - 1];
    QVector3D positionsMid = (positionsStart + positionsPrevious)/2;
    QVector3D midVector = (positionsPrevious - positionsStart).normalized();
    QVector3D midVectorForCross = {midVector.x()+1, midVector.y()-1, midVector.z()+1};
    QVector3D arrowBase = positionsMid + arrowDelta * midVector;
    QVector3D arrowBaseNormal = QVector3D::crossProduct(midVector, midVectorForCross).normalized();
    QVector3D arrowCorner = arrowBase + 0.35 * arrowDelta * arrowBaseNormal;

    Vertex midVertex = {.position = positionsMid, .normal = midVector};
    Vertex cornerVertex = {.position = arrowCorner, .normal = arrowBaseNormal};
    arrowVertices.push_back(midVertex);
    arrowVertices.push_back(cornerVertex);

    int numArrowCorners = 64;
    for (int i=1; i<numArrowCorners + 1; i++) {
        QMatrix4x4 matrix;
        matrix.translate(arrowBase);
        matrix.rotate(i*(360.0/numArrowCorners), midVector);
        matrix.translate(-arrowBase);
        QVector3D newArrowCorner = matrix.map(arrowCorner);
        Vertex newArrowCornerVertex = {.position = newArrowCorner, .normal = arrowBaseNormal};
        arrowVertices.push_back(midVertex);
        arrowVertices.push_back(newArrowCornerVertex);
    }

    return arrowVertices;
}

void GeometryEngine::initArrows(std::vector<QVector3D> positions1, std::vector<QVector3D> colors1, std::vector<QVector3D> positions2, std::vector<QVector3D> colors2, bool boundariesOverlapping, bool boundariesReversed, int numOpenings) {
    std::vector<QVector3D> arrowPositions;
    std::vector<QVector3D> arrowNormals;
    std::vector<QVector3D> arrowColors;

    if (!positions1.empty()) {
        int startIndex1;
        QVector3D arrowColor1;
        if ((boundariesOverlapping && boundariesReversed)) {
            startIndex1 = (1.0 / 3.0) * positions1.size();
            arrowColor1 = colors1[(2.0 / 3.0) * positions1.size()];
        } else if (numOpenings == 2) {
            startIndex1 = (1.0 / 3.0) * positions1.size();
            arrowColor1 = colors1[(1.0 / 3.0) * positions1.size()];
        }
        else {
            startIndex1 = (2.0 / 3.0) * positions1.size();
            arrowColor1 = colors1[(2.0 / 3.0) * positions1.size()];
        }
        std::vector<Vertex> arrowVertices1 = buildArrow(positions1, startIndex1);
        for (Vertex arrowVertex : arrowVertices1) {
            arrowPositions.push_back(arrowVertex.position);
            arrowColors.push_back(arrowColor1);
            arrowNormals.push_back(arrowVertex.normal);
        }
    }

    if (!positions2.empty()) {
        float startIndex2 = (1.0/3.0) * positions2.size();
        QVector3D arrowColor2 = colors2[startIndex2];
        std::vector<Vertex> arrowVertices2 = buildArrow(positions2, startIndex2);
        for (Vertex arrowVertex : arrowVertices2) {
            arrowPositions.push_back(arrowVertex.position);
            arrowColors.push_back(arrowColor2);
            arrowNormals.push_back(arrowVertex.normal);
        }
    }

    numArrowVertices = arrowPositions.size();

    // Transfer position data to VBO 0
    arrowArrayBuf.bind();
    arrowArrayBuf.allocate(&arrowPositions[0], numArrowVertices * sizeof(QVector3D));

    // Transfer color data to VBO 0
    arrowColorBuf.bind();
    arrowColorBuf.allocate(&arrowColors[0], numArrowVertices * sizeof(QVector3D));

    // Transfer normal data to VBO 0
    arrowNormalBuf.bind();
    arrowNormalBuf.allocate(&arrowNormals[0], numArrowVertices * sizeof(QVector3D));
}

void GeometryEngine::drawMesh(QOpenGLShaderProgram *program) {
    // Tell OpenGL which VBOs to use
    arrayBuf.bind();
    indexBuf.bind();

    // Tell OpenGL programmable pipeline how to locate position data
    int positionLocation = program->attributeLocation("input_position");
    program->enableAttributeArray(positionLocation);
    program->setAttributeBuffer(positionLocation, GL_FLOAT, 0, 3, sizeof(QVector3D));

    // Tell OpenGL which VBOs to use
    colorBuf.bind();

    // Tell OpenGL programmable pipeline how to locate color data
    int colorLocation = program->attributeLocation("input_color");
    program->enableAttributeArray(colorLocation);
    program->setAttributeBuffer(colorLocation, GL_FLOAT, 0, 3, sizeof(QVector3D));

    // Tell OpenGL which VBOs to use
    normalBuf.bind();

    // Tell OpenGL programmable pipeline how to locate color data
    int normalLocation = program->attributeLocation("input_normal");
    program->enableAttributeArray(normalLocation);
    program->setAttributeBuffer(normalLocation, GL_FLOAT, 0, 3, sizeof(QVector3D));

    // Draw geometry using indices from VBO 1
    glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, nullptr);
}

void GeometryEngine::drawLine(QOpenGLShaderProgram *program) {
    // Tell OpenGL which VBOs to use
    lineArrayBuf.bind();

    // Tell OpenGL programmable pipeline how to locate position data
    int positionLocation = program->attributeLocation("input_position");
    program->enableAttributeArray(positionLocation);
    program->setAttributeBuffer(positionLocation, GL_FLOAT, 0, 3, sizeof(QVector3D));

    // Tell OpenGL which VBOs to use
    lineColorBuf.bind();

    // Tell OpenGL programmable pipeline how to locate color data
    int colorLocation = program->attributeLocation("input_color");
    program->enableAttributeArray(colorLocation);
    program->setAttributeBuffer(colorLocation, GL_FLOAT, 0, 3, sizeof(QVector3D));

    // Tell OpenGL which VBOs to use
    lineNormalBuf.bind();

    // Tell OpenGL programmable pipeline how to locate color data
    int normalLocation = program->attributeLocation("input_normal");
    program->enableAttributeArray(normalLocation);
    program->setAttributeBuffer(normalLocation, GL_FLOAT, 0, 3, sizeof(QVector3D));

    // Draw geometry using indices from VBO 1
    glLineWidth(10.0);
    glDrawArrays(GL_LINE_STRIP, 0, numLineVertices);
}

void GeometryEngine::drawGenerators(QOpenGLShaderProgram *program) {
    // Tell OpenGL which VBOs to use
    generatorsArrayBuf.bind();

    // Tell OpenGL programmable pipeline how to locate position data
    int positionLocation = program->attributeLocation("input_position");
    program->enableAttributeArray(positionLocation);
    program->setAttributeBuffer(positionLocation, GL_FLOAT, 0, 3, sizeof(QVector3D));

    // Tell OpenGL which VBOs to use
    generatorsColorBuf.bind();

    // Tell OpenGL programmable pipeline how to locate color data
    int colorLocation = program->attributeLocation("input_color");
    program->enableAttributeArray(colorLocation);
    program->setAttributeBuffer(colorLocation, GL_FLOAT, 0, 3, sizeof(QVector3D));

    // Tell OpenGL which VBOs to use
    generatorsNormalBuf.bind();

    // Tell OpenGL programmable pipeline how to locate color data
    int normalLocation = program->attributeLocation("input_normal");
    program->enableAttributeArray(normalLocation);
    program->setAttributeBuffer(normalLocation, GL_FLOAT, 0, 3, sizeof(QVector3D));

    // Draw geometry using indices from VBO 1
    glLineWidth(10.0);
    glDrawArrays(GL_LINES, 0, numGeneratorVertices);
}

void GeometryEngine::drawBoundary(QOpenGLShaderProgram *program) {
    // Tell OpenGL which VBOs to use
    boundaryArrayBuf2.bind();

    // Tell OpenGL programmable pipeline how to locate position data
    int positionLocation2 = program->attributeLocation("input_position");
    program->enableAttributeArray(positionLocation2);
    program->setAttributeBuffer(positionLocation2, GL_FLOAT, 0, 3, sizeof(QVector3D));

    // Tell OpenGL which VBOs to use
    boundaryColorBuf2.bind();

    // Tell OpenGL programmable pipeline how to locate color data
    int colorLocation2 = program->attributeLocation("input_color");
    program->enableAttributeArray(colorLocation2);
    program->setAttributeBuffer(colorLocation2, GL_FLOAT, 0, 3, sizeof(QVector3D));

    // Tell OpenGL which VBOs to use
    boundaryNormalBuf2.bind();

    // Tell OpenGL programmable pipeline how to locate color data
    int normalLocation2 = program->attributeLocation("input_normal");
    program->enableAttributeArray(normalLocation2);
    program->setAttributeBuffer(normalLocation2, GL_FLOAT, 0, 3, sizeof(QVector3D));

    // Draw geometry using indices from VBO 1
    glLineWidth(10.0);
    glDrawArrays(GL_LINE_STRIP, 0, numBoundaryVertices2);

    // Tell OpenGL which VBOs to use
    boundaryArrayBuf1.bind();

    // Tell OpenGL programmable pipeline how to locate position data
    int positionLocation1 = program->attributeLocation("input_position");
    program->enableAttributeArray(positionLocation1);
    program->setAttributeBuffer(positionLocation1, GL_FLOAT, 0, 3, sizeof(QVector3D));

    // Tell OpenGL which VBOs to use
    boundaryColorBuf1.bind();

    // Tell OpenGL programmable pipeline how to locate color data
    int colorLocation1 = program->attributeLocation("input_color");
    program->enableAttributeArray(colorLocation1);
    program->setAttributeBuffer(colorLocation1, GL_FLOAT, 0, 3, sizeof(QVector3D));

    // Tell OpenGL which VBOs to use
    boundaryNormalBuf1.bind();

    // Tell OpenGL programmable pipeline how to locate color data
    int normalLocation1 = program->attributeLocation("input_normal");
    program->enableAttributeArray(normalLocation1);
    program->setAttributeBuffer(normalLocation1, GL_FLOAT, 0, 3, sizeof(QVector3D));

    // Draw geometry using indices from VBO 1
    glLineWidth(10.0);
    glDrawArrays(GL_LINE_STRIP, 0, numBoundaryVertices1);

    drawArrows(program);
}

void GeometryEngine::drawArrows(QOpenGLShaderProgram *program) {
    // Tell OpenGL which VBOs to use
    arrowArrayBuf.bind();

    // Tell OpenGL programmable pipeline how to locate position data
    int positionLocation = program->attributeLocation("input_position");
    program->enableAttributeArray(positionLocation);
    program->setAttributeBuffer(positionLocation, GL_FLOAT, 0, 3, sizeof(QVector3D));

    // Tell OpenGL which VBOs to use
    arrowColorBuf.bind();

    // Tell OpenGL programmable pipeline how to locate color data
    int colorLocation = program->attributeLocation("input_color");
    program->enableAttributeArray(colorLocation);
    program->setAttributeBuffer(colorLocation, GL_FLOAT, 0, 3, sizeof(QVector3D));

    // Tell OpenGL which VBOs to use
    arrowNormalBuf.bind();

    // Tell OpenGL programmable pipeline how to locate color data
    int normalLocation = program->attributeLocation("input_normal");
    program->enableAttributeArray(normalLocation);
    program->setAttributeBuffer(normalLocation, GL_FLOAT, 0, 3, sizeof(QVector3D));

    // Draw geometry using indices from VBO 1
    glLineWidth(5.0);
    glDrawArrays(GL_LINES, 0, numArrowVertices);
}

void GeometryEngine::drawPoints(QOpenGLShaderProgram *program) {
    // Tell OpenGL which VBOs to use
    pointArrayBuf.bind();

    // Tell OpenGL programmable pipeline how to locate position data
    int positionLocation = program->attributeLocation("input_position");
    program->enableAttributeArray(positionLocation);
    program->setAttributeBuffer(positionLocation, GL_FLOAT, 0, 3, sizeof(QVector3D));

    // Tell OpenGL which VBOs to use
    pointColorBuf.bind();

    // Tell OpenGL programmable pipeline how to locate color data
    int inputBoundaryColorLocation = program->attributeLocation("input_color");
    program->enableAttributeArray(inputBoundaryColorLocation);
    program->setAttributeBuffer(inputBoundaryColorLocation, GL_FLOAT, 0, 3, sizeof(QVector3D));

    // Tell OpenGL which VBOs to use
    pointNormalBuf.bind();

    // Tell OpenGL programmable pipeline how to locate color data
    int normalLocation = program->attributeLocation("input_normal");
    program->enableAttributeArray(normalLocation);
    program->setAttributeBuffer(normalLocation, GL_FLOAT, 0, 3, sizeof(QVector3D));

    // Draw geometry using indices from VBO 1
    glPointSize(10.0);
    glDrawArrays(GL_POINTS, 0, numPointVertices);
}
