#include "ResultsWidget.h"
#include <QMouseEvent>
#include <QtWidgets>

ResultsWidget::~ResultsWidget() {
    // Make sure the context is current when deleting the buffers.
    makeCurrent();
    delete cubeEngine;
    delete sphereEngine;
    delete torusEngine;
    delete mobiusStripEngine;
    delete crossCapEngine;
    delete kleinBottleEngine;
    doneCurrent();
}

void ResultsWidget::addSurface(surface newSurface) {
    shouldPaintGL = true;
    if (newSurface == surface::cube) {
        geometryEngine = cubeEngine;
    } else if (newSurface == surface::sphere) {
        geometryEngine = sphereEngine;
    } else if (newSurface == surface::torus) {
        geometryEngine = torusEngine;
    } else if (newSurface == surface::mobiusStrip) {
        geometryEngine = mobiusStripEngine;
    } else if (newSurface == surface::crossCap) {
        geometryEngine = crossCapEngine;
    } else if (newSurface == surface::kleinBottle) {
        geometryEngine = kleinBottleEngine;
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

    cubeSurface = new Cube();
    sphereSurface = new Sphere();
    torusSurface = new Torus();
    mobiusStripSurface = new MobiusStrip();
    kleinBottleSurface = new KleinBottle();
    crossCapSurface = new CrossCap();
    cubeEngine = new GeometryEngine(cubeSurface);
    sphereEngine = new GeometryEngine(sphereSurface);
    torusEngine = new GeometryEngine(torusSurface);
    mobiusStripEngine = new GeometryEngine(mobiusStripSurface);
    kleinBottleEngine = new GeometryEngine(kleinBottleSurface);
    crossCapEngine = new GeometryEngine(crossCapSurface);

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
