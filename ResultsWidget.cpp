#include "ResultsWidget.h"
#include <QMouseEvent>
#include <QtWidgets>
#include <cmath>
#include <limits>
#include <queue>
#include <tuple>
#include <iostream>

ResultsWidget::~ResultsWidget() {
    // Make sure the context is current when deleting the buffers.
    makeCurrent();
    delete geometryEngine;
    doneCurrent();
}

void ResultsWidget::addSurface(surface newSurface) {
    shouldPaintGL = true;
    loopDetected = false;
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

void ResultsWidget::toggleCutting() {
    cuttingEnabled = !cuttingEnabled;
}

void ResultsWidget::mousePressEvent(QMouseEvent *e) {
    if (cuttingEnabled && loopDetected) {
        cutSurface(e);
    } else {
        //TODO: Uncomment and comment out clear() for testing
        //mouseMoveEvent(e);
        lineVertices.clear();
        isDrawingMode = true;
    }
}

void ResultsWidget::cutSurface(QMouseEvent *e) {
    VertexData *startingVertex = getVertexFromMouseEvent(e);
    if (startingVertex == nullptr) {
        return;
    }


    std::unordered_map<std::string, int> verticesToNotCut;
    for (VertexData* vertex : lineVertices) {
        verticesToNotCut.insert({vertex->toString(), 0});
    }

    if (verticesToNotCut.find(startingVertex->toString()) != verticesToNotCut.end()) {
        return;
    }

    std::queue<VertexData*> verticesToCut;
    verticesToCut.push(startingVertex);

    while (!verticesToCut.empty()) {
        VertexData* vertexToCut = verticesToCut.front();
        verticesToCut.pop();

        for (VertexData* neighbor : vertexToCut->neighbors) {
            std::string neighborString = neighbor->toString();
            if (verticesToNotCut.find(neighborString) == verticesToNotCut.end()) {
                verticesToCut.push(neighbor);
                verticesToNotCut.insert({neighborString, 0});
            }
        }
        mesh->cutVertex(vertexToCut);
    }
    geometryEngine->initMesh(mesh);
    update();
}

void ResultsWidget::mouseMoveEvent(QMouseEvent *e) {
    if (not isDrawingMode or not shouldPaintGL) {
        return;
    }

    //TODO: Comment for testing
    loopDetected = false;

    VertexData *vertex = getVertexFromMouseEvent(e);
    if (vertex == nullptr) {
        lineVertices.clear();
    } else if (lineVertices.size() == 0 || vertex != lineVertices.back()) {
        addLineVertices(vertex);
    }

    geometryEngine->initLine(lineVertices);
    update();
}

VertexData* ResultsWidget::getVertexFromMouseEvent(QMouseEvent *e) {
    VertexData *closestVertex = new VertexData;
    bool surfaceVertexFound = false;
    float smallestZDistance = std::numeric_limits<float>::max();
    std::vector<VertexData*> vertices = mesh->getVertices();
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

void ResultsWidget::addLineVertices(VertexData *newVertex) {
    std::vector<VertexData *> newVertices = getNewVertices(newVertex);
    for (int i = newVertices.size() - 1; i > -1; i--) {
        checkLineVerticesForLoop(newVertices[i]);
        lineVertices.push_back(newVertices[i]);
    }
}

std::vector<VertexData*> ResultsWidget::getNewVertices(VertexData *newVertex) {
    std::vector<VertexData*> newVertices = {newVertex};
    if (lineVertices.size() == 0) {
        return newVertices;
    }

    std::unordered_map<VertexData*, int> checkedVertices;
    std::queue<std::vector<VertexData*>> potentialPaths;
    potentialPaths.push(newVertices);
    QVector3D lastVertexPosition = lineVertices.back()->position;

    while(!potentialPaths.empty()) {
        std::vector<VertexData*> nextPath = potentialPaths.front();
        VertexData *nextVertex = nextPath.back();
        potentialPaths.pop();

        if(checkedVertices.find(nextVertex) != checkedVertices.end()) {
            continue;
        }
        checkedVertices.insert({nextVertex, 0});

        for (VertexData* neighbor : nextVertex->neighbors) {
            if (neighbor->position == lastVertexPosition) {
                return nextPath;
            } else if(checkedVertices.find(neighbor) == checkedVertices.end()) {
                std::vector<VertexData*> newPotentialPath = nextPath;
                newPotentialPath.push_back(neighbor);
                potentialPaths.push(newPotentialPath);
            }
        }
    }
    std::cout << "No connecting path found between drawing vertices using BFS." << std::endl;
}

void ResultsWidget::checkLineVerticesForLoop(VertexData *newVertex) {
    //TODO: Change to 3 for testing
    int numRecentVerticesToExcludeForLoopChecking = 5;
    if (lineVertices.size() < numRecentVerticesToExcludeForLoopChecking) {
        return;
    }

    for (int i=0; i<=lineVertices.size()-numRecentVerticesToExcludeForLoopChecking; i++) {
        if (lineVertices[i] == newVertex) {
            isDrawingMode = false;
            loopDetected = true;
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
    loopDetected = false;
    cuttingEnabled = false;
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
