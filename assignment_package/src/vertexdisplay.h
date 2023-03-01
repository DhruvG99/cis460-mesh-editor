#ifndef VERTEXDISPLAY_H
#define VERTEXDISPLAY_H
#include <drawable.h>
#include <vertex.h>

class VertexDisplay: public Drawable
{
protected:
    Vertex* repVertex;
public:
    VertexDisplay(OpenGLContext*);
    void create() override;
    void updateVertex(Vertex*);
    GLenum drawMode() override;
};

#endif // VERTEXDISPLAY_H
