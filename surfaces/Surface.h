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

protected:
    void initNeighbors();

private:
    virtual void initVertices() = 0;
    virtual void initTriangles() = 0;
};

#endif
