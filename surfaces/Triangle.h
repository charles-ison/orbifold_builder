#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <set>
#include "Vertex.h"

struct Triangle {
    std::vector<int> vertexIndices;

    bool sharesEdge(Triangle* triangle) {
        int numSharedVertices = 0;
        for (int vertexIndex1: vertexIndices) {
            for (int vertexIndex2 : triangle->vertexIndices) {
                if (vertexIndex1 == vertexIndex2) {
                    numSharedVertices += 1;
                }
            }
        }

        return (numSharedVertices > 1);
    }

    std::string toString(){
        return std::to_string(vertexIndices[0]) + ", " + std::to_string(vertexIndices[1]) + ", " + std::to_string(vertexIndices[2]);
    }
};

#endif
