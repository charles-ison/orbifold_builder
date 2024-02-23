#include "PlyFile.h"
#include <fstream>
#include <iostream>

PlyFile::PlyFile(QString fileName, QVector3D centerPosition, QVector3D scale) {
    std::ifstream plyFile(fileName.toStdString(), std::ifstream::binary);
    std::string plyFileToken;
    while(plyFileToken != "end_header") {
        plyFile >> plyFileToken;
        if (plyFileToken == "vertex") {
            plyFile >> plyFileToken;
            numVertices = stoi(plyFileToken);
        }
        if (plyFileToken == "face") {
            plyFile >> plyFileToken;
            numTriangles = stoi(plyFileToken);
        }
    }

    int vertexCounter = 0;
    for (int i=0; i<4*numVertices; i+=4) {
        plyFile >> plyFileToken;
        int x = stoi(plyFileToken);

        plyFile >> plyFileToken;
        int y = stoi(plyFileToken);

        plyFile >> plyFileToken;
        int z = stoi(plyFileToken);

        // Reading in angle deficit, but not doing anything with it
        plyFile >> plyFileToken;

        std::cout << "x: " << x << std::endl;
        std::cout << "y: " << y << std::endl;
        std::cout << "z: " << z << std::endl;

        vertices.push_back({.index = vertexCounter, .position = QVector3D(x,  y,  z)});
        vertexCounter += 1;
    }

    for (int i=0; i<5*numTriangles; i+=5) {
        // Reading in number of vertices, but not doing anything with it
        plyFile >> plyFileToken;

        plyFile >> plyFileToken;
        int index0 = stoi(plyFileToken);

        plyFile >> plyFileToken;
        int index1 = stoi(plyFileToken);

        plyFile >> plyFileToken;
        int index2 = stoi(plyFileToken);

        plyFile >> plyFileToken;
        int index3 = stoi(plyFileToken);

        triangles.push_back({{index0,  index1,  index2}});
        triangles.push_back({{index1,  index2,  index3}});
    }
    plyFile.close();
    initNeighbors();
}

void PlyFile::initVertices(QVector3D centerPosition, QVector3D scale) {

}

void PlyFile::initTriangles() {

}

Triangle* PlyFile::getTriangles() {
    return &triangles[0];
}

Vertex* PlyFile::getVertices() {
    return &vertices[0];
}

int PlyFile::getNumVertices() {
    return numVertices;
}

int PlyFile::getNumTriangles() {
    return numTriangles;
}
