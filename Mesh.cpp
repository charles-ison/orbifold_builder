#include "Mesh.h"
#include <queue>
#include <iostream>

void Mesh::resetSurface(Surface *surface) {
    vertices.clear();
    triangles.clear();
    addSurface(surface);
}

void Mesh::addSurface(Surface *surface) {
    int numSurfaceVertices = surface->getNumVertices();
    Vertex* surfaceVertices = surface->getVertices();
    for (int i=0; i<numSurfaceVertices; i++) {
        vertices.push_back(&surfaceVertices[i]);
    }

    int numSurfaceTriangles = surface->getNumTriangles();
    Triangle* surfaceTriangles = surface->getTriangles();
    for (int i=0; i<numSurfaceTriangles; i++) {
        triangles.push_back(&surfaceTriangles[i]);
    }

    updateVertexNeighbors();
    updateTriangles();
}

void Mesh::updateVertexNeighbors() {
    for (int i=0; i<triangles.size(); i++) {
        Vertex* vertex0 = triangles[i]->vertices[0];
        Vertex* vertex1 = triangles[i]->vertices[1];
        Vertex* vertex2 = triangles[i]->vertices[2];

        vertex0->triangles.insert(triangles[i]);
        vertex1->triangles.insert(triangles[i]);
        vertex2->triangles.insert(triangles[i]);
    }
}

std::vector<double> Mesh::computeAngles(Vertex* vertex0, Vertex* vertex1, Vertex* vertex2) {
    double edge01 = euclideanDistance(vertex0, vertex1);
    double edge12 = euclideanDistance(vertex1, vertex2);
    double edge20 = euclideanDistance(vertex2, vertex0);
    double angle0 = acos((pow(edge01, 2) + pow(edge20, 2) - pow(edge12, 2)) / (2 * edge01 * edge20));
    double angle1 = acos((pow(edge01, 2) + pow(edge12, 2) - pow(edge20, 2)) / (2 * edge01 * edge12));
    double angle2 = acos((pow(edge12, 2) + pow(edge20, 2) - pow(edge01, 2)) / (2 * edge12 * edge20));
    return {angle0, angle1, angle2};
}

void Mesh::updateTriangles() {
    for (int i=0; i<triangles.size(); i++) {
        Vertex* vertex0 = triangles[i]->vertices[0];
        Vertex* vertex1 = triangles[i]->vertices[1];
        Vertex* vertex2 = triangles[i]->vertices[2];

        QVector3D vector1 = vertex1->position - vertex0->position;
        QVector3D vector2 = vertex2->position - vertex1->position;
        QVector3D faceNormal = QVector3D::crossProduct(vector1, vector2).normalized();
        triangles[i]->normal = faceNormal;
        triangles[i]->angles = computeAngles(vertex0, vertex1, vertex2);
    }

    for (int i=0; i<vertices.size(); i++) {
        for (Triangle* triangle : vertices[i]->triangles) {
            vertices[i]->normal += triangle->normal;
        }
        vertices[i]->normal = vertices[i]->normal.normalized();
    }
}

std::vector<Vertex*> Mesh::getVertices() {
    return vertices;
}

std::vector<Triangle*> Mesh::getTriangles() {
    return triangles;
}

void Mesh::addVertex(Vertex *vertexToAdd) {
    vertices.push_back(vertexToAdd);
}

void Mesh::deleteTriangleReferences(Triangle* triangleToDelete) {
    for (Vertex* vertex : triangleToDelete->vertices) {
        std::set<Triangle *> newTriangles;
        for (Triangle *vertexTriangle: vertex->triangles) {
            if (vertexTriangle != triangleToDelete) {
                newTriangles.insert(vertexTriangle);
            }
        }
        vertex->triangles = newTriangles;
    }
}

void Mesh::deleteVerticesReferences(std::unordered_set<Vertex*> verticesToDelete) {
    std::vector<Triangle*> newTriangles;
    for (Triangle* triangle : triangles) {
        Vertex* vertex1 = triangle->vertices[0];
        Vertex* vertex2 = triangle->vertices[1];
        Vertex* vertex3 = triangle->vertices[2];

        if (verticesToDelete.find(vertex1) != verticesToDelete.end() || verticesToDelete.find(vertex2) != verticesToDelete.end() || verticesToDelete.find(vertex3) != verticesToDelete.end()) {
            deleteTriangleReferences(triangle);
        } else {
            newTriangles.push_back(triangle);
        }
    }
    triangles = newTriangles;
}

