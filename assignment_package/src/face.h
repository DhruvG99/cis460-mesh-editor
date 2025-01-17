#ifndef FACE_H
#define FACE_H
#include<la.h>
#include<drawable.h>
#include<qlistwidget.h>

class Vertex;
class HalfEdge;

class Face
        : public QListWidgetItem
{
private:
    HalfEdge* halfedge;
    glm::vec4 color;
    int idx;
public:
    Face(int reset = 1);
    Face(HalfEdge*, glm::vec4);
    void setEdge(HalfEdge*);
    void setColor(glm::vec4);
    glm::vec4 getColor() const;
    HalfEdge* getEdge() const;
};

#endif // FACE_H
