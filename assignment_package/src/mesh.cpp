#include "mesh.h"
#include <smartpointerhelp.h>
#include <la.h>
#include <iostream>

Mesh::Mesh(OpenGLContext* context):
    Drawable(context)
{

}

GLenum Mesh::drawMode()
{
    return GL_TRIANGLES;
}

Mesh::~Mesh()
{

}

void Mesh::testMesh()
{
    int i =1;
    for(const uPtr<Face> &f: this->faceCollection)
    {
        std::cout<<"Face: "<<i<<std::endl;
        i++;
        HalfEdge* curr = f->getEdge();
        do
        {
            glm::vec4 pos = curr->getVert()->pos;
            std::cout << pos.x << ", "
                    << pos.y << ", "
                   << pos.z << std::endl;
            curr = curr->getNext();
        }while(curr != f->getEdge());
    }
}

//create VBO data
void Mesh::create()
{
    std::vector<GLuint> idx;
    std::vector<glm::vec4> pos;
    int numVert = 0;
    int offsetVert = 0;
    for(const uPtr<Face> &f: this->faceCollection)
    {
        HalfEdge* curr = f->getEdge();
        do
        {
            numVert++;
            glm::vec4 p = curr->getVert()->pos;
            pos.push_back(p);
            curr = curr->getNext();
        }while(curr != f->getEdge());

        for(int i =0; i< numVert-2; i++)
        {
            idx.push_back(offsetVert + 0);
            idx.push_back(offsetVert + i+1);
            idx.push_back(offsetVert + i+2);
        }
        offsetVert += numVert;
        numVert = 0;
    }

    //number of vertices being drawn
    count = idx.size();

    //comments for my own understanding
    generateIdx();
    //binds bufPos to this VBO
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufIdx);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx.size() * sizeof(GLuint), idx.data(), GL_STATIC_DRAW);

    generatePos();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufPos);
    mp_context->glBufferData(GL_ARRAY_BUFFER, pos.size() * sizeof(glm::vec4), pos.data(), GL_STATIC_DRAW);

//    generateNor();
//    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufNor);
//    mp_context->glBufferData(GL_ARRAY_BUFFER, nor.size() * sizeof(glm::vec4), nor.data(), GL_STATIC_DRAW);

//    //random color. or leave black
//    generateCol();
//    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufCol);
//    mp_context->glBufferData(GL_ARRAY_BUFFER, col.size() * sizeof(glm::vec4), col.data(), GL_STATIC_DRAW);
}

