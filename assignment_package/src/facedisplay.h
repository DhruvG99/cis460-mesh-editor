#ifndef FACEDISPLAY_H
#define FACEDISPLAY_H
#include <face.h>
#include <drawable.h>
#include <halfedge.h>

class FaceDisplay: public Drawable
{
private:
    Face* repFace;
public:
    FaceDisplay(OpenGLContext*);
    void create() override;
    GLenum drawMode() override;
    void updateFace(Face*);
    Face* getFace();
};

#endif // FACEDISPLAY_H
