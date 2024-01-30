#include "ResultsWidget.h"
#include <QMouseEvent>
#include <QtWidgets>
#include <cmath>
#include <limits>
#include <queue>
#include <unordered_set>
#include <iostream>

ResultsWidget::~ResultsWidget() {
    // Make sure the context is current when deleting the buffers.
    makeCurrent();
    delete geometryEngine;
    doneCurrent();
}

void ResultsWidget::addSurface(surface newSurface) {
    shouldPaintGL = true;
    lineVertices.clear();
    if (newSurface == surface::cube) {
        mesh->copySurface(new Cube());
    } else if (newSurface == surface::sphere) {
        mesh->copySurface(new Sphere());
    } else if (newSurface == surface::torus) {
        mesh->copySurface(new Torus());
    } else if (newSurface == surface::mobiusStrip) {
        mesh->copySurface(new MobiusStrip());
    } else if (newSurface == surface::crossCap) {
        mesh->copySurface(new CrossCap);
    } else if (newSurface == surface::kleinBottle) {
        mesh->copySurface(new KleinBottle());
    }
    geometryEngine->initMesh(mesh);
    geometryEngine->initLine(lineVertices);
    update();
}

void ResultsWidget::toggleShouldDeleteSurface() {
    shouldDeleteSurface = !shouldDeleteSurface;
}

void ResultsWidget::mousePressEvent(QMouseEvent *e) {
    if (shouldDeleteSurface) {
        deleteSurface(e);
    } else {
        // TODO: Comment out for testing
        mouseMoveEvent(e);
        //lineVertices.clear();
        isDrawingMode = true;
    }
}

void ResultsWidget::cutSurface() {
    std::vector<Vertex*> vertices = mesh->getVertices();
    std::vector<Triangle> triangles = mesh->getTriangles();
    Triangle prevTriangle;
    int prevTriangleIndex;
    Vertex *prevVertex;

    for (Vertex* nextVertex : lineVertices) {
        bool isFirstVertex = (nextVertex == lineVertices.front());
        for (int i=0; i<triangles.size(); i++) {
            Triangle nextTriangle = triangles[i];
            Vertex *vertex1 = vertices[nextTriangle.vertexIndices[0]];
            Vertex *vertex2 = vertices[nextTriangle.vertexIndices[1]];
            Vertex *vertex3 = vertices[nextTriangle.vertexIndices[2]];

            int matchingIndex = -1;
            if (vertex1 == nextVertex){
                matchingIndex = 0;
            } else if (vertex2 == nextVertex) {
                matchingIndex = 1;
            } else if (vertex3 == nextVertex) {
                matchingIndex = 2;
            }

            bool containsNextVertex = (matchingIndex != -1);
            bool doesNotContainPreviousVertex = (vertex1 != prevVertex && vertex2 != prevVertex && vertex3 != prevVertex);

            int numConnectedVertices = 0;
            int prevMatchingIndex = -1;
            for (int j=0; j<prevTriangle.vertexIndices.size(); j++) {
                int previousTriangleIndex = prevTriangle.vertexIndices[j];
                Vertex *previousTriangleVertex = vertices[previousTriangleIndex];
                if (vertex1 == previousTriangleVertex || vertex2 == previousTriangleVertex || vertex3 == previousTriangleVertex) {
                    numConnectedVertices += 1;
                }
                if (nextVertex == previousTriangleVertex) {
                    prevMatchingIndex = j;
                }
            }
            bool sharesEdgeWithPreviousTriangle = (numConnectedVertices > 1);

            if (containsNextVertex && (sharesEdgeWithPreviousTriangle && doesNotContainPreviousVertex || isFirstVertex)) {
                Vertex *newVertex = new Vertex({QVector3D(nextVertex->position.x(), nextVertex->position.y(), nextVertex->position.z())});
                mesh->addVertex(newVertex);
                mesh->updateTriangles(i, matchingIndex, mesh->getVertices().size()-1);
                if (prevMatchingIndex != -1) {
                    mesh->updateTriangles(prevTriangleIndex, prevMatchingIndex, mesh->getVertices().size() - 1);
                }

                std::cout << "NewVertex: " << std::endl;
                std::cout << newVertex->toString() << std::endl;
                std::cout << "Neighbors: " << std::endl;
                for (auto neighbor : newVertex->neighbors) {
                    std::cout << neighbor->toString() << std::endl;
                }

                prevTriangleIndex = i;
                prevTriangle = nextTriangle;
                prevVertex = nextVertex;
                break;
            }
        }
    }
    geometryEngine->initMesh(mesh);
    update();
}

