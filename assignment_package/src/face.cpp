#include "face.h"

static int count = 0;

Face::Face()
{
    idx = count;
    count += 1;
    QListWidgetItem::setText(QString::number(idx));
}

Face::Face(HalfEdge* edge, glm::vec3 v)
    :halfedge(edge), color(v)
{
    idx = count;
    count += 1;
    QListWidgetItem::setText(QString::number(idx));
}

void Face::setEdge(HalfEdge* e)
{
    this->halfedge = e;
}

HalfEdge* Face::getEdge() const
{
    return this->halfedge;
}