void Mesh::deleteVertices(std::unordered_set<Vertex*> verticesToDelete) {
    int delta = 0;
    auto itr = vertices.begin();
    while(itr != vertices.end()) {
        Vertex* vertex = *itr;
        if(verticesToDelete.find(vertex) != verticesToDelete.end()) {
            itr = vertices.erase(itr);
            delta += 1;
        } else {
            vertex->index -= delta;
            itr ++;
        }
    }
}

void Mesh::deleteOutdatedTriangles() {
    std::vector<Triangle*> newTriangles;
    for (Triangle* triangle : triangles) {
        Vertex* vertex1 = triangle->vertices[0];
        Vertex* vertex2 = triangle->vertices[1];
        Vertex* vertex3 = triangle->vertices[2];
        std::set<Vertex*> triangleVertices = {vertex1, vertex2, vertex3};

        if (triangleVertices.size() != 3) {
            deleteTriangleReferences(triangle);
        } else {
            newTriangles.push_back(triangle);
        }
    }
    triangles = newTriangles;
}

double Mesh::getMeanValueWeights(Vertex* vertex1, Vertex* vertex2) {
    double weights = 0.0;
    int numAnglesFound = 0;
    for (Triangle* triangle : vertex1->triangles) {
        if (triangleContainsVertex(vertex2, triangle)) {
            for (int i=0; i<3; i++) {
                if (triangle->vertices[i] == vertex1) {
                    weights += tan(triangle->angles[i]/2.0)/2.0;
                    numAnglesFound += 1;
                }
            }
        }
    }

    if (numAnglesFound != 2) {
        std::cout << "Error: 2 angles not found for mean value weights. Surface not closed." << std::endl;
    }
    return weights;
}

double Mesh::getMeanCurvatureWeights(Vertex* vertex1, Vertex* vertex2) {
    double weights = 0.0;
    int numAnglesFound = 0;
    for (Triangle* triangle : vertex1->triangles) {
        if (triangleContainsVertex(vertex2, triangle)) {
            for (int i=0; i<3; i++) {
                if (triangle->vertices[i] != vertex1 && triangle->vertices[i] != vertex2) {
                    weights += cot(triangle->angles[i])/2.0;
                    numAnglesFound += 1;
                }
            }
        }
    }
    if (numAnglesFound != 2) {
        std::cout << "Error: 2 angles not found for mean curvature weights. Surface not closed." << std::endl;
    }
    return weights;
}

