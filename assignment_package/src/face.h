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
    glm::vec3 color;
    int idx;
public:
    Face();
    Face(HalfEdge*, glm::vec3);
    void setEdge(HalfEdge* e);
    HalfEdge* getEdge() const;
};

#endif // FACE_H
