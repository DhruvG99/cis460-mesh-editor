#include "vertex.h"
static int count = 0;

Vertex::Vertex()
    : QListWidgetItem()
{
    idx = count;
    count += 1;
    QListWidgetItem::setText(QString::number(idx));
}

Vertex::Vertex(glm::vec4 v)
    :pos(v)
{
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
