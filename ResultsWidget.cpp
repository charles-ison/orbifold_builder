#include "ResultsWidget.h"
#include <QMouseEvent>
#include <QtWidgets>
#include <cmath>
#include <limits>
#include <queue>
#include <unordered_set>
#include <tuple>

ResultsWidget::~ResultsWidget() {
    // Make sure the context is current when deleting the buffers.
    makeCurrent();
    delete geometryEngine;
    delete mesh;
    delete cubeSurface;
    delete sphereSurface;
    delete torusSurface;
    delete mobiusStripSurface;
    delete crossCapSurface;
    delete kleinBottleSurface;
    doneCurrent();
}

void ResultsWidget::addSurface(surface newSurface) {
    shouldPaintGL = true;
    drawnVertices.clear();
    boundaryVertices1.clear();
    boundaryVertices2.clear();
    QVector3D centerPosition = {0, 0, 0};
    if (newSurface == surface::cube) {
        QVector3D scale = {1.0, 0.0, 0.0};
        cubeSurface = new Cube(centerPosition, scale);
        mesh->resetSurface(cubeSurface);
    } else if (newSurface == surface::sphere) {
        QVector3D scale = {1.3, 1.3, 1.3};
        sphereSurface = new Sphere(centerPosition, scale);
        mesh->resetSurface( sphereSurface);
    } else if (newSurface == surface::torus) {
        QVector3D scale = {1.0, 0.5, 0.0};
        torusSurface = new Torus(centerPosition, scale);
        mesh->resetSurface(torusSurface);
    } else if (newSurface == surface::mobiusStrip) {
        QVector3D scale = {1.0, 0.0, 0.0};
        mobiusStripSurface = new MobiusStrip(centerPosition, scale);
        mesh->resetSurface(mobiusStripSurface);
    } else if (newSurface == surface::crossCap) {
        QVector3D scale = {1.5, 1.0, 1.0};
        crossCapSurface = new CrossCap(centerPosition, scale);
        mesh->resetSurface(crossCapSurface);
    } else if (newSurface == surface::kleinBottle) {
        QVector3D scale = {0.5, 1.0, 0.5};
        kleinBottleSurface = new KleinBottle(centerPosition, scale);
        mesh->resetSurface(kleinBottleSurface);
    }

    if (shouldAnimate) {
        geometryEngine->initAnimation(mesh);
    } else {
        geometryEngine->initMesh(mesh);
    }
    geometryEngine->initLine(drawnVertices, drawingColor);
    geometryEngine->initBoundary(boundaryVertices1, boundaryVertices2);
    update();
}

void ResultsWidget::toggleShouldDeleteSurface() {
    shouldDeleteSurface = !shouldDeleteSurface;
}

void ResultsWidget::mousePressEvent(QMouseEvent *e) {
    if (shouldDeleteSurface) {
        deleteSurface(e);
    } else {
        // TODO: Switch comments for testing
        mouseMoveEvent(e);
        //drawnVertices.clear();
        isDrawingMode = true;
    }
}

bool ResultsWidget::triangleContainsVertex(Vertex *vertex, Triangle *triangle, std::vector<Vertex*> vertices) {
    Vertex *vertex1 = vertices[triangle->vertexIndices[0]];
    Vertex *vertex2 = vertices[triangle->vertexIndices[1]];
    Vertex *vertex3 = vertices[triangle->vertexIndices[2]];

    if (vertex1==vertex || vertex2==vertex || vertex3==vertex) {
        return true;
    } else {
        return false;
    }
}

bool ResultsWidget::rotationDirectionAligns(Triangle* triangle, Vertex* vertex1, Vertex* vertex2, std::vector<Vertex*> vertices) {
    for (int i=0; i<3; i++) {
        Vertex* triangleVertex1 = vertices[triangle->vertexIndices[i]];
        Vertex* triangleVertex2 = vertices[triangle->vertexIndices[(i+1)%3]];

        if (vertex1 == triangleVertex1 && vertex2 == triangleVertex2) {
            return true;
        }
    }
    return false;
}

