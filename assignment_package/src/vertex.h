#ifndef VERTEX_H
#define VERTEX_H
#include <la.h>
#include <drawable.h>
#include <joint.h>
#include <QListWidgetItem>

class HalfEdge;
class Face;

class Vertex
     : public QListWidgetItem
{
public:
    glm::vec4 pos;
    HalfEdge* halfedge;
    int idx;

    Joint *influencer1, *influencer2;
    float weight1, weight2;

    Vertex();
    Vertex(glm::vec4, int reset = 1);
    Vertex(glm::vec4, HalfEdge*);
    void setPos(glm::vec4);
};





#endif // VERTEX_H
