#ifndef VERTEX_H
#define VERTEX_H
#include <la.h>
#include <drawable.h>
#include <vertex.h>

class HalfEdge;
class Face;

class Vertex
//     : public Drawable
{
public:
    glm::vec3 pos;
    HalfEdge* halfedge;
    int idx;
    static int count;

    Vertex();
    Vertex(glm::vec3);
    Vertex(glm::vec3, HalfEdge*);
};





#endif // VERTEX_H