void ResultsWidget::cutSurface() {
    if (drawnVertices.size() < 2) {
        return;
    }

    bool loopDetected = false;
    boundaryVertices1.clear();
    boundaryVertices2.clear();
    Triangle* startingTriangle;
    Triangle* stepStartingTriangle;
    std::vector<Vertex*> vertices = mesh->getVertices();
    std::vector<Vertex*> verticesToCut;
    std::vector<std::vector<Triangle*>> trianglePathsToCut;
    for (Triangle *triangle : drawnVertices[0]->triangles) {
        if(triangleContainsVertex(drawnVertices[0], triangle, vertices)
              && triangleContainsVertex(drawnVertices[1], triangle, vertices)) {
            startingTriangle = triangle;
            break;
        }
    }

    stepStartingTriangle = startingTriangle;
    verticesToCut.push_back(drawnVertices[0]);
    verticesToCut.push_back(drawnVertices[1]);
    trianglePathsToCut.push_back({stepStartingTriangle});
    bool startingRotationDirectionAligns = rotationDirectionAligns(startingTriangle, drawnVertices[0], drawnVertices[1], vertices);

    // Required for loops
    if (drawnVertices.front() == drawnVertices.back()) {
        loopDetected = true;
        drawnVertices.push_back(drawnVertices[1]);
    }

    // Find all triangles that need to be cut
    for (int i=1; i<drawnVertices.size()-1; i++) {
        Vertex* prevStartVertex = drawnVertices[i-1];
        Vertex* startVertex = drawnVertices[i];
        Vertex* endVertex = drawnVertices[i+1];

        std::unordered_set<std::string> checkedTriangles;
        checkedTriangles.insert(stepStartingTriangle->toString());

        std::queue<std::vector<Triangle*>> potentialPaths;
        potentialPaths.push({stepStartingTriangle});

        while(!potentialPaths.empty()) {
            std::vector<Triangle*> nextPath = potentialPaths.front();
            potentialPaths.pop();
            Triangle* nextTriangle = nextPath.back();

            bool nextTriangleContainsVertex = triangleContainsVertex(endVertex, nextTriangle, vertices);
            bool nextRotationDirectionAligns = (startingRotationDirectionAligns == rotationDirectionAligns(nextTriangle, startVertex, endVertex, vertices));
            if (nextTriangleContainsVertex && nextRotationDirectionAligns) {
                verticesToCut.push_back(endVertex);
                trianglePathsToCut.push_back(nextPath);
                stepStartingTriangle = nextTriangle;
                break;
            }

            for (Triangle *triangle : startVertex->triangles) {
                std::string triangleString = triangle->toString();
                bool sharesEdge = nextTriangle->sharesEdge(triangle);
                bool notChecked = checkedTriangles.find(triangleString) == checkedTriangles.end();
                bool containsPrevStartingVertex = triangleContainsVertex(prevStartVertex, triangle, vertices);
                bool directionAligns = (startingRotationDirectionAligns == rotationDirectionAligns(triangle, prevStartVertex, startVertex, vertices));
                bool doesNotCrossLine = !containsPrevStartingVertex || directionAligns;
                if (sharesEdge && notChecked && doesNotCrossLine) {
                    std::vector<Triangle*> newPotentialPath = nextPath;
                    newPotentialPath.push_back(triangle);
                    potentialPaths.push(newPotentialPath);
                    checkedTriangles.insert(triangleString);
                }
            }
        }
    }

    std::unordered_map<int, int> oldIndexToNewIndexMap;
    for (int i=0; i<verticesToCut.size(); i++) {
        Vertex* vertexToCut = verticesToCut[i];
        Vertex* newVertex = new Vertex;
        newVertex->index = vertices.size();
        newVertex->position = vertexToCut->position;
        mesh->addVertex(newVertex);
        boundaryVertices1.push_back(vertexToCut);
        boundaryVertices2.push_back(newVertex);
        vertices = mesh->getVertices();
        oldIndexToNewIndexMap.insert({vertexToCut->index, newVertex->index});
    }

    if (loopDetected) {
        boundaryVertices1.pop_back();
        boundaryVertices2.pop_back();
    }

    // Updating vertices on triangles, adding new triangles, and removing out of date triangles
    for (std::vector<Triangle*> nextTrianglePath : trianglePathsToCut) {
        for (Triangle *triangle: nextTrianglePath) {
            for (int j = 0; j < 3; j++) {
                int oldVertexIndex = triangle->vertexIndices[j];
                if (oldIndexToNewIndexMap.find(oldVertexIndex) != oldIndexToNewIndexMap.end()) {
                    int newVertexIndex = oldIndexToNewIndexMap.at(oldVertexIndex);
                    triangle->vertexIndices[j] = newVertexIndex;
                    vertices[newVertexIndex]->triangles.insert(triangle);
                    vertices[oldVertexIndex]->triangles.erase(triangle);
                }
            }
        }
    }
    drawnVertices.clear();
    geometryEngine->initLine(drawnVertices, drawingColor);
    geometryEngine->initBoundary(boundaryVertices1, boundaryVertices2);
    geometryEngine->initMesh(mesh);
    update();
}

