#include "vertex.h"

Vertex::Vertex()
{
    idx = count;
    count += 1;
}

Vertex::Vertex(glm::vec3 v)
    :pos(v)
{
    idx = count;
    count += 1;
}

Vertex::Vertex(glm::vec3 v, HalfEdge* e)
    : pos(v), halfedge(e)
{
    idx = count;
    count += 1;
}
