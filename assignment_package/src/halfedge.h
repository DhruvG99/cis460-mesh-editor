#ifndef HALFEDGE_H
#define HALFEDGE_H
#include<drawable.h>
#include <vertex.h>
#include <face.h>

class Vertex;
class Face;

class HalfEdge
//        : public Drawable
{
private:
    HalfEdge* nextEdge;
    HalfEdge* symEdge;
    Face* face;
    //vertex between current and nextEdge
    Vertex* vert;
    int idx;
    static int count;
public:
    HalfEdge();
    HalfEdge(Vertex* v);
    HalfEdge(HalfEdge* next, HalfEdge* sym, Face* f , Vertex* v);
    void setVert(Vertex* v);
    void setFace(Face* f);
    void setNext(HalfEdge* e);
    void setSym(HalfEdge* e);
    HalfEdge* getNext();
    Vertex* getVert();
};

#endif // HALFEDGE_H