void ResultsWidget::deleteSurface(QMouseEvent *e) {
    Vertex *startingVertex = getVertexFromMouseEvent(e);
    if (startingVertex == nullptr) {
        return;
    }

    std::unordered_set<Vertex*> verticesToDeleteSet;
    verticesToDeleteSet.insert(startingVertex);

    std::queue<Vertex*> verticesToDelete;
    verticesToDelete.push(startingVertex);

    while (!verticesToDelete.empty()) {
        std::vector<Vertex*> meshVertices = mesh->getVertices();
        Vertex* vertexToDelete = verticesToDelete.front();
        verticesToDelete.pop();

        for (Triangle* triangle : vertexToDelete->triangles) {
            for (int vertexIndex : triangle->vertexIndices) {
                Vertex* neighbor = meshVertices[vertexIndex];
                if (verticesToDeleteSet.find(neighbor) == verticesToDeleteSet.end()) {
                    verticesToDelete.push(neighbor);
                    verticesToDeleteSet.insert(neighbor);
                }
            }
        }
    }
    mesh->deleteVertices(verticesToDeleteSet);
    geometryEngine->initMesh(mesh);

    std::vector<Vertex*> remainingVertices;
    if (verticesToDeleteSet.find(boundaryVertices1.back()) == verticesToDeleteSet.end()) {
        remainingVertices = boundaryVertices1;
    } else {
        remainingVertices = boundaryVertices2;
    }
    auto first = remainingVertices.begin();
    auto middle = remainingVertices.begin() + remainingVertices.size()/2;
    auto last = remainingVertices.end();
    boundaryVertices1 = std::vector<Vertex*>(first, middle);
    boundaryVertices2 = std::vector<Vertex*>(middle, last);

    boundaryVertices1.push_back(boundaryVertices2.front());
    std::reverse(boundaryVertices2.begin(), boundaryVertices2.end());
    geometryEngine->initBoundary(boundaryVertices1, boundaryVertices2);
    update();
}

