#include "halfedge.h"

static int count = 0;

HalfEdge::HalfEdge()
{
    idx = count;
    count += 1;
    QListWidgetItem::setText(QString::number(idx));
}

HalfEdge::HalfEdge(Vertex* v)
    :vert(v)
{
    idx = count;
    count += 1;
    QListWidgetItem::setText(QString::number(idx));
}

HalfEdge::HalfEdge(HalfEdge* next, HalfEdge* sym, Face* f, Vertex* v)
    :nextEdge(next), symEdge(sym), face(f), vert(v)
{
    idx = count;
    count += 1;
    QListWidgetItem::setText(QString::number(idx));
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

HalfEdge* HalfEdge::getNext() const
{
    return this->nextEdge;
}

HalfEdge* HalfEdge::getSym() const
{
    return this->symEdge;
}

Face* HalfEdge::getFace() const
{
    return this->face;
}

Vertex* HalfEdge::getVert() const
{
    return this->vert;
}
