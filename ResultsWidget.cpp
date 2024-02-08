#include "ResultsWidget.h"
#include <QMouseEvent>
#include <QtWidgets>
#include <cmath>
#include <limits>
#include <queue>
#include <unordered_set>

ResultsWidget::~ResultsWidget() {
    // Make sure the context is current when deleting the buffers.
    makeCurrent();
    delete geometryEngine;
    doneCurrent();
}

void ResultsWidget::addSurface(surface newSurface) {
    shouldPaintGL = true;
    lineVertices.clear();
    QVector3D centerPosition = {0, 0, 0};
    if (newSurface == surface::cube) {
        QVector3D scale = {1.0, 0.0, 0.0};
        mesh->resetSurface(new Cube(centerPosition, scale));
    } else if (newSurface == surface::sphere) {
        QVector3D scale = {1.3, 1.3, 1.3};
        mesh->resetSurface(new Sphere(centerPosition, scale));
    } else if (newSurface == surface::torus) {
        QVector3D scale = {1.0, 0.5, 0.0};
        mesh->resetSurface(new Torus(centerPosition, scale));
    } else if (newSurface == surface::mobiusStrip) {
        QVector3D scale = {1.0, 0.0, 0.0};
        mesh->resetSurface(new MobiusStrip(centerPosition, scale));
    } else if (newSurface == surface::crossCap) {
        QVector3D scale = {1.5, 1.0, 1.0};
        mesh->resetSurface(new CrossCap(centerPosition, scale));
    } else if (newSurface == surface::kleinBottle) {
        QVector3D scale = {0.5, 1.0, 0.5};
        mesh->resetSurface(new KleinBottle(centerPosition, scale));
    }

    if (shouldAnimate) {
        geometryEngine->initAnimation(mesh);
    } else {
        geometryEngine->initMesh(mesh);
    }

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
        // TODO: Switch comments for testing
        //mouseMoveEvent(e);
        lineVertices.clear();
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
    if (lineVertices.size() < 2) {
        return;
    }

    Triangle* startingTriangle;
    Triangle* stepStartingTriangle;
    std::vector<Vertex*> vertices = mesh->getVertices();
    std::vector<Vertex*> verticesToCut;
    std::vector<std::vector<Triangle*>> trianglePathsToCut;
    for (Triangle *triangle : lineVertices[0]->triangles) {
        if(triangleContainsVertex(lineVertices[0], triangle, vertices)
              && triangleContainsVertex(lineVertices[1], triangle, vertices)) {
            startingTriangle = triangle;
            break;
        }
    }

    stepStartingTriangle = startingTriangle;
    verticesToCut.push_back(lineVertices[1]);
    trianglePathsToCut.push_back({stepStartingTriangle});
    bool startingRotationDirectionAligns = rotationDirectionAligns(startingTriangle, lineVertices[0], lineVertices[1], vertices);

    // Required for loops
    if (lineVertices.front() == lineVertices.back()) {
        lineVertices.push_back(lineVertices[1]);
    }

    // Find all triangles that need to be cut
    for (int i=1; i<lineVertices.size()-1; i++) {
        Vertex* prevStartVertex = lineVertices[i-1];
        Vertex* startVertex = lineVertices[i];
        Vertex* endVertex = lineVertices[i+1];

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
    std::unordered_map<Vertex*, int> oldVertexToNewIndexMap;
    for (int i=0; i<verticesToCut.size(); i++) {
        Vertex* vertexToCut = verticesToCut[i];
        Vertex* newVertex = new Vertex;
        newVertex->position = vertexToCut->position;
        mesh->addVertex(newVertex);
        vertices = mesh->getVertices();
        oldVertexToNewIndexMap.insert({vertexToCut, vertices.size()-1});
        for (Triangle* nextTriangle : trianglePathsToCut[i]) {
            for (int j=0; j<3; j++) {
                int oldVertexIndex = nextTriangle->vertexIndices[j];
                if (vertexToCut == vertices[oldVertexIndex]) {
                    oldIndexToNewIndexMap.insert({oldVertexIndex, vertices.size()-1});
                }
            }
        }
    }

    // Updating vertices on triangles and adding new triangles
    for (std::vector<Triangle*> nextTrianglePath : trianglePathsToCut) {
        for (Triangle *triangle: nextTrianglePath) {
            for (int j = 0; j < 3; j++) {
                int oldVertexIndex = triangle->vertexIndices[j];
                if (oldIndexToNewIndexMap.find(oldVertexIndex) != oldIndexToNewIndexMap.end()) {
                    int newVertexIndex = oldIndexToNewIndexMap.at(oldVertexIndex);
                    triangle->vertexIndices[j] = newVertexIndex;
                    vertices[newVertexIndex]->triangles.insert(triangle);
                }
            }
        }
    }

    // Removing old triangles that are no longer accurate
    for (Vertex* vertex : verticesToCut) {
        int newIndex = oldVertexToNewIndexMap.at(vertex);
        std::set<Triangle *> newTriangles;
        for (Triangle* triangle : vertex->triangles) {
            int index1 = triangle->vertexIndices[0];
            int index2 = triangle->vertexIndices[1];
            int index3 = triangle->vertexIndices[2];

            if (index1 != newIndex && index2 != newIndex && index3 != newIndex) {
                newTriangles.insert(triangle);
            }
        }
        vertex->triangles = newTriangles;
    }

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
    update();
}

void ResultsWidget::mouseMoveEvent(QMouseEvent *e) {
    if (not isDrawingMode or not shouldPaintGL) {
        return;
    }

    Vertex *vertex = getVertexFromMouseEvent(e);
    if (vertex == nullptr) {
        // TODO: comment out for testing
        lineVertices.clear();
    } else if (lineVertices.size() == 0 || vertex != lineVertices.back()) {
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
        if (xyDistance < 0.01 && surfacePosition.z() < smallestZDistance) {
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
        if (lineVertices.size() > 2 && newVertices[i] == lineVertices[lineVertices.size()-2]) {
            lineVertices.pop_back();
        }
        else {
            checkLineVerticesForLoop(newVertices[i]);
            lineVertices.push_back(newVertices[i]);
        }
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
    std::vector<Vertex*> meshVertices = mesh->getVertices();

    while(!potentialPaths.empty()) {
        std::vector<Vertex*> nextPath = potentialPaths.front();
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
                    potentialPaths.push(newPotentialPath);
                    checkedVertices.insert(neighbor);
                }
            }
        }
    }
    return {newVertex};
}

