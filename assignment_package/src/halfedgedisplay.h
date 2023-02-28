#ifndef HALFEDGEDISPLAY_H
#define HALFEDGEDISPLAY_H
#include <halfedge.h>
#include <drawable.h>

class HalfEdgeDisplay :Drawable
{
private:
    HalfEdge* repEdge;
public:
    void create() override;
    void updateEdge(HalfEdge*);
    GLenum drawMode() override;
};

#endif // HALFEDGEDISPLAY_H
