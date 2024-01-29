#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <set>
#include "Vertex.h"

struct Triangle {
    std::set<Vertex*> neighbors;
};

#endif
