#ifndef TRIANGLE_H
#define TRIANGLE_H

class Vertex;

struct Triangle {
    std::vector<Vertex*> vertices;
    std::vector<double> angles;
    QVector3D normal;

    bool sharesEdge(Triangle* triangle) {
        int numSharedVertices = 0;
        for (Vertex* vertex1 : vertices) {
            for (Vertex* vertex2 : triangle->vertices) {
                if (vertex1 == vertex2) {
                    numSharedVertices += 1;
                }
            }
        }

        return (numSharedVertices > 1);
    }
};

#endif
