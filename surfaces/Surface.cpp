#include "Surface.h"
#include <iostream>

void Surface::initNeighbors() {
    VertexData* vertices = getVertices();
    GLushort* indices = getIndices();
    int numIndices = getNumIndices();

    for (int i=0; i<numIndices; i+=3) {
        GLushort index1 = indices[i];
        GLushort index2 = indices[i+1];
        GLushort index3 = indices[i+2];

        vertices[index1].neighbors.push_back(&vertices[index2]);
        vertices[index1].neighbors.push_back(&vertices[index3]);

        vertices[index2].neighbors.push_back(&vertices[index1]);
        vertices[index2].neighbors.push_back(&vertices[index3]);

        vertices[index3].neighbors.push_back(&vertices[index1]);
        vertices[index3].neighbors.push_back(&vertices[index2]);
    }
}

void Surface::removeVertex(VertexData *vertex) {
    //std::cout << "lineVertices.size()1: " << lineVertices.size() << std::endl;
    //for(auto itr = lineVertices.begin(); itr != lineVertices.end(); itr++) {
    //    std::cout << "*itr: " << *itr << std::endl;
    //    std::cout << "itr->position.x(): " << (*itr)->position.x() << std::endl;
    //    std::cout << "itr->position.y(): " << (*itr)->position.y() << std::endl;
   //     std::cout << "itr->position.z(): " << (*itr)->position.z() << std::endl;
   //     if (*itr == vertexToCut) {
    //        std::cout << "hi3" << std::endl;
    //        lineVertices.erase(itr);
    //    }
   // }
    //std::cout << "lineVertices.size()2: " << lineVertices.size() << std::endl;
}