void Mesh::directLaplaceEquationSolving(std::vector<Vertex*> verticesToSmooth, std::map<Vertex*, int> verticesToSmoothMap, std::vector<Vertex*> drawnVertices) {
    std::vector<double> bX, bY, bZ;
    int numVertices = verticesToSmooth.size();
    for (int i=0; i<numVertices; i++) {
        bX.push_back(0.0);
        bY.push_back(0.0);
        bZ.push_back(0.0);
    }

    std::unordered_set<Vertex*> drawnVerticesSet;
    for(Vertex* drawnVertex : drawnVertices) {
        if (drawnVerticesSet.find(drawnVertex) == drawnVerticesSet.end()) {
            drawnVerticesSet.insert(drawnVertex);
            verticesToSmoothMap.insert({drawnVertex, verticesToSmoothMap.size()});
        }
    }

    SparseMat* matrixA = new SparseMat(numVertices, numVertices, 0);
    for (int i=0; i<numVertices; i++) {
        Vertex* vertex = verticesToSmooth[i];
        double neighborWeightSum = 0.0;
        std::unordered_set<Vertex *> visitedNeighbors;
        std::vector<Vertex *> neighbors;
        for (Triangle *triangle: vertex->triangles) {
            for (Vertex *neighbor: triangle->vertices) {
                bool impactsSystemOfEquations = verticesToSmoothMap.find(neighbor) != verticesToSmoothMap.end() || drawnVerticesSet.find(neighbor) != drawnVerticesSet.end();
                if (neighbor != vertex && visitedNeighbors.find(neighbor) == visitedNeighbors.end() && impactsSystemOfEquations) {
                    // Uniform weights
                    neighborWeightSum += 1;

                    // Cord Weights
                    // neighborWeightSum += euclideanDistance(vertex, neighbor);

                    // Mean Curvature Weights
                    // neighborWeightSum += getMeanCurvatureWeights(vertex, neighbor);

                    // Mean Value Weights
                    // neighborWeightSum += getMeanValueWeights(vertex, neighbor);

                    visitedNeighbors.insert(neighbor);
                    neighbors.push_back(neighbor);
                }
            }
        }

        neighbors.push_back(vertex);
        std::sort(neighbors.begin(), neighbors.end(), [&](Vertex* v1, Vertex* v2) {return verticesToSmoothMap.at(v1) < verticesToSmoothMap.at(v2); });
        int vertexIndex = verticesToSmoothMap.at(vertex);
        for (Vertex *neighbor : neighbors) {
            bool valFound = false;
            if (drawnVerticesSet.find(neighbor) != drawnVerticesSet.end()) {
                bX[i] -= neighbor->position.x();
                bY[i] -= neighbor->position.y();
                bZ[i] -= neighbor->position.z();
            } else if (vertex == neighbor) {
                matrixA->vals.push_back(-neighborWeightSum);
                valFound = true;
            } else {
                // Uniform weights
                double weight = 1;

                // Cord weights
                // double weight = euclideanDistance(vertex, neighbor);

                // Mean Curvature weights
                // double weight = getMeanCurvatureWeights(vertex, neighbor);

                // Mean Value weights
                // double weight = getMeanValueWeights(vertex, neighbor);

                valFound = true;
                matrixA->vals.push_back(weight);
            }

            if (valFound) {
                int neighborIndex = verticesToSmoothMap.at(neighbor);
                matrixA->rowIndices.push_back(neighborIndex);
                if (matrixA->vals.size() - 1 <= matrixA->colFirstIndices[vertexIndex]) {
                    matrixA->colFirstIndices[vertexIndex] = matrixA->vals.size() - 1;
                }
            }
        }
    }

    matrixA->colFirstIndices[matrixA->colFirstIndices.size()-1] = matrixA->vals.size();
    std::vector<double> newXPositions = biconjugateGradientMethod(matrixA, bX, 0.0001, 10000);
    std::vector<double> newYPositions = biconjugateGradientMethod(matrixA, bY, 0.0001, 10000);
    std::vector<double> newZPositions = biconjugateGradientMethod(matrixA, bZ, 0.0001, 10000);

    for (int i=0; i<verticesToSmooth.size(); i++) {
        verticesToSmooth[i]->position.setX(newXPositions[i]);
        verticesToSmooth[i]->position.setY(newYPositions[i]);
        verticesToSmooth[i]->position.setZ(newZPositions[i]);
    }

    updateTriangles();
}

void Mesh::implicitSmooth(std::vector<Vertex*> verticesToSmooth, std::map<Vertex*, int> verticesToSmoothMap) {
    std::vector<double> bX, bY, bZ;
    for (Vertex* vertex: verticesToSmooth) {
        bX.push_back(vertex->position.x());
        bY.push_back(vertex->position.y());
        bZ.push_back(vertex->position.z());
    }

    double stepSize = 2.0;
    int numVertices = verticesToSmooth.size();
    SparseMat* matrixA = new SparseMat(numVertices, numVertices, 0);
    for (Vertex* vertex: verticesToSmooth) {
        double neighborWeightSum = 0.0;
        std::unordered_set<Vertex *> visitedNeighbors;
        std::vector<Vertex *> neighbors;
        for (Triangle *triangle: vertex->triangles) {
            for (Vertex *neighbor: triangle->vertices) {
                if (neighbor != vertex && verticesToSmoothMap.find(neighbor) != verticesToSmoothMap.end() && visitedNeighbors.find(neighbor) == visitedNeighbors.end()) {
                    // Uniform weights
                    //neighborWeightSum += 1;

                    // Cord Weights
                    // neighborWeightSum += euclideanDistance(vertex, neighbor);

                    // Mean Curvature Weights
                    neighborWeightSum += getMeanCurvatureWeights(vertex, neighbor);

                    // Mean Value Weights
                    // neighborWeightSum += getMeanValueWeights(vertex, neighbor);

                    visitedNeighbors.insert(neighbor);
                    neighbors.push_back(neighbor);
                }
            }
        }

        neighbors.push_back(vertex);
        std::sort(neighbors.begin(), neighbors.end(), [&](Vertex* v1, Vertex* v2) {return verticesToSmoothMap.at(v1) < verticesToSmoothMap.at(v2); });

        int vertexIndex = verticesToSmoothMap.at(vertex);
        for (Vertex *neighbor: neighbors) {
            if (vertex == neighbor) {
                matrixA->vals.push_back(1.0 + stepSize * neighborWeightSum);
            } else {
                // Uniform weights
                // double weight = 1;

                // Cord weights
                // double weight = euclideanDistance(vertex, neighbor);

                // Mean Curvature weights
                double weight = getMeanCurvatureWeights(vertex, neighbor);

                // Mean Value weights
                // double weight = getMeanValueWeights(vertex, neighbor);

                matrixA->vals.push_back(-stepSize * weight);
            }
            int neighborIndex = verticesToSmoothMap.at(neighbor);
            matrixA->rowIndices.push_back(neighborIndex);
            if (matrixA->vals.size() - 1 <= matrixA->colFirstIndices[vertexIndex]) {
                matrixA->colFirstIndices[vertexIndex] = matrixA->vals.size() - 1;
            }
        }
    }

    matrixA->colFirstIndices[matrixA->colFirstIndices.size()-1] = matrixA->vals.size();
    std::vector<double> newXPositions = biconjugateGradientMethod(matrixA, bX, 0.001, 100);
    std::vector<double> newYPositions = biconjugateGradientMethod(matrixA, bY, 0.001, 100);
    std::vector<double> newZPositions = biconjugateGradientMethod(matrixA, bZ, 0.001, 100);

    for (int i=0; i<verticesToSmooth.size(); i++) {
        verticesToSmooth[i]->position.setX(newXPositions[i]);
        verticesToSmooth[i]->position.setY(newYPositions[i]);
        verticesToSmooth[i]->position.setZ(newZPositions[i]);
    }

    updateTriangles();
}