void ResultsWidget::deleteSurface(QMouseEvent *e) {
    Vertex *startingVertex = getVertexFromMouseEvent(e);
    if (startingVertex == nullptr) {
        return;
    }

    std::unordered_set<Vertex*> deletedVertices;
    std::queue<Vertex*> verticesToDelete;
    verticesToDelete.push(startingVertex);

    while (!verticesToDelete.empty()) {
        Vertex* vertexToDelete = verticesToDelete.front();

        std::cout << "Deleted vectors: " << std::endl;
        std::cout << vertexToDelete->toString() << std::endl;

        mesh->deleteVertex(vertexToDelete);
        verticesToDelete.pop();

        for (Vertex* neighbor : vertexToDelete->neighbors) {
            //std::string neighborString = neighbor->toString();
            if (deletedVertices.find(neighbor) == deletedVertices.end()) {
                verticesToDelete.push(neighbor);
                deletedVertices.insert(neighbor);
            }
        }
    }
    geometryEngine->initMesh(mesh);
    update();
}

void ResultsWidget::mouseMoveEvent(QMouseEvent *e) {
    if (not isDrawingMode or not shouldPaintGL) {
        return;
    }

    Vertex *vertex = getVertexFromMouseEvent(e);
    if (vertex == nullptr) {
        lineVertices.clear();
    } else if (lineVertices.size() == 0 || vertex != lineVertices.back()) {
        std::cout << "drawn vertices: " << std::endl;
        std::cout << vertex->toString() << std::endl;
        addLineVertices(vertex);
    }

    geometryEngine->initLine(lineVertices);
    update();
}

Vertex* ResultsWidget::getVertexFromMouseEvent(QMouseEvent *e) {
    Vertex *closestVertex = new Vertex;
    bool surfaceVertexFound = false;
    float smallestZDistance = std::numeric_limits<float>::max();
    std::vector<Vertex*> vertices = mesh->getVertices();
    float x = (e->position().x() - (width()/2.0)) / (width()/2.0);
    float y = -(e->position().y() - (height()/2.0)) / (height()/2.0);

    for (int i=0; i<vertices.size(); i++) {
        QVector3D surfacePosition = mvp_matrix.map(vertices[i]->position);
        float xyDistance = sqrt(pow(x - surfacePosition.x(), 2) + pow(y - surfacePosition.y(), 2));

        //TODO: Change to 0.1 for testing
        if (xyDistance < 0.1 && surfacePosition.z() < smallestZDistance) {
            smallestZDistance = surfacePosition.z();
            closestVertex = vertices[i];
            surfaceVertexFound = true;
        }
    }
    if (surfaceVertexFound) {
        return closestVertex;
    } else {
        return nullptr;
    }
}

void ResultsWidget::addLineVertices(Vertex *newVertex) {
    std::vector<Vertex *> newVertices = getNewVertices(newVertex);
    for (int i = newVertices.size() - 1; i > -1; i--) {
        checkLineVerticesForLoop(newVertices[i]);
        lineVertices.push_back(newVertices[i]);
    }
}

std::vector<Vertex*> ResultsWidget::getNewVertices(Vertex *newVertex) {
    std::vector<Vertex*> newVertices = {newVertex};
    if (lineVertices.size() == 0) {
        return newVertices;
    }

    std::unordered_set<Vertex*> checkedVertices;
    std::queue<std::vector<Vertex*>> potentialPaths;
    potentialPaths.push(newVertices);
    QVector3D lastVertexPosition = lineVertices.back()->position;

    while(!potentialPaths.empty()) {
        std::vector<Vertex*> nextPath = potentialPaths.front();
        Vertex *nextVertex = nextPath.back();
        potentialPaths.pop();

        for (Vertex* neighbor : nextVertex->neighbors) {
            if (neighbor->position == lastVertexPosition) {
                return nextPath;
            } else if(checkedVertices.find(neighbor) == checkedVertices.end()) {
                std::vector<Vertex*> newPotentialPath = nextPath;
                newPotentialPath.push_back(neighbor);
                potentialPaths.push(newPotentialPath);
                checkedVertices.insert(neighbor);
            }
        }
    }
    return {newVertex};
}

