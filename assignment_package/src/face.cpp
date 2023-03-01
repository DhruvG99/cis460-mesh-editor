#include "face.h"
#include <halfedge.h>

static int count = 0;

Face::Face()
    :halfedge(nullptr), color()
{
    idx = count;
    count += 1;
    QListWidgetItem::setText(QString::number(idx));
}

Face::Face(glm::vec4 v, int reset)
    :halfedge(nullptr), color(v)
{
    if(!reset)
        count = 0;
    idx = count;
    count += 1;
    QListWidgetItem::setText(QString::number(idx));
}

Face::Face(HalfEdge* edge, glm::vec4 v)
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

glm::vec4 Face::getColor() const
{
    return this->color;
}

HalfEdge* Face::getEdge() const
{
    return this->halfedge;
}
