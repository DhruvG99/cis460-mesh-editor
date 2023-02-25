#ifndef EDGEDISPLAY_H
#define EDGEDISPLAY_H
#include <halfedge.h>

class EdgeDisplay: public Drawable
{
protected:
    HalfEdge* repEdge;
public:
    EdgeDisplay();
    void create() override;
    void updateEdge(HalfEdge*);
    GLenum drawMode() override;
};

#endif // EDGEDISPLAY_H
