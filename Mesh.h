#ifndef MESH_H
#define MESH_H

#include <unordered_set>
#include "surfaces/Vertex.h"
#include "surfaces/Surface.h"
#include "Util.h"
#include "SparseMat.h"

class Mesh {

public:
    void resetSurface(Surface *surface);
    void addSurface(Surface *surface);
    std::vector<Vertex*> getVertices();
    std::vector<Triangle*> getTriangles();
    void addVertex(Vertex *vertexToAdd);
    void deleteVerticesReferences(std::unordered_set<Vertex*> verticesToDelete);
    void deleteVertices(std::unordered_set<Vertex*> verticesToDelete);
    void deleteOutdatedTriangles();
    void updateTriangles();
    void directLaplaceEquationSolving(std::vector<Vertex*> verticesToSmooth, std::map<Vertex*, int> verticesToSmoothMap, std::vector<Vertex*> drawnVertices);
    void implicitSmooth(std::vector<Vertex*> verticesToSmooth, std::map<Vertex*, int> verticesToSmoothMap);
    void explicitSmooth(std::vector<Vertex*> verticesToSmooth, std::map<Vertex*, int> verticesToSmoothMap);
    bool triangleContainsVertex(Vertex *vertex, Triangle *triangle);
    std::tuple<float, std::vector<Vertex*>> getVerticesPathAndDistance(Vertex *startVertex, Vertex *endVertex);

private:
    void deleteTriangleReferences(Triangle* triangle);
    std::vector<double> computeAngles(Vertex* vertex0, Vertex* vertex1, Vertex* vertex2);
    void updateVertexNeighbors();
    double getMeanValueWeights(Vertex* vertex1, Vertex* vertex2);
    double getMeanCurvatureWeights(Vertex* vertex1, Vertex* vertex2);
    std::vector<double> biconjugateGradientMethod(SparseMat* matrixA, std::vector<double> b, double tolerance, int maxIters);
    std::vector<double> solveEquation(SparseMat* matrixA, std::vector<double> b);
    double computeNorm(std::vector<double> b);

    std::vector<Vertex*> vertices;
    std::vector<Triangle*> triangles;
};

#endif
