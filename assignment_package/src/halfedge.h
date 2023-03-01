#ifndef HALFEDGE_H
#define HALFEDGE_H
#include<drawable.h>
#include <vertex.h>
#include <face.h>

class Vertex;
class Face;

class HalfEdge
        : public QListWidgetItem
{
private:
    HalfEdge* nextEdge;
    HalfEdge* symEdge;
    Face* face;
    //vertex between current and nextEdge
    Vertex* vert;
    int idx;

public:
    HalfEdge(int reset = 1);
    HalfEdge(Vertex* v);
    HalfEdge(HalfEdge* next, HalfEdge* sym, Face* f , Vertex* v);
    void setVert(Vertex* v);
    void setFace(Face* f);
    void setNext(HalfEdge* e);
    void setSym(HalfEdge* e);
    HalfEdge* getNext() const;
    HalfEdge* getSym() const;
    Face* getFace() const;
    Vertex* getVert() const;
};

#endif // HALFEDGE_H
