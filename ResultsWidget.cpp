#include "ResultsWidget.h"
#include <QMouseEvent>
#include <QtWidgets>
#include <cmath>
#include <queue>
#include <unordered_set>
#include <tuple>
#include <iostream>

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

void ResultsWidget::addSurface(Surface newSurface) {
    shouldPaintGL = true;
    boundariesReversed = false;
    isBoundary1Loop = false;
    isBoundary2Loop = false;
    boundariesAreCombinedLoop = false;
    boundariesOverlapping = false;
    numOpenings = 0;
    boundary1DisplaySize = 0;
    boundary2DisplaySize = 0;
    numSmoothingSteps = 0;
    numSmoothingStepsSoFar = 0;
    drawnVertices.clear();
    verticesToSmooth.clear();
    verticesToSmoothMap.clear();
    boundaryVertices1.clear();
    boundaryVertices2.clear();
    selectedPoints.clear();
    QVector3D centerPosition = {0, 0, 0};
    if (newSurface == Surface::cube) {
        QVector3D scale = {1.0, 0.0, 0.0};
        cubeSurface = new Cube(centerPosition, scale);
        mesh->resetSurface(cubeSurface);
    } else if (newSurface == Surface::sphere) {
        QVector3D scale = {1.3, 1.3, 1.3};
        sphereSurface = new Sphere(centerPosition, scale);
        mesh->resetSurface( sphereSurface);
    } else if (newSurface == Surface::torus) {
        QVector3D scale = {1.0, 0.5, 0.0};
        torusSurface = new Torus(centerPosition, scale);
        mesh->resetSurface(torusSurface);
    } else if (newSurface == Surface::mobiusStrip) {
        QVector3D scale = {1.0, 0.0, 0.0};
        mobiusStripSurface = new MobiusStrip(centerPosition, scale);
        mesh->resetSurface(mobiusStripSurface);
    } else if (newSurface == Surface::crossCap) {
        QVector3D scale = {1.5, 1.0, 1.0};
        crossCapSurface = new CrossCap(centerPosition, scale);
        mesh->resetSurface(crossCapSurface);
    } else if (newSurface == Surface::kleinBottle) {
        QVector3D scale = {0.5, 1.0, 0.5};
        kleinBottleSurface = new KleinBottle(centerPosition, scale);
        mesh->resetSurface(kleinBottleSurface);
    } else if (newSurface == Surface::plyFile) {
        QVector3D scale = {1.0, 1.0, 1.0};
        QString fileName = QFileDialog::getOpenFileName(this,tr("Open Ply File"), QDir::homePath(), tr("Ply Files (*.ply)"));
        plyFileSurface = new PlyFile(fileName, centerPosition, scale);
        mesh->resetSurface(plyFileSurface);
    }

    geometryEngine->initMesh(mesh);
    geometryEngine->initLine(drawnVertices, drawingColor);
    geometryEngine->initBoundary(boundaryVertices1, boundaryVertices2, boundary1DisplaySize, boundary2DisplaySize, isBoundary1Loop, isBoundary2Loop, boundariesAreCombinedLoop, boundariesReversed, boundariesOverlapping, numOpenings);
    update();
}

void ResultsWidget::toggleDeleteSurface() {
    if (!findVertexToDelete) {
        isDrawingEnabled = false;
        drawnVertices.clear();
        geometryEngine->initLine(drawnVertices, drawingColor);
    } else {
        if(!selectedPoints.empty()) {
            deleteSurface(selectedPoints.front());
        }
        isDrawingEnabled = true;
    }

    findVertexToDelete = !findVertexToDelete;
    update();
}

void ResultsWidget::mousePressEvent(QMouseEvent *e) {
    if (findVertexToDelete || findVertexToSmooth) {
        Vertex *vertex = getVertexFromMouseEvent(e);
        setSelectedPoints(vertex);
    } else {
        if (drawingMode == DrawingMode::click) {
            mouseMoveEvent(e);
        } else if (drawingMode == DrawingMode::drag) {
            drawnVertices.clear();
        }
        isDrawingEnabled = true;
    }
}

void ResultsWidget::setSelectedPoints(Vertex *vertex) {
    if (vertex != nullptr) {
        selectedPoints = {vertex};
    } else {
        selectedPoints.clear();
    }
    geometryEngine->initPoints(selectedPoints);
    update();
}

bool ResultsWidget::triangleContainsVertex(Vertex *vertex, Triangle *triangle) {
    Vertex *vertex1 = triangle->vertices[0];
    Vertex *vertex2 = triangle->vertices[1];
    Vertex *vertex3 = triangle->vertices[2];

    if (vertex1==vertex || vertex2==vertex || vertex3==vertex) {
        return true;
    } else {
        return false;
    }
}

