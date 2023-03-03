#ifndef MESH_H
#define MESH_H
#include <smartpointerhelp.h>
#include <face.h>
#include <vertex.h>
#include <halfedge.h>
#include <drawable.h>
#include <openglcontext.h>
#include <iostream>

//hash function creation from here
//https://stackoverflow.com/questions/32685540/why-cant-i-compile-an-unordered-map-with-a-pair-as-key
class pair_hash
{
public:
    std::size_t operator () (const std::pair<glm::vec4,glm::vec4>& p) const
    {
        auto& p1 = p.first;
        auto& p2 = p.second;
        std::size_t seed;
        seed = (std::hash<float>()(p1.x) ^ std::hash<float>()(p2.x)) >> 3;
        seed ^= (std::hash<float>()(p1.y) ^ std::hash<float>()(p2.y)) << 5;
        seed ^= (std::hash<float>()(p1.z) ^ std::hash<float>()(p2.z)) >> 7;

        return seed;
    }
};

class pair_equal
{
public:
    bool operator () (const std::pair<glm::vec4,glm::vec4>& p1, const std::pair<glm::vec4,glm::vec4>& p2) const
    {
        return (glm::all(glm::equal(p1.first, p2.first)) &&
                glm::all(glm::equal(p1.second, p2.second)));
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
    std::unordered_map<std::pair<glm::vec4 , glm::vec4>, HalfEdge*, pair_hash, pair_equal> edgeBounds;

    Mesh(OpenGLContext*);
    //used to test the mesh data structure
    void testMesh() const;
    virtual void create() override;

    GLenum drawMode() override;
};

#endif // MESH_H
