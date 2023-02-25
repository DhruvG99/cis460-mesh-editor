#ifndef VERTEX_H
#define VERTEX_H
#include <la.h>
#include <drawable.h>

class HalfEdge;
class Face;

class Vertex
//     : public Drawable
{
public:
    glm::vec4 pos;
    HalfEdge* halfedge;
    int idx;

    Vertex();
    Vertex(glm::vec4);
    Vertex(glm::vec4, HalfEdge*);
};





#endif // VERTEX_H
