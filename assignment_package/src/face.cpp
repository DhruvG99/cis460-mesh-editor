#include "face.h"

Face::Face()
{
    idx = count;
    count += 1;
}

Face::Face(HalfEdge* edge, glm::vec3 v)
    :halfedge(edge), color(v)
{
    idx = count;
    count += 1;
}

void Face::setEdge(HalfEdge* e)
{
    this->halfedge = e;
}

HalfEdge* Face::getEdge()
{
    return this->halfedge;
}
