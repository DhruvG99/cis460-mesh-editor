#ifndef MESH_H
#define MESH_H
#include <smartpointerhelp.h>
#include <face.h>
#include <vertex.h>
#include <halfedge.h>
#include <drawable.h>
#include <openglcontext.h>

struct comp
{
    template<typename T>
    bool operator()(const T &l, const T &r) const
    {
        if(glm::all(glm::lessThan(l.first,r.first)))
            return true;
        return false;
    }
};

class Mesh
        : public Drawable
{
private:
public:
    std::vector<uPtr<Face>> faceCollection;
    std::vector<uPtr<HalfEdge>> halfedgeCollection;
    std::vector<uPtr<Vertex>> vertexCollection;
    std::map<std::pair<glm::vec4 , glm::vec4>, HalfEdge*, comp> edgeBounds;

    Mesh(OpenGLContext*);
    ~Mesh();
    //used to test the mesh data structure
    void testMesh() const;
    virtual void create() override;

    GLenum drawMode() override;
};

#endif // MESH_H
