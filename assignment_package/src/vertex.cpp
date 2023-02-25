#include "vertex.h"
static int count = 0;

Vertex::Vertex()
{
    idx = count;
    count += 1;
}

Vertex::Vertex(glm::vec4 v)
    :pos(v)
{
    idx = count;
    count += 1;
}

Vertex::Vertex(glm::vec4 v, HalfEdge* e)
    : pos(v), halfedge(e)
{
    idx = count;
    count += 1;
}