void ResultsWidget::mouseMoveEvent(QMouseEvent *e) {
    if (not isDrawingMode or not shouldPaintGL) {
        return;
    }

    Vertex *vertex = getVertexFromMouseEvent(e);
    if (vertex == nullptr) {
        // TODO: comment out for testing
        //drawnVertices.clear();
    } else if (drawnVertices.size() == 0 || vertex != drawnVertices.back()) {
        addDrawnVertices(vertex);
    }

    geometryEngine->initLine(drawnVertices, drawingColor);
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

void ResultsWidget::addDrawnVertices(Vertex *newVertex) {
    std::vector<Vertex *> newVertices = getNewVerticesPath(newVertex);
    for (int i = newVertices.size() - 1; i > -1; i--) {
        if (drawnVertices.size() > 2 && newVertices[i] == drawnVertices[drawnVertices.size()-2]) {
            drawnVertices.pop_back();
        }
        else {
            bool loopFound = drawnVerticesContainLoop(newVertices[i]);
            drawnVertices.push_back(newVertices[i]);
            if (loopFound) {
                break;
            }
        }
    }
}

std::vector<Vertex*> ResultsWidget::getNewVerticesPath(Vertex *newVertex) {
    std::vector<Vertex*> newVertices = {newVertex};
    if (drawnVertices.size() == 0) {
        return newVertices;
    }

    std::unordered_set<Vertex*> checkedVertices;
    std::priority_queue<std::tuple<float, std::vector<Vertex*>>, std::vector<std::tuple<float, std::vector<Vertex*>>>, std::greater<std::tuple<float, std::vector<Vertex*>>> > potentialPaths;

    potentialPaths.push({0.0, newVertices});
    QVector3D lastVertexPosition = drawnVertices.back()->position;
    std::vector<Vertex*> meshVertices = mesh->getVertices();

    while(!potentialPaths.empty()) {
        std::tuple<float, std::vector<Vertex*>> nextDistanceAndPath = potentialPaths.top();
        float oldDistance = std::get<0>(nextDistanceAndPath);
        std::vector<Vertex*> nextPath = std::get<1>(nextDistanceAndPath);
        Vertex *nextVertex = nextPath.back();
        potentialPaths.pop();

        for (Triangle* triangle : nextVertex->triangles) {
            for (int vertexIndex : triangle->vertexIndices) {
                Vertex *neighbor = meshVertices[vertexIndex];
                if (neighbor->position == lastVertexPosition) {
                    return nextPath;
                } else if (checkedVertices.find(neighbor) == checkedVertices.end()) {
                    std::vector<Vertex*> newPotentialPath = nextPath;
                    newPotentialPath.push_back(neighbor);
                    float distance = oldDistance + euclideanDistance(neighbor, nextVertex);
                    potentialPaths.push({distance, newPotentialPath});
                    checkedVertices.insert(neighbor);
                }
            }
        }
    }
    return {newVertex};
}

float ResultsWidget::euclideanDistance(Vertex* vertex1, Vertex* vertex2) {
    float dx = vertex1->position.x()-vertex2->position.x();
    float dy = vertex1->position.y()-vertex2->position.y();
    float dz = vertex1->position.z()-vertex2->position.z();

    // Use for fast marching
    //float delta = 2*oldDistance - pow(dx-dy-dz, 2);
    //if (delta >= 0) {
    //    return (dx + dy + dz + sqrt(delta))/2;
    //} else {
    //    return oldDistance + sqrt(pow(dx, 2) + pow(dy, 2) + pow(dz, 2));
    //}

    return sqrt(pow(dx, 2) + pow(dy, 2) + pow(dz, 2));
}

bool ResultsWidget::drawnVerticesContainLoop(Vertex *newVertex) {
    bool loopFound = false;
    for (int i=0; i<drawnVertices.size(); i++) {
        if (drawnVertices[i] == newVertex) {
            isDrawingMode = false;
            loopFound = true;
            for (int j=0; j<i; j++) {
                drawnVertices.erase(drawnVertices.begin());
            }
        }
    }
    return loopFound;
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

    bool shouldUpdate = false;

    // Decrease angular speed (friction)
    angularSpeed *= 0.9;

    // Stop rotation when speed goes below threshold
    if (angularSpeed < 0.01) {
        angularSpeed = 0.0;
    } else {
        // Update rotation
        rotation = QQuaternion::fromAxisAndAngle(rotationAxis, angularSpeed) * rotation;

        // Request an update
        shouldUpdate = true;
    }

    if (shouldPaintGL and shouldAnimate) {
        geometryEngine->initAnimation(mesh);
        shouldUpdate = true;
    }

    if (shouldUpdate) {
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
    drawingColor = Qt::white;

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
        geometryEngine->drawLine(&program);
        geometryEngine->drawBoundary(&program);
    }
}

void ResultsWidget::setDrawingColor(QColor newColor) {
    drawingColor = newColor;
}

void ResultsWidget::glue() {
    for (int i=0; i<boundaryVertices2.size(); i++) {
        Vertex* boundaryVertex2 = boundaryVertices2[i];
        Vertex* boundaryVertex1 = boundaryVertices1[boundaryVertices1.size()-i-1];

        for (Triangle* triangle : boundaryVertex2->triangles) {
            std::vector<int> boundaryTriangleIndices = triangle->vertexIndices;
            for (int j=0; j<3; j++) {
                if (boundaryTriangleIndices[j] == boundaryVertex2->index) {
                    triangle->vertexIndices[j] = boundaryVertex1->index;
                    boundaryVertex1->triangles.insert(triangle);
                }
            }
        }
    }

    for (Triangle* triangle: boundaryVertices1.back()->triangles) {
        std::vector<int> boundaryTriangleVertices = triangle->vertexIndices;
        for (int j=0; j<3; j++) {
            if (boundaryTriangleVertices[j] == boundaryVertices1.back()->index) {
                triangle->vertexIndices[j] = boundaryVertices1.front()->index;
                boundaryVertices1.front()->triangles.insert(triangle);
            }
        }
    }

    boundaryVertices1.clear();
    boundaryVertices2.clear();
    geometryEngine->initBoundary(boundaryVertices1, boundaryVertices2);
    geometryEngine->initMesh(mesh);
    update();
}

//TODO: geodesic distance?
void ResultsWidget::smooth() {
    float stepSize = 0.5;
    std::vector<Vertex*> vertices = mesh->getVertices();
    std::vector<QVector3D> newPositions;

    for (Vertex* vertex : vertices) {
        float xDiff = 0.0;
        float yDiff = 0.0;
        float zDiff = 0.0;
        float weightDenominator = 0.0;
        std::unordered_set<Vertex*> visitedNeighbors;

        // Initializing cord weights
        for (Triangle* triangle : vertex->triangles) {
            std::vector<int> triangleIndices = triangle->vertexIndices;
            for (int index : triangleIndices) {
                Vertex* neighbor = vertices[index];
                if (neighbor != vertex && visitedNeighbors.find(neighbor) == visitedNeighbors.end()) {
                    weightDenominator += (1.0 / euclideanDistance(vertex, neighbor));
                    visitedNeighbors.insert(neighbor);
                }
            }
        }

        visitedNeighbors.clear();
        for (Triangle* triangle : vertex->triangles) {
            std::vector<int> triangleIndices = triangle->vertexIndices;
            for (int index : triangleIndices) {
                Vertex* neighbor = vertices[index];
                if (neighbor != vertex && visitedNeighbors.find(neighbor) == visitedNeighbors.end()) {
                    float distance = euclideanDistance(vertex, neighbor);
                    float weight = (1.0 / distance) / weightDenominator;
                    xDiff += weight * (neighbor->position.x() - vertex->position.x());
                    yDiff += weight * (neighbor->position.y() - vertex->position.y());
                    zDiff += weight * (neighbor->position.z() - vertex->position.z());
                    visitedNeighbors.insert(neighbor);
                }
            }
        }

        float newX = vertex->position.x() + stepSize * xDiff;
        float newY = vertex->position.y() + stepSize * yDiff;
        float newZ = vertex->position.z() + stepSize * zDiff;
        newPositions.push_back({newX, newY, newZ});
    }

    for (int i=0; i<vertices.size(); i++) {
        vertices[i]->position = newPositions[i];
    }

    geometryEngine->initMesh(mesh);
    geometryEngine->initLine(drawnVertices, drawingColor);
    geometryEngine->initBoundary(boundaryVertices1, boundaryVertices2);
    update();
}

void ResultsWidget::glueAnimation() {
    //shouldAnimate = !shouldAnimate;

    //mesh->addSurface(new CrossCap(centerPosition, scale));
    //geometryEngine->initMesh(mesh);
    //update();
}
