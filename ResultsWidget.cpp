#include "ResultsWidget.h"
#include <QMouseEvent>
#include <QtWidgets>
#include <iostream>

ResultsWidget::~ResultsWidget() {
    // Make sure the context is current when deleting the buffers.
    makeCurrent();
    delete cubeGeometryEngine;
    delete sphereGeometryEngine;
    delete torusGeometryEngine;
    delete mobiusStripEngine;
    delete kleinBottleEngine;
    doneCurrent();
}

void ResultsWidget::setShouldPaintGL(bool newShouldPaintGL) {
    shouldPaintGL = newShouldPaintGL;
    if (geometryEngine == cubeGeometryEngine) {
        geometryEngine = sphereGeometryEngine;
    } else if (geometryEngine == sphereGeometryEngine) {
        geometryEngine = torusGeometryEngine;
    } else if (geometryEngine == torusGeometryEngine) {
        geometryEngine = mobiusStripEngine;
    } else if (geometryEngine == mobiusStripEngine) {
        geometryEngine = crossCapEngine;
    } else if (geometryEngine == crossCapEngine) {
        geometryEngine = kleinBottleEngine;
    } else {
        geometryEngine = cubeGeometryEngine;
    }
    update();
}

void ResultsWidget::mousePressEvent(QMouseEvent *e) {
    // Save mouse press position
    mousePressPosition = QVector2D(e->position());
}

void ResultsWidget::mouseMoveEvent(QMouseEvent *e) {

}

void ResultsWidget::wheelEvent(QWheelEvent *e) {
    // Rotation axis is perpendicular to the angle delta difference vector
    QVector3D n = QVector3D(e->angleDelta().y()/2.0, e->angleDelta().x()/2.0, 0.0).normalized();

    // Accelerate angular speed relative to the length of the mouse sweep
    qreal acc = 0.12;

    // Calculate new rotation axis as weighted sum
    rotationAxis = (rotationAxis * angularSpeed - n * acc).normalized();

    // Increase angular speed
    angularSpeed += acc;
}

void ResultsWidget::timerEvent(QTimerEvent *) {
    // Decrease angular speed (friction)
    angularSpeed *= 0.92;

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

    cube = new Cube();
    sphere = new Sphere();
    torus = new Torus();
    mobiusStrip = new MobiusStrip();
    kleinBottle = new KleinBottle();
    crossCap = new CrossCap();
    cubeGeometryEngine = new GeometryEngine(cube);
    sphereGeometryEngine = new GeometryEngine(sphere);
    torusGeometryEngine = new GeometryEngine(torus);
    mobiusStripEngine = new GeometryEngine(mobiusStrip);
    kleinBottleEngine = new GeometryEngine(kleinBottle);
    crossCapEngine = new GeometryEngine(crossCap);
    geometryEngine = cubeGeometryEngine;

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

        // Set modelview-projection matrix
        program.setUniformValue("mvp_matrix", projection * matrix);

        // Draw cube geometry
        geometryEngine->drawGeometry(&program);
    }
}
