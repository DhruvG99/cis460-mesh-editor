#include "vertex.h"
#include <halfedge.h>

static int count = 0;

Vertex::Vertex()
    : QListWidgetItem(), pos(), halfedge(nullptr)
{
    idx = count;
    count += 1;
    QListWidgetItem::setText(QString::number(idx));
}

Vertex::Vertex(glm::vec4 v, int reset)
    :pos(v), halfedge(nullptr)
{
    if(!reset)
        count = 0;
    idx = count;
    count += 1;
    QListWidgetItem::setText(QString::number(idx));
}

Vertex::Vertex(glm::vec4 v, HalfEdge* e)
    : pos(v), halfedge(e)
{
    idx = count;
    count += 1;
    QListWidgetItem::setText(QString::number(idx));
}

void Vertex::setPos(glm::vec4 p)
{
    this->pos = p;
}

