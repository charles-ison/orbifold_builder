#ifndef Surface_H
#define Surface_H

#include <QOpenGLFunctions>
#include "Vertex.h"
#include "Triangle.h"

class Surface {
public:
    virtual Vertex* getVertices() = 0;
    virtual Triangle* getTriangles() = 0;
    virtual int getNumVertices() = 0;
    virtual int getNumTriangles() = 0;

private:
    virtual void initVertices(QVector3D centerPosition, QVector3D scale) = 0;
    virtual void initTriangles() = 0;
};

#endif
