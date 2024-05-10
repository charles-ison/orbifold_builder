#include "PlyFile.h"
#include <fstream>

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

    float scalingFactor = 1.0;
    std::string fileNameString = fileName.toStdString();
    std::string kleinBottleFileName = "klein_bottle.ply";
    if (fileNameString.substr(fileNameString.length() - kleinBottleFileName.length(), fileNameString.length()) == kleinBottleFileName) {
        scalingFactor = 0.1;
    }

    for (int i=0; i<numVertices; i+=1) {
        plyFile >> plyFileToken;
        float x = scalingFactor * stof(plyFileToken);

        plyFile >> plyFileToken;
        float y = scalingFactor * stof(plyFileToken);

        plyFile >> plyFileToken;
        float z = scalingFactor * stof(plyFileToken);

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

        triangles.push_back({{&vertices[index2],  &vertices[index1],  &vertices[index0]}});
    }
    plyFile.close();
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