void ResultsWidget::checkLineVerticesForLoop(Vertex *newVertex) {
    for (int i=0; i<lineVertices.size(); i++) {
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

void ResultsWidget::glue() {

}

void ResultsWidget::glueAnimation() {
    //shouldAnimate = !shouldAnimate;

    float sumX = 0.0;
    float sumY = 0.0;
    float sumZ = 0.0;
    float maxX = std::numeric_limits<float>::min();;
    float maxY = std::numeric_limits<float>::min();;
    float maxZ = std::numeric_limits<float>::min();;
    float minX = std::numeric_limits<float>::max();;
    float minY = std::numeric_limits<float>::max();;
    float minZ = std::numeric_limits<float>::max();;

    for (Vertex* vertex : lineVertices) {
        float x = vertex->position.x();
        float y = vertex->position.y();
        float z = vertex->position.z();

        maxX = std::max(x, maxX);
        maxY = std::max(y, maxY);
        maxZ = std::max(z, maxZ);
        minX = std::min(x, minX);
        minY = std::min(y, minY);
        minZ = std::min(z, minZ);
        sumX += x;
        sumY += y;
        sumZ += z;
    }

    float avgX = sumX / lineVertices.size();
    float avgY = sumY / lineVertices.size();
    float avgZ = sumZ / lineVertices.size();
    float xRange = maxX - minX;
    float yRange = maxY - minY;
    float zRange = maxZ - minZ;
    QVector3D centerPosition = QVector3D(avgX, avgY, avgZ);
    QVector3D scale = {xRange, yRange, zRange};
    //mesh->addSurface(new CrossCap(centerPosition, scale));
    //geometryEngine->initMesh(mesh);
    //update();
}