void Mesh::explicitSmooth(std::vector<Vertex*> verticesToSmooth, std::map<Vertex*, int> verticesToSmoothMap) {
    float stepSize = 0.5;
    std::vector<QVector3D> newPositions;

    for (Vertex *vertex: verticesToSmooth) {
        double neighborWeightSum = 0.0;
        double xDiff = 0.0;
        double yDiff = 0.0;
        double zDiff = 0.0;
        std::vector<Vertex *> neighbors;
        std::unordered_set<Vertex *> visitedNeighbors;

        // Initializing weights
        for (Triangle *triangle : vertex->triangles) {
            for (Vertex *neighbor : triangle->vertices) {
                if (neighbor != vertex && verticesToSmoothMap.find(neighbor) != verticesToSmoothMap.end() && visitedNeighbors.find(neighbor) == visitedNeighbors.end()) {
                    // Uniform weights
                    // neighborWeightSum += 1;

                    // Cord Weights
                    // neighborWeightSum += 1.0 / euclideanDistance(vertex, neighbor);

                    // Mean Curvature Weights
                    // neighborWeightSum += getMeanCurvatureWeights(vertex, neighbor);

                    // Mean Value Weights
                    neighborWeightSum += getMeanValueWeights(vertex, neighbor);

                    visitedNeighbors.insert(neighbor);
                    neighbors.push_back(neighbor);
                }
            }
        }

        for (Vertex *neighbor : neighbors) {
            // Uniform weights
            // double weight = 1;

            // Cord weights
            // double weight = 1.0 / euclideanDistance(vertex, neighbor);

            // Mean Curvature weights
            // double weight = getMeanCurvatureWeights(vertex, neighbor);

            // Mean Value weights
            double weight = getMeanValueWeights(vertex, neighbor);

            xDiff += (weight/neighborWeightSum) * (neighbor->position.x() - vertex->position.x());
            yDiff += (weight/neighborWeightSum) * (neighbor->position.y() - vertex->position.y());
            zDiff += (weight/neighborWeightSum) * (neighbor->position.z() - vertex->position.z());
        }

        float newX = vertex->position.x() + stepSize * xDiff;
        float newY = vertex->position.y() + stepSize * yDiff;
        float newZ = vertex->position.z() + stepSize * zDiff;
        newPositions.push_back({newX, newY, newZ});
    }

    for (int j = 0; j < verticesToSmooth.size(); j++) {
        verticesToSmooth[j]->position = newPositions[j];
    }

    updateTriangles();
}

bool Mesh::triangleContainsVertex(Vertex *vertex, Triangle *triangle) {
    Vertex *vertex1 = triangle->vertices[0];
    Vertex *vertex2 = triangle->vertices[1];
    Vertex *vertex3 = triangle->vertices[2];

    if (vertex1==vertex || vertex2==vertex || vertex3==vertex) {
        return true;
    } else {
        return false;
    }
}