void ResultsWidget::checkLineVerticesForLoop(Vertex *newVertex) {
    //TODO: Change to 3 for testing
    int numRecentVerticesToExcludeForLoopChecking = 3;
    if (lineVertices.size() < numRecentVerticesToExcludeForLoopChecking) {
        return;
    }

    for (int i=0; i<=lineVertices.size()-numRecentVerticesToExcludeForLoopChecking; i++) {
        if (lineVertices[i] == newVertex) {
            isDrawingMode = false;
            for (int j=0; j<i; j++) {
                lineVertices.erase(lineVertices.begin());
            }
        }
    }
}


void ResultsWidget::wheelEvent(QWheelEvent *e) {
    // Rotation axis is perpendicular to the angle delta difference vector
    QVector3D n = QVector3D(e->angleDelta().y(), e->angleDelta().x(), 0.0).normalized();

    // Accelerate angular speed relative to the length of the mouse sweep
    qreal acc = 2.0;

    // Calculate new rotation axis as weighted sum
    rotationAxis = (rotationAxis * angularSpeed - n * acc).normalized();

    // Increase angular speed
    angularSpeed += (acc/12.0);
}

void ResultsWidget::timerEvent(QTimerEvent *) {
    // Decrease angular speed (friction)
    angularSpeed *= 0.9;

    // Stop rotation when speed goes below threshold
    if (angularSpeed < 0.01) {
        angularSpeed = 0.0;
    } else {
        // Update rotation
        rotation = QQuaternion::fromAxisAndAngle(rotationAxis, angularSpeed) * rotation;

        // Request an update
        update();
    }
}

void ResultsWidget::initializeGL() {
    initializeOpenGLFunctions();
    glClearColor(96.0/255, 96.0/255, 96.0/255, 1.0);

    initShaders();
    geometryEngine = new GeometryEngine();
    mesh = new Mesh();

    isDrawingMode = true;
    shouldPaintGL = false;
    shouldDeleteSurface = false;
    lineDrawingColor = Qt::white;

    // Use QBasicTimer because it's faster than QTimer
    timer.start(12, this);
}

void ResultsWidget::initShaders() {
    // Compile vertex shader
    if (!program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/VShader.glsl"))
        close();

    // Compile fragment shader
    if (!program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/FShader.glsl"))
        close();

    // Link shader pipeline
    if (!program.link())
        close();

    // Bind shader pipeline for use
    if (!program.bind())
        close();
}

void ResultsWidget::resizeGL(int w, int h) {

    // Calculate aspect ratio
    qreal aspect = qreal(w) / qreal(h ? h : 1);

    // Set near plane to 3.0, far plane to 7.0, field of view 45 degrees
    const qreal zNear = 3.0, zFar = 7.0, fov = 45.0;

    // Reset projection
    projection.setToIdentity();

    // Set perspective projection
    projection.perspective(fov, aspect, zNear, zFar);
}

void ResultsWidget::paintGL() {
    if (shouldPaintGL) {
        // Clear color and depth buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Enable depth buffer
        glEnable(GL_DEPTH_TEST);

        // Enable back face culling
        //glEnable(GL_CULL_FACE);

        program.bind();

        // Calculate model view transformation
        QMatrix4x4 matrix;
        matrix.translate(0.0, 0.0, -5.0);
        matrix.rotate(rotation);
        mvp_matrix = projection * matrix;

        // Set modelview-projection matrix
        program.setUniformValue("mvp_matrix", projection * matrix);

        // Draw geometry
        geometryEngine->drawMesh(&program);
        geometryEngine->drawLine(&program, lineDrawingColor);
    }
}

void ResultsWidget::setLineDrawingColor(QColor newColor) {
    lineDrawingColor = newColor;
}
