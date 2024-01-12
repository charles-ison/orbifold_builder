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
};

#endif
