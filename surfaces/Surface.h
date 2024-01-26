#ifndef Surface_H
#define Surface_H

#include <QOpenGLFunctions>
#include "VertexData.h"

class Surface {
public:
    virtual GLushort* getIndices() = 0;
    virtual VertexData* getVertices() = 0;
    virtual int getNumVertices() = 0;
    virtual int getNumIndices() = 0;
    void removeVertex(VertexData *vertex);

protected:
    void initNeighbors();

private:
    virtual void initVertices() = 0;
    virtual void initIndices() = 0;
};

#endif
