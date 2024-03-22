#ifndef UTIL_H
#define UTIL_H

static double euclideanDistance(Vertex* vertex1, Vertex* vertex2) {
    float dx = vertex1->position.x()-vertex2->position.x();
    float dy = vertex1->position.y()-vertex2->position.y();
    float dz = vertex1->position.z()-vertex2->position.z();

    // Use for fast marching
    //float delta = 2*oldDistance - pow(dx-dy-dz, 2);
    //if (delta >= 0) {
    //    return (dx + dy + dz + sqrt(delta))/2;
    //} else {
    //    return oldDistance + sqrt(pow(dx, 2) + pow(dy, 2) + pow(dz, 2));
    //}

    return sqrt(pow(dx, 2) + pow(dy, 2) + pow(dz, 2));
}

static double cot(double angle) {
    return cos(angle) / sin(angle);
}

#endif