bool ResultsWidget::rotationDirectionAligns(Triangle* triangle, Vertex* vertex1, Vertex* vertex2) {
    for (int i=0; i<3; i++) {
        Vertex* triangleVertex1 = triangle->vertices[i];
        Vertex* triangleVertex2 = triangle->vertices[(i+1)%3];

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
    if (numOpenings >= 2) {
        return;
    }
    if (numOpenings == 1 && isBoundary1Loop && isBoundary2Loop) {
        return;
    }

    bool loopDetected = false;
    Triangle* startingTriangle;
    Triangle* stepStartingTriangle;
    std::vector<Vertex*> tempBoundaryVertices1;
    std::vector<Vertex*> tempBoundaryVertices2;
    std::vector<Vertex*> vertices = mesh->getVertices();
    std::vector<Vertex*> verticesToCut;
    std::vector<std::vector<Triangle*>> trianglePathsToCut;
    for (Triangle *triangle : drawnVertices[0]->triangles) {
        if(triangleContainsVertex(drawnVertices[0], triangle) && triangleContainsVertex(drawnVertices[1], triangle)) {
            startingTriangle = triangle;
            break;
        }
    }

    stepStartingTriangle = startingTriangle;
    verticesToCut.push_back(drawnVertices[0]);
    verticesToCut.push_back(drawnVertices[1]);
    trianglePathsToCut.push_back({stepStartingTriangle});
    bool startingRotationDirectionAligns = rotationDirectionAligns(startingTriangle, drawnVertices[0], drawnVertices[1]);

    // Setting loop configurations
    if (drawnVertices.front() == drawnVertices.back()) {
        drawnVertices.push_back(drawnVertices[1]);
        if (numOpenings == 0 || boundariesAreCombinedLoop) {
            isBoundary1Loop = true;
        }
        isBoundary2Loop = true;
        loopDetected = true;
    } else {
        if (numOpenings == 0) {
            isBoundary1Loop = false;
        } else if (boundariesAreCombinedLoop) {
            isBoundary1Loop = true;
        }
        isBoundary2Loop = false;
    }

    // Find all triangles that need to be cut
    for (int i=1; i<drawnVertices.size()-1; i++) {
        Vertex* prevStartVertex = drawnVertices[i-1];
        Vertex* startVertex = drawnVertices[i];
        Vertex* endVertex = drawnVertices[i+1];

        std::unordered_set<Triangle*> checkedTriangles;
        checkedTriangles.insert(stepStartingTriangle);

        std::queue<std::vector<Triangle*>> potentialPaths;
        potentialPaths.push({stepStartingTriangle});

        while(!potentialPaths.empty()) {
            std::vector<Triangle*> nextPath = potentialPaths.front();
            potentialPaths.pop();
            Triangle* nextTriangle = nextPath.back();

            bool nextTriangleContainsVertex = triangleContainsVertex(endVertex, nextTriangle);
            bool nextRotationDirectionAligns = (startingRotationDirectionAligns == rotationDirectionAligns(nextTriangle, startVertex, endVertex));
            if (nextTriangleContainsVertex && nextRotationDirectionAligns) {
                verticesToCut.push_back(endVertex);
                trianglePathsToCut.push_back(nextPath);
                stepStartingTriangle = nextTriangle;
                break;
            }

            for (Triangle *triangle : startVertex->triangles) {
                bool sharesEdge = nextTriangle->sharesEdge(triangle);
                bool notChecked = checkedTriangles.find(triangle) == checkedTriangles.end();
                bool containsPrevStartingVertex = triangleContainsVertex(prevStartVertex, triangle);
                bool directionAligns = (startingRotationDirectionAligns == rotationDirectionAligns(triangle, prevStartVertex, startVertex));
                bool doesNotCrossLine = !containsPrevStartingVertex || directionAligns;
                if (sharesEdge && notChecked && doesNotCrossLine) {
                    std::vector<Triangle*> newPotentialPath = nextPath;
                    newPotentialPath.push_back(triangle);
                    potentialPaths.push(newPotentialPath);
                    checkedTriangles.insert(triangle);
                }
            }
        }
    }

    std::unordered_map<Vertex*, Vertex*> oldVertexToNewVertexMap;
    int numVerticesToCut = loopDetected ? verticesToCut.size()-2 : verticesToCut.size();
    for (int i=0; i<numVerticesToCut; i++) {
        Vertex* vertexToCut = verticesToCut[i];
        Vertex* newVertex = new Vertex;
        newVertex->index = vertices.size();
        newVertex->position = vertexToCut->position;
        mesh->addVertex(newVertex);
        tempBoundaryVertices1.push_back(vertexToCut);
        tempBoundaryVertices2.push_back(newVertex);
        vertices = mesh->getVertices();
        oldVertexToNewVertexMap.insert({vertexToCut, newVertex});
    }

    // Updating vertices on triangles, adding new triangles, and removing out of date triangles
    for (std::vector<Triangle*> nextTrianglePath : trianglePathsToCut) {
        for (Triangle *triangle: nextTrianglePath) {
            for (int j = 0; j < 3; j++) {
                Vertex* oldVertex = triangle->vertices[j];
                if (oldVertexToNewVertexMap.find(oldVertex) != oldVertexToNewVertexMap.end()) {
                    Vertex* newVertex = oldVertexToNewVertexMap.at(oldVertex);
                    triangle->vertices[j] = newVertex;
                    newVertex->triangles.insert(triangle);
                    oldVertex->triangles.erase(triangle);
                }
            }
        }
    }

    if (numOpenings == 0) {
        boundariesOverlapping = true;
        boundaryVertices1 = tempBoundaryVertices1;
        boundaryVertices2 = tempBoundaryVertices2;
        boundary1DisplaySize = boundaryVertices1.size();
        boundary2DisplaySize = boundaryVertices2.size();
    } else {
        if (!boundariesReversed) {
            std::reverse(boundaryVertices2.begin(), boundaryVertices2.end());
        }
        if (boundariesAreCombinedLoop) {
            boundary1DisplaySize = boundaryVertices1.size() + boundaryVertices2.size();
        }
        if(!loopDetected) {
            std::reverse(tempBoundaryVertices2.begin(), tempBoundaryVertices2.end());
        }

        boundaryVertices1.insert(boundaryVertices1.end(), boundaryVertices2.begin(), boundaryVertices2.end());
        boundaryVertices2.clear();
        boundaryVertices2.insert(boundaryVertices2.end(), tempBoundaryVertices1.begin(), tempBoundaryVertices1.end());
        boundaryVertices2.insert(boundaryVertices2.end(), tempBoundaryVertices2.begin(), tempBoundaryVertices2.end());
        boundary2DisplaySize = tempBoundaryVertices2.size();
        boundariesOverlapping = false;
    }

    numOpenings += 1;
    drawnVertices.clear();
    mesh->updateNormals();
    boundariesAreCombinedLoop = false;
    geometryEngine->initLine(drawnVertices, drawingColor);
    geometryEngine->initBoundary(boundaryVertices1, boundaryVertices2, boundary1DisplaySize, boundary2DisplaySize, isBoundary1Loop, isBoundary2Loop, boundariesAreCombinedLoop, boundariesReversed, boundariesOverlapping, numOpenings);
    geometryEngine->initMesh(mesh);
    update();
}

void ResultsWidget::deleteSurface(Vertex* vertexToDelete) {
    std::unordered_set<Vertex*> verticesToDeleteSet;
    verticesToDeleteSet.insert(vertexToDelete);

    std::queue<Vertex*> verticesToDelete;
    verticesToDelete.push(vertexToDelete);

    while (!verticesToDelete.empty()) {
        Vertex* nextVertexToDelete = verticesToDelete.front();
        verticesToDelete.pop();

        for (Triangle* triangle : nextVertexToDelete->triangles) {
            for (Vertex* neighbor : triangle->vertices) {
                if (verticesToDeleteSet.find(neighbor) == verticesToDeleteSet.end()) {
                    verticesToDelete.push(neighbor);
                    verticesToDeleteSet.insert(neighbor);
                }
            }
        }
    }

    if (numOpenings > 1) {
        std::vector<Vertex*> newBoundaryVertices1;
        for (auto itr = boundaryVertices1.begin(); itr < boundaryVertices1.end(); itr++) {
            if (verticesToDeleteSet.find(*itr) == verticesToDeleteSet.end()) {
                newBoundaryVertices1.push_back(*itr);
            }
        }
        boundaryVertices1 = newBoundaryVertices1;

        std::vector<Vertex*> newBoundaryVertices2;
        for (auto itr = boundaryVertices2.begin(); itr < boundaryVertices2.end(); itr++) {
            if (verticesToDeleteSet.find(*itr) == verticesToDeleteSet.end()) {
                newBoundaryVertices2.push_back(*itr);
            }
        }
        boundaryVertices2 = newBoundaryVertices2;
    } else if (numOpenings == 1){
        std::vector<Vertex*> remainingVertices;
        if (verticesToDeleteSet.find(boundaryVertices1.back()) == verticesToDeleteSet.end()) {
            remainingVertices = boundaryVertices1;
        } else {
            remainingVertices = boundaryVertices2;
        }

        auto first = remainingVertices.begin();
        auto middle = remainingVertices.begin() + remainingVertices.size() / 2;
        auto last = remainingVertices.end();
        boundaryVertices1 = std::vector<Vertex *>(first, middle);
        boundaryVertices2 = std::vector<Vertex *>(middle, last);
        std::reverse(boundaryVertices2.begin(), boundaryVertices2.end());

        if (isBoundary1Loop && isBoundary2Loop) {
            boundariesAreCombinedLoop = true;
        }

        isBoundary1Loop = false;
        isBoundary2Loop = false;

        boundary1DisplaySize = boundaryVertices1.size();
        boundary2DisplaySize = boundaryVertices2.size();
    }

    boundariesOverlapping = false;
    boundariesReversed = false;
    selectedPoints.clear();
    mesh->deleteVerticesReferences(verticesToDeleteSet);
    mesh->deleteVertices(verticesToDeleteSet);
    geometryEngine->initMesh(mesh);
    geometryEngine->initPoints(selectedPoints);
    geometryEngine->initBoundary(boundaryVertices1, boundaryVertices2, boundary1DisplaySize, boundary2DisplaySize, isBoundary1Loop, isBoundary2Loop, boundariesAreCombinedLoop, boundariesReversed, boundariesOverlapping, numOpenings);
    update();
}

void ResultsWidget::mouseMoveEvent(QMouseEvent *e) {
    if (not isDrawingEnabled or not shouldPaintGL) {
        return;
    }

    Vertex *vertex = getVertexFromMouseEvent(e);
    if (vertex == nullptr) {
        drawnVertices.clear();
    } else if (drawnVertices.empty() || vertex != drawnVertices.back()) {
        addDrawnVertices(vertex);
    }

    if (drawingMode == DrawingMode::click && drawnVertices.size() == 1) {
        setSelectedPoints(vertex);
    } else {
        selectedPoints.clear();
        geometryEngine->initPoints(selectedPoints);
    }

    geometryEngine->initLine(drawnVertices, drawingColor);
    update();
}

Vertex* ResultsWidget::getVertexFromMouseEvent(QMouseEvent *e) {
    bool vertexFound = false;
    float x = (e->position().x() - (width()/2.0)) / (width()/2.0);
    float y = -(e->position().y() - (height()/2.0)) / (height()/2.0);
    std::vector<Vertex*> vertices = mesh->getVertices();
    std::priority_queue<std::tuple<float, float, Vertex*>, std::vector<std::tuple<float, float, Vertex*>>, std::greater<std::tuple<float, float, Vertex*>>> candidateVertices;
    std::vector<float> zDistances;

    for (Vertex* vertex : vertices) {
        QVector3D rotatedVertexPosition = mvpMatrix.map(vertex->position);
        QVector3D rotatedVertexNormal = rotation.rotatedVector(vertex->normal);
        float xyDistance = sqrt(pow(x - rotatedVertexPosition.x(), 2) + pow(y - rotatedVertexPosition.y(), 2));
        if (xyDistance < xyThreshold && rotatedVertexNormal.z() > 0) {
            float zDistance = vertex->position.z();
            zDistances.push_back(zDistance);
            candidateVertices.push({xyDistance, zDistance, vertex});
            vertexFound = true;
        }
    }
    if (vertexFound) {
        std::sort(zDistances.begin(), zDistances.end());
        float zDistanceThreshold = zDistances[(9.0/10.0) * zDistances.size()];
        while (!candidateVertices.empty()) {
            std::tuple<float, float, Vertex *> candidateVertex = candidateVertices.top();
            candidateVertices.pop();
            if (std::get<1>(candidateVertex) >= zDistanceThreshold) {
                return std::get<2>(candidateVertex);
            }
        }
    }
    return nullptr;
}

void ResultsWidget::addDrawnVertices(Vertex *newVertex) {
    std::vector<Vertex*> newVertices;
    if (drawnVertices.size() == 0) {
        newVertices= {newVertex};
    } else {
        std::tuple<float, std::vector<Vertex*>> verticesPathAndDistance = getVerticesPathAndDistance(newVertex, drawnVertices.back());
        newVertices = std::get<1>(verticesPathAndDistance);
    }
    for (int i = newVertices.size() - 1; i > -1; i--) {
        if (drawnVertices.size() > 2 && newVertices[i] == drawnVertices[drawnVertices.size()-2]) {
            drawnVertices.pop_back();
        } else {
            bool loopFound = drawnVerticesContainLoop(newVertices[i]);
            drawnVertices.push_back(newVertices[i]);
            if (loopFound) {
                break;
            }
        }
    }
}

std::tuple<float, std::vector<Vertex*>> ResultsWidget::getVerticesPathAndDistance(Vertex *startVertex, Vertex *endVertex) {
    std::vector<Vertex*> path = {startVertex};
    std::unordered_set<Vertex*> checkedVertices;
    std::priority_queue<std::tuple<float, std::vector<Vertex*>>, std::vector<std::tuple<float, std::vector<Vertex*>>>, std::greater<std::tuple<float, std::vector<Vertex*>>> > potentialPaths;
    potentialPaths.push({0.0, path});
    QVector3D endVertexPosition = endVertex->position;

    while(!potentialPaths.empty()) {
        std::tuple<float, std::vector<Vertex*>> nextDistanceAndPath = potentialPaths.top();
        float oldDistance = std::get<0>(nextDistanceAndPath);
        std::vector<Vertex*> nextPath = std::get<1>(nextDistanceAndPath);
        Vertex *nextVertex = nextPath.back();
        potentialPaths.pop();

        for (Triangle* triangle : nextVertex->triangles) {
            for (Vertex *neighbor : triangle->vertices) {
                if (neighbor->position == endVertexPosition) {
                    float distance = oldDistance + euclideanDistance(neighbor, nextVertex);
                    return {distance, nextPath};
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
    return {0.0, path};;
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
            isDrawingEnabled = false;
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

    if (shouldPaintGL and numSmoothingStepsSoFar < numSmoothingSteps) {
        implicitSmooth();
        shouldUpdate = true;
        numSmoothingStepsSoFar += 1;
    } else if (numSmoothingStepsSoFar == numSmoothingSteps){
        numSmoothingStepsSoFar = 0;
        numSmoothingSteps = 0;
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

    isDrawingEnabled = true;
    shouldPaintGL = false;
    findVertexToDelete = false;
    findVertexToSmooth = false;
    boundariesReversed = false;
    isBoundary1Loop = false;
    isBoundary2Loop = false;
    boundariesAreCombinedLoop = false;
    boundariesOverlapping = false;
    drawingMode = DrawingMode::drag;
    numOpenings = 0;
    boundary1DisplaySize = 0;
    boundary2DisplaySize = 0;
    numSmoothingSteps = 0;
    numSmoothingStepsSoFar = 0;
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
        mvpMatrix = projection * matrix;

        // Set modelview-projection matrix
        program.setUniformValue("mvp_matrix", projection * matrix);

        // Draw geometry
        geometryEngine->drawMesh(&program);
        geometryEngine->drawLine(&program);
        geometryEngine->drawBoundary(&program);
        geometryEngine->drawPoints(&program);
    }
}

void ResultsWidget::setDrawingColor(QColor newColor) {
    drawingColor = newColor;
}

void ResultsWidget::setDrawingMode(DrawingMode newDrawingMode) {
    drawingMode = newDrawingMode;
    drawnVertices.clear();
    geometryEngine->initLine(drawnVertices, drawingColor);
    update();
}

void ResultsWidget::glue() {
    if (numOpenings < 1) {
        return;
    }

    connectVertices();

    if (numOpenings == 1) {
        numOpenings -= 1;
    } else {
        numOpenings -= 2;
    }

    boundaryVertices1.clear();
    boundaryVertices2.clear();
    isBoundary1Loop = false;
    isBoundary2Loop = false;
    boundariesReversed = false;
    boundariesAreCombinedLoop = false;
    boundariesOverlapping = false;

    mesh->updateNormals();
    geometryEngine->initBoundary(boundaryVertices1, boundaryVertices2,boundary1DisplaySize, boundary2DisplaySize, isBoundary1Loop, isBoundary2Loop, boundariesAreCombinedLoop, boundariesReversed, boundariesOverlapping, numOpenings);
    geometryEngine->initMesh(mesh);
    update();
}

std::vector<Vertex*> ResultsWidget::connectFirstAndLastVertices(std::vector<Vertex*> boundary) {
    Vertex* firstVertex = boundary.front();
    Vertex* lastVertex = boundary.back();
    for (Triangle *triangle: lastVertex->triangles) {
        for (int k = 0; k < 3; k++) {
            if (triangle->vertices[k] == lastVertex) {
                triangle->vertices[k] = firstVertex;
                firstVertex->triangles.insert(triangle);
            }
        }
    }
    boundary.pop_back();
    mesh->deleteVertices({lastVertex});
    return boundary;
}

std::vector<Vertex*> ResultsWidget::connectMiddleVertices(std::vector<Vertex*> boundary) {
    auto midItr = boundary.begin() + boundary.size()/2 - 1;
    Vertex* middleVertex = *midItr;
    Vertex* nextMiddleVertex = *(midItr + 1);
    for (Triangle *triangle: nextMiddleVertex->triangles) {
        for (int k = 0; k < 3; k++) {
            if (triangle->vertices[k] == nextMiddleVertex) {
                triangle->vertices[k] = middleVertex;
                middleVertex->triangles.insert(triangle);
            }
        }
    }
    boundary.erase(midItr + 1);
    mesh->deleteVertices({nextMiddleVertex});
    return boundary;
}

void ResultsWidget::connectVertices() {
    std::vector<Vertex*> smallerBoundaryVertices;
    std::vector<Vertex*> largerBoundaryVertices;
    std::unordered_set<Vertex*> verticesToDelete;

    if (boundaryVertices1.size() > boundaryVertices2.size()) {
        largerBoundaryVertices = boundaryVertices1;
        smallerBoundaryVertices = boundaryVertices2;
    } else {
        largerBoundaryVertices = boundaryVertices2;
        smallerBoundaryVertices = boundaryVertices1;
    }

    if(numOpenings == 2 && !isBoundary1Loop && !isBoundary2Loop) {
        largerBoundaryVertices = connectMiddleVertices(largerBoundaryVertices);
        largerBoundaryVertices = connectFirstAndLastVertices(largerBoundaryVertices);
        smallerBoundaryVertices = connectMiddleVertices(smallerBoundaryVertices);
        smallerBoundaryVertices = connectFirstAndLastVertices(smallerBoundaryVertices);
    }

    int i=0;
    for (int j=0; j<smallerBoundaryVertices.size(); j++) {
        Vertex* smallerBoundaryVertex = smallerBoundaryVertices[j];
        float smallerBoundaryPercentageCompleted = float(j)/(smallerBoundaryVertices.size()-1);
        float largerBoundaryPercentageCompleted = float(i)/(largerBoundaryVertices.size()-1);
        while (largerBoundaryPercentageCompleted <= smallerBoundaryPercentageCompleted) {
            Vertex* largerBoundaryVertex = largerBoundaryVertices[i];
            for (Triangle* triangle : largerBoundaryVertex->triangles) {
                for (int k=0; k<3; k++) {
                    if (triangle->vertices[k] == largerBoundaryVertex) {
                        triangle->vertices[k] = smallerBoundaryVertex;
                        smallerBoundaryVertex->triangles.insert(triangle);
                        verticesToDelete.insert(largerBoundaryVertex);
                    }
                }
            }
            i += 1;
            largerBoundaryPercentageCompleted = float(i)/(largerBoundaryVertices.size()-1);
        }
    }

    if (boundariesReversed && !isBoundary1Loop && !isBoundary2Loop && !boundariesAreCombinedLoop && numOpenings == 1) {
        connectFirstAndLastVertices(smallerBoundaryVertices);
    }

    mesh->deleteVertices(verticesToDelete);
    mesh->deleteOutdatedTriangles();
}

void ResultsWidget::reverseBoundaries() {
    boundariesReversed = !boundariesReversed;
    std::reverse(boundaryVertices2.begin(), boundaryVertices2.end());
    geometryEngine->initBoundary(boundaryVertices1, boundaryVertices2, boundary1DisplaySize, boundary2DisplaySize, isBoundary1Loop, isBoundary2Loop, boundariesAreCombinedLoop, boundariesReversed, boundariesOverlapping, numOpenings);
    update();
}

void ResultsWidget::findVerticesToSmooth(Vertex *vertexToSmooth) {
    std::unordered_set<Vertex*> verticesToSmoothSet;
    verticesToSmoothSet.insert(vertexToSmooth);
    verticesToSmooth.push_back(vertexToSmooth);
    verticesToSmoothMap.insert({vertexToSmooth, verticesToSmoothMap.size()});

    std::queue<Vertex*> verticesToSmoothQueue;
    verticesToSmoothQueue.push(vertexToSmooth);

    for (Vertex* vertex : drawnVertices) {
        verticesToSmoothSet.insert(vertex);
    }

    while (!verticesToSmoothQueue.empty()) {
        Vertex* nextVertexToSmooth = verticesToSmoothQueue.front();
        verticesToSmoothQueue.pop();

        for (Triangle* triangle : nextVertexToSmooth->triangles) {
            for (Vertex* neighbor : triangle->vertices) {
                if (verticesToSmoothSet.find(neighbor) == verticesToSmoothSet.end()) {
                    verticesToSmoothQueue.push(neighbor);
                    verticesToSmoothSet.insert(neighbor);
                    verticesToSmooth.push_back(neighbor);
                    verticesToSmoothMap.insert({neighbor, verticesToSmoothMap.size()});
                }
            }
        }
    }
}

void ResultsWidget::toggleSmoothSurface() {
    if (!findVertexToSmooth) {
        isDrawingEnabled = false;
        verticesToSmooth.clear();
        verticesToSmoothMap.clear();
    } else {
        if (!selectedPoints.empty()) {
            findVerticesToSmooth(selectedPoints.front());
            numSmoothingSteps = 30;
            numSmoothingStepsSoFar = 0;
        }
        isDrawingEnabled = true;
        selectedPoints.clear();
        geometryEngine->initPoints(selectedPoints);
    }

    findVertexToSmooth = !findVertexToSmooth;
    update();
}

void ResultsWidget::implicitSmooth() {
    std::vector<double> bX, bY, bZ;
    for (Vertex* vertex: verticesToSmooth) {
        bX.push_back(vertex->position.x());
        bY.push_back(vertex->position.y());
        bZ.push_back(vertex->position.z());
    }

    double stepSize = 1.0;
    int numVertices = verticesToSmooth.size();
    SparseMat* matrixA = new SparseMat(numVertices, numVertices, 0);
    for (Vertex* vertex: verticesToSmooth) {
        float neighborDistanceSum = 0.0;
        std::unordered_set<Vertex *> visitedNeighbors;
        std::vector<Vertex *> neighbors;
        for (Triangle *triangle: vertex->triangles) {
            for (Vertex *neighbor: triangle->vertices) {
                if (verticesToSmoothMap.find(neighbor) != verticesToSmoothMap.end() && visitedNeighbors.find(neighbor) == visitedNeighbors.end()) {
                    neighborDistanceSum += euclideanDistance(vertex, neighbor);
                    visitedNeighbors.insert(neighbor);
                    neighbors.push_back(neighbor);
                }
            }
        }
        std::sort(neighbors.begin(), neighbors.end(), [this](Vertex* v1, Vertex* v2) {return verticesToSmoothMap.at(v1) < verticesToSmoothMap.at(v2); });

        int vertexIndex = verticesToSmoothMap.at(vertex);
        for (Vertex *neighbor: neighbors) {
            if (vertex == neighbor) {
                //matrixA->vals.push_back(1.0 + stepSize * (neighbors.size() - 1));
                matrixA->vals.push_back(1.0 + stepSize * (neighborDistanceSum));
            } else {
                //matrixA->vals.push_back(-stepSize);
                matrixA->vals.push_back(-stepSize * euclideanDistance(vertex, neighbor));
            }
            int neighborIndex = verticesToSmoothMap.at(neighbor);
            matrixA->rowIndices.push_back(neighborIndex);
            if (matrixA->vals.size() - 1 <= matrixA->colFirstIndices[vertexIndex]) {
                matrixA->colFirstIndices[vertexIndex] = matrixA->vals.size() - 1;
            }
        }
    }
    matrixA->colFirstIndices[matrixA->colFirstIndices.size()-1] = matrixA->vals.size();
    std::vector<double> newXPositions = biconjugateGradientMethod(matrixA, bX, 0.0001, 10);
    std::vector<double> newYPositions = biconjugateGradientMethod(matrixA, bY, 0.0001, 10);
    std::vector<double> newZPositions = biconjugateGradientMethod(matrixA, bZ, 0.0001, 10);

    for (int i=0; i<verticesToSmooth.size(); i++) {
        verticesToSmooth[i]->position.setX(newXPositions[i]);
        verticesToSmooth[i]->position.setY(newYPositions[i]);
        verticesToSmooth[i]->position.setZ(newZPositions[i]);
    }

    mesh->updateNormals();
    geometryEngine->initMesh(mesh);
    geometryEngine->initLine(drawnVertices, drawingColor);
    geometryEngine->initBoundary(boundaryVertices1, boundaryVertices2, boundary1DisplaySize, boundary2DisplaySize, isBoundary1Loop, isBoundary2Loop, boundariesAreCombinedLoop, boundariesReversed, boundariesOverlapping, numOpenings);
    update();
}

void ResultsWidget::explicitSmooth() {
    float stepSize = 0.5;
    std::vector<QVector3D> newPositions;
    std::vector<Vertex *> vertices = mesh->getVertices();

    for (Vertex *vertex: verticesToSmooth) {
        float xDiff = 0.0;
        float yDiff = 0.0;
        float zDiff = 0.0;
        int numNeighbors = 1;
        std::unordered_set<Vertex *> visitedNeighbors;

        // Initializing uniform weights
        for (Triangle *triangle : vertex->triangles) {
            for (Vertex *neighbor : triangle->vertices) {
                if (neighbor != vertex && visitedNeighbors.find(neighbor) == visitedNeighbors.end()) {
                    numNeighbors += 1;
                    visitedNeighbors.insert(neighbor);
                }
            }
        }

        visitedNeighbors.clear();
        for (Triangle *triangle: vertex->triangles) {
            for (Vertex *neighbor : triangle->vertices) {
                if (neighbor != vertex && visitedNeighbors.find(neighbor) == visitedNeighbors.end()) {
                    float weight = 1.0 / numNeighbors;
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

    for (int j = 0; j < verticesToSmooth.size(); j++) {
        verticesToSmooth[j]->position = newPositions[j];
    }

    mesh->updateNormals();
    geometryEngine->initMesh(mesh);
    geometryEngine->initLine(drawnVertices, drawingColor);
    geometryEngine->initBoundary(boundaryVertices1, boundaryVertices2, boundary1DisplaySize, boundary2DisplaySize, isBoundary1Loop, isBoundary2Loop, boundariesAreCombinedLoop, boundariesReversed, boundariesOverlapping, numOpenings);
    update();
}

std::string ResultsWidget::getResultsAttributesLabelText() {
    std::set<int> uniqueVertices;
    std::set<std::set<int>> uniqueEdges;
    std::set<std::set<int>> uniqueFaces;

    std::vector<Triangle*> triangles = mesh->getTriangles();
    int numTriangles = triangles.size();
    for (int i=0; i<numTriangles; i++) {
        int index0 = triangles[i]->vertices[0]->index;
        int index1 = triangles[i]->vertices[1]->index;
        int index2 = triangles[i]->vertices[2]->index;

        uniqueVertices.insert(index0);
        uniqueVertices.insert(index1);
        uniqueVertices.insert(index2);

        uniqueEdges.insert({index0, index1});
        uniqueEdges.insert({index1, index2});
        uniqueEdges.insert({index2, index0});

        uniqueFaces.insert({index0, index1, index2});
    }

    int numUniqueVertices = uniqueVertices.size();
    int numUniqueEdges = uniqueEdges.size();
    int numUniqueFaces = uniqueFaces.size();
    int eulerCharacteristic = numUniqueVertices - numUniqueEdges + numUniqueFaces;
    return " Vertices: " + std::to_string(numUniqueVertices) + "\n Edges: " + std::to_string(numUniqueEdges) + "\n Faces: " + std::to_string(numUniqueFaces) + "\n Euler Characteristic: " + std::to_string(eulerCharacteristic);
}


void ResultsWidget::reset() {
    drawnVertices.clear();
    verticesToSmooth.clear();
    verticesToSmoothMap.clear();
    boundaryVertices1.clear();
    boundaryVertices2.clear();
    selectedPoints.clear();
    boundariesReversed = false;
    isBoundary1Loop = false;
    isBoundary2Loop = false;
    boundariesAreCombinedLoop = false;
    boundariesOverlapping = false;
    numOpenings = 0;
    numSmoothingSteps = 0;
    numSmoothingStepsSoFar = 0;

    geometryEngine->initPoints(selectedPoints);
    geometryEngine->initLine(drawnVertices, drawingColor);
    geometryEngine->initBoundary(boundaryVertices1, boundaryVertices2, boundary1DisplaySize, boundary2DisplaySize, isBoundary1Loop, isBoundary2Loop, boundariesAreCombinedLoop, boundariesReversed, boundariesOverlapping, numOpenings);
    update();
}

std::vector<double> ResultsWidget::solveEquation(SparseMat* matrixA, std::vector<double> b) {
    double diag;
    int size = b.size();
    std::vector<double> x;
    for (int i=0; i<size; i++) {
        diag = 0.0;
        for (int j=matrixA->colFirstIndices[i]; j<matrixA->colFirstIndices[i+1]; j++) {
            if (matrixA->rowIndices[j] == i) {
                diag = matrixA->vals[j];
                break;
            }
        }
        if (diag != 0.0) {
            x.push_back(b[i]/diag);
        } else {
            x.push_back(b[i]);
        }
    }
    return x;
}

double ResultsWidget::computeNorm(std::vector<double> b) {
    double sum = 0.0;
    for (int i=0; i<b.size(); i++) {
        sum += pow(b[i], 2);
    }
    return sqrt(sum);
}

std::vector<double> ResultsWidget::biconjugateGradientMethod(SparseMat* matrixA, std::vector<double> b, double tolerance, int maxIters) {
    int size = b.size();
    int iter = 0;
    double error = 0.0;
    double ak = 0.0;
    double akDen = 1.0;
    double bk = 0.0;
    double bkDen = 1.0;
    double bkNum = 0.0;
    std::vector<double> p(size);
    std::vector<double> pp(size);
    std::vector<double> z(size);
    std::vector<double> zz(size);
    std::vector<double> r = b;
    std::vector<double> rr = b;

    std::vector<double> x;
    for (int i=0; i<size; i++) {
        x.push_back(0.0);
    }

    double bNorm = computeNorm(b);
    z = solveEquation(matrixA, r);

    while (iter < maxIters) {
        iter++;

        // Technically matrixA should be transposed here
        zz = solveEquation(matrixA, rr);

        bkNum = 0.0;
        for (int i=0; i<size; i++) {
            bkNum += z[i]*rr[i];
        }

        if (iter == 1) {
            for (int i=0; i<size; i++) {
                p[i] = z[i];
                pp[i] = z[i];
            }
        } else {
            bk = bkNum/bkDen;
            for (int i=0; i<size; i++) {
                p[i] = bk * p[i] + z[i];
                pp[i] = bk * pp[i] + zz[i];
            }
        }

        bkDen = bkNum;
        z = matrixA->multiply(p);

        akDen = 0.0;
        for (int i = 0; i<size; i++) {
            akDen += z[i]*pp[i];
        }

        ak = bkNum/akDen;
        zz = matrixA->multiplyInverse(pp);

        for (int i=0; i<size; i++) {
            x[i] += ak*p[i];
            r[i] -= ak*z[i];
            rr[i] -= ak*zz[i];
        }
        z = solveEquation(matrixA, r);
        error = computeNorm(r)/bNorm;
        if (error <= tolerance) {
            break;
        }
    }
    return x;
}