std::vector<double> Mesh::biconjugateGradientMethod(SparseMat* matrixA, std::vector<double> b, double tolerance, int maxIters) {
    int size = b.size();
    int iter = 0;
    double ak = 0.0;
    double akDen = 1.0;
    double bk = 0.0;
    double bkDen = 1.0;
    double bkNum = 0.0;
    std::vector<double> p(size);
    std::vector<double> pp(size);
    std::vector<double> zz(size);
    std::vector<double> rr(size);
    std::vector<double> x = b;
    std::vector<double> r = matrixA->multiply(x);

    for (int i=0; i<size; i++) {
        r[i] = b[i] - r[i];
        rr[i] = r[i];
    }

    double bNorm = computeNorm(b);
    std::vector<double> z = solveEquation(matrixA, r);
    double error = computeNorm(r)/bNorm;

    while (iter < maxIters && error >= tolerance) {
        iter++;

        // Technically matrixA should be transposed here
        zz = solveEquation(matrixA, rr);

        bkNum = 0.0;
        for (int i=0; i<size; i++) {
            bkNum += z[i]*rr[i];
        }

        if (iter == 1) {
            for (int i=0; i<size; i++) {
                p[i] = z[i];
                pp[i] = z[i];
            }
        } else {
            bk = bkNum/bkDen;
            for (int i=0; i<size; i++) {
                p[i] = bk * p[i] + z[i];
                pp[i] = bk * pp[i] + zz[i];
            }
        }

        bkDen = bkNum;
        z = matrixA->multiply(p);

        akDen = 0.0;
        for (int i = 0; i<size; i++) {
            akDen += z[i]*pp[i];
        }

        ak = bkNum / akDen;
        zz = matrixA->multiplyInverse(pp);

        for (int i=0; i<size; i++) {
            x[i] += ak*p[i];
            r[i] -= ak*z[i];
            rr[i] -= ak*zz[i];
        }
        z = solveEquation(matrixA, r);
        error = computeNorm(r)/bNorm;
    }
    return x;
}

std::vector<double> Mesh::solveEquation(SparseMat* matrixA, std::vector<double> b) {
    double diag;
    std::vector<double> x;
    for (int i=0; i<b.size(); i++) {
        diag = 0.0;
        for (int j=matrixA->colFirstIndices[i]; j<matrixA->colFirstIndices[i+1]; j++) {
            if (matrixA->rowIndices[j] == i) {
                diag = matrixA->vals[j];
                break;
            }
        }
        if (diag != 0.0) {
            x.push_back(b[i]/diag);
        } else {
            x.push_back(b[i]);
        }
    }
    return x;
}

double Mesh::computeNorm(std::vector<double> b) {
    double sum = 0.0;
    for (int i=0; i<b.size(); i++) {
        sum += pow(b[i], 2);
    }
    return sqrt(sum);
}

std::vector<Vertex*> Mesh::getConnectingPath(Vertex *startVertex, Vertex *endVertex) {
    std::vector<Vertex*> path = {startVertex};
    std::unordered_set<Vertex*> checkedVertices;
    std::priority_queue<std::tuple<double, std::vector<Vertex*>>, std::vector<std::tuple<double, std::vector<Vertex*>>>, std::greater<std::tuple<float, std::vector<Vertex*>>> > potentialPaths;
    potentialPaths.push({0.0, path});
    QVector3D endVertexPosition = endVertex->position;

    while(!potentialPaths.empty()) {
        std::tuple<double, std::vector<Vertex*>> nextDistanceAndPath = potentialPaths.top();
        double oldDistance = std::get<0>(nextDistanceAndPath);
        std::vector<Vertex*> nextPath = std::get<1>(nextDistanceAndPath);
        Vertex *nextVertex = nextPath.back();
        potentialPaths.pop();

        for (Triangle* triangle : nextVertex->triangles) {
            for (Vertex *neighbor : triangle->vertices) {
                if (neighbor->position == endVertexPosition) {
                    return nextPath;
                } else if (checkedVertices.find(neighbor) == checkedVertices.end()) {
                    std::vector<Vertex*> newPotentialPath = nextPath;
                    newPotentialPath.push_back(neighbor);
                    double distance = oldDistance + euclideanDistance(neighbor, nextVertex);
                    potentialPaths.push({distance, newPotentialPath});
                    checkedVertices.insert(neighbor);
                }
            }
        }
    }
    return path;
}
