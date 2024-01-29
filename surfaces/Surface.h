#ifndef Surface_H
#define Surface_H

#include <QOpenGLFunctions>
#include "Vertex.h"

class Surface {
public:
    virtual GLushort* getIndices() = 0;
    virtual Vertex* getVertices() = 0;
    virtual int getNumVertices() = 0;
    virtual int getNumIndices() = 0;

protected:
    void initNeighbors();

private:
    virtual void initVertices() = 0;
    virtual void initIndices() = 0;
    virtual void initTriangles() = 0;
};

#endif
