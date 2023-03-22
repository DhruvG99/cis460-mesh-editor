#ifndef SKELETON_H
#define SKELETON_H
#include <smartpointerhelp.h>
#include <openglcontext.h>
#include <joint.h>

class Skeleton
        : public Drawable
{
private:
    std::vector<glm::vec4> pos;
    std::vector<glm::vec4> col;
    std::vector<GLuint> idx;
    int N;
public:
    uPtr<Joint> root;
    Skeleton();
    Skeleton(OpenGLContext*);
    void jointCreate(Joint*);
    void boneCreate(Joint*);
    void computeBindMats() const;
    virtual void create() override;

    GLenum drawMode() override;
};

#endif // SKELETON_H
