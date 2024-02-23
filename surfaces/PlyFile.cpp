#include "PlyFile.h"
#include <fstream>
#include <iostream>

PlyFile::PlyFile(QString fileName, QVector3D centerPosition, QVector3D scale) {
    std::ifstream plyFile(fileName.toStdString(), std::ifstream::binary);
    std::string plyFileToken;
    int numProperties = -1;
    while(plyFileToken != "end_header") {
        plyFile >> plyFileToken;
        if (plyFileToken == "vertex") {
            plyFile >> plyFileToken;
            numVertices = stoi(plyFileToken);
        }
        else if (plyFileToken == "face") {
            plyFile >> plyFileToken;
            numTriangles = stoi(plyFileToken);
        }
        else if (plyFileToken == "property") {
            numProperties += 1;
        }
    }

    for (int i=0; i<numVertices; i+=1) {
        plyFile >> plyFileToken;
        float x = stof(plyFileToken);

        plyFile >> plyFileToken;
        float y = stof(plyFileToken);

        plyFile >> plyFileToken;
        float z = stof(plyFileToken);

        int numPropertiesRead = 3;
        while (numPropertiesRead < numProperties) {
            plyFile >> plyFileToken;
            numPropertiesRead += 1;
        }

        vertices.push_back({.index = i, .position = QVector3D(x,  y,  z)});
    }

    for (int i=0; i<numTriangles; i+=1) {
        // Reading in number of vertices, but not doing anything with it
        plyFile >> plyFileToken;

        plyFile >> plyFileToken;
        int index0 = stoi(plyFileToken);

        plyFile >> plyFileToken;
        int index1 = stoi(plyFileToken);

        plyFile >> plyFileToken;
        int index2 = stoi(plyFileToken);

        triangles.push_back({{index0,  index1,  index2}});
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
