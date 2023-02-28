#ifndef FACEDISPLAY_H
#define FACEDISPLAY_H
#include <face.h>
#include <drawable.h>
#include <halfedge.h>

class FaceDisplay: Drawable
{
private:
    Face* repFace;
public:
    void create() override;
    GLenum drawMode() override;
    void updateFace(Face*);
};

#endif // FACEDISPLAY_H
