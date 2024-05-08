#ifndef GEOMETRYENGINE_H
#define GEOMETRYENGINE_H

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include "Mesh.h"

class GeometryEngine : protected QOpenGLFunctions {
public:
    GeometryEngine();
    virtual ~GeometryEngine();
    void drawMesh(QOpenGLShaderProgram *program);
    void drawLine(QOpenGLShaderProgram *program);
    void drawBoundary(QOpenGLShaderProgram *program);
    void drawPoints(QOpenGLShaderProgram *program);
    void drawGenerators(QOpenGLShaderProgram *program);
    void initLine(std::vector<Vertex*> lineVerticesVector, QColor color);
    void initBoundary(std::vector<Vertex*> boundaryVertices1, std::vector<Vertex*> boundaryVertices2, int displaySize1, int displaySize2, bool isBoundary1Loop, bool isBoundary2Loop, bool boundariesAreCombinedLoop, bool boundariesReversed, bool boundariesOverlapping, int numOpenings);
    void initMesh(Mesh* mesh);
    void initPoints(std::vector<Vertex*> vertices);
    void initGenerators(std::vector<Vertex*> lineVerticesVector, QColor color);

private:
    std::vector<QVector3D> initBoundaryColors(int numVertices, int displaySize);
    void drawArrows(QOpenGLShaderProgram *program);
    void initArrows(std::vector<QVector3D> positions1, std::vector<QVector3D> colors1, std::vector<QVector3D> positions2, std::vector<QVector3D> colors2, bool boundariesOverlapping, bool boundariesReversed, int numOpenings);
    std::vector<Vertex> buildArrow(std::vector<QVector3D> positions, int startIndex);

    int numIndices;
    QOpenGLBuffer arrayBuf;
    QOpenGLBuffer indexBuf;
    QOpenGLBuffer colorBuf;
    QOpenGLBuffer normalBuf;

    int numLineVertices;
    QOpenGLBuffer lineColorBuf;
    QOpenGLBuffer lineArrayBuf;
    QOpenGLBuffer lineNormalBuf;

    int numGeneratorVertices;
    QOpenGLBuffer generatorsColorBuf;
    QOpenGLBuffer generatorsArrayBuf;
    QOpenGLBuffer generatorsNormalBuf;

    int numPointVertices;
    QOpenGLBuffer pointColorBuf;
    QOpenGLBuffer pointArrayBuf;
    QOpenGLBuffer pointNormalBuf;

    int numBoundaryVertices1;
    QOpenGLBuffer boundaryColorBuf1;
    QOpenGLBuffer boundaryArrayBuf1;
    QOpenGLBuffer boundaryNormalBuf1;

    int numBoundaryVertices2;
    QOpenGLBuffer boundaryColorBuf2;
    QOpenGLBuffer boundaryArrayBuf2;
    QOpenGLBuffer boundaryNormalBuf2;

    int numArrowVertices;
    QOpenGLBuffer arrowColorBuf;
    QOpenGLBuffer arrowIndexBuf;
    QOpenGLBuffer arrowArrayBuf;
    QOpenGLBuffer arrowNormalBuf;
};

#endif
