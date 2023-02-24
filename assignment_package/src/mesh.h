#ifndef MESH_H
#define MESH_H
#include <smartpointerhelp.h>
#include <face.h>
#include <vertex.h>
#include <halfedge.h>
#include <drawable.h>

class Mesh
        : public Drawable
{
private:
public:
    std::vector<uPtr<Face>> faceCollection;
    std::vector<uPtr<HalfEdge>> halfedgeCollection;
    std::vector<uPtr<Vertex>> vertexCollection;
    std::map<std::pair<glm::vec3 , glm::vec3>, HalfEdge*> edgeBounds;
    Mesh();
    ~Mesh();

    void create() override;

//    GLenum drawMode() override;
};

#endif // MESH_H
