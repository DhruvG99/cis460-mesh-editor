#include "halfedge.h"

static int count = 0;

HalfEdge::HalfEdge()
{
    idx = count;
    count += 1;
}

HalfEdge::HalfEdge(Vertex* v)
    :vert(v)
{
    idx = count;
    count += 1;
}

HalfEdge::HalfEdge(HalfEdge* next, HalfEdge* sym, Face* f, Vertex* v)
    :nextEdge(next), symEdge(sym), face(f), vert(v)
{
    idx = count;
    count += 1;
}

void HalfEdge::setVert(Vertex* v)
{
    this->vert = v;
    v->halfedge = this;
}

void HalfEdge::setFace(Face* f)
{
    this->face = f;
}

void HalfEdge::setNext(HalfEdge* e)
{
    this->nextEdge = e;
}

void HalfEdge::setSym(HalfEdge* e)
{
    this->symEdge = e;
    e->symEdge = this;
}

HalfEdge* HalfEdge::getNext()
{
    return this->nextEdge;
}

Vertex* HalfEdge::getVert()
{
    return this->vert;
}
