#ifndef HALFEDGEDISPLAY_H
#define HALFEDGEDISPLAY_H
#include <halfedge.h>
#include <drawable.h>

class HalfEdgeDisplay: public Drawable
{
private:
    HalfEdge* repEdge;
public:
    HalfEdgeDisplay(OpenGLContext*);
    void updateEdge(HalfEdge*);
    HalfEdge* getEdge();
    void create() override;
    GLenum drawMode() override;
};

#endif // HALFEDGEDISPLAY_H
