#ifndef FACE_H
#define FACE_H
#include<la.h>
#include<drawable.h>

class Vertex;
class HalfEdge;

class Face
//        : public Drawable
{
private:
    HalfEdge* halfedge;
    glm::vec3 color;
    int idx;
    static int count;
public:
    Face();
    Face(HalfEdge*, glm::vec3);
    void setEdge(HalfEdge* e);
    HalfEdge* getEdge();
};

#endif // FACE_H
