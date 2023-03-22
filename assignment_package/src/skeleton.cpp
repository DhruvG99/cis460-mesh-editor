#include "skeleton.h"
#include <glm/gtx/transform.hpp>
#include <iostream>

Skeleton::Skeleton(OpenGLContext* context):
    Drawable(context)
{
}

GLenum Skeleton::drawMode()
{
    return GL_LINES;
}

void Skeleton::jointCreate(Joint* node)
{
    glm::vec4 v(0.5f, 0.f, 0.f, 1.f);
    float angle = 360.f / 12;

    glm::mat4 overallT = node->getOverallTransformation();
    for(int i = 0; i < 12; i++)
    {
        glm::vec4 vert_pos = overallT *
                glm::rotate(glm::radians(i * angle), glm::vec3(0.f, 0.f, 1.f))*
                v;
        this->idx.push_back(12*N + (i%12));
        this->idx.push_back(12*N + ((i+1)%12));
        this->pos.push_back(vert_pos);
        this->col.push_back(glm::vec4(0.0, 0.0, 1.0, 0.1));
    }
    this->N += 1;

    v = glm::vec4(0.f, 0.5f, 0.f, 1.f);
    for(int i = 0; i < 12; i++)
    {
        glm::vec4 vert_pos = overallT *
                glm::rotate(glm::radians(i * angle), glm::vec3(1.f, 0.f, 0.f))*
                v;
        this->idx.push_back(12*N + (i%12));
        this->idx.push_back(12*N + ((i+1)%12));
        this->pos.push_back(vert_pos);
        this->col.push_back(glm::vec4(1.0, 0.0, 0.0, 0.1));
    }
    this->N += 1;

    v = glm::vec4(0.f, 0.f, 0.5f, 1.f);
    for(int i = 0; i < 12; i++)
    {
        glm::vec4 vert_pos = overallT *
                glm::rotate(glm::radians(i * angle), glm::vec3(0.f, 1.f, 0.f))*
                v;
        this->idx.push_back(12*N + (i%12));
        this->idx.push_back(12*N + ((i+1)%12));
        this->pos.push_back(vert_pos);
        this->col.push_back(glm::vec4(0.0, 1.0, 0.0, 0.1));
    }
    this->N += 1;

    for(auto& child: node->children)
        jointCreate(child.get());
}

void Skeleton::boneCreate(Joint* node)
{
    int i = this->pos.size();
    glm::vec4 parentPos = node->getOverallTransformation()*glm::vec4(0.0f,0.0f,0.0f,1.0f);
    this->pos.push_back(parentPos);
    this->col.push_back(glm::vec4(0.9, 1.0, 0.0, 1));
    for(auto& child: node->children)
    {
        this->idx.push_back(i);
        glm::vec4 childPos = child->getOverallTransformation()*glm::vec4(0.0f,0.0f,0.0f,1.0f);
        this->idx.push_back(pos.size());
        this->pos.push_back(childPos);
        this->col.push_back(glm::vec4(0.2, 0.3 , 0.3, 1));
        if(!(child->children.empty()))
            boneCreate(child.get());
    }
    i++;

}

//print multiple lines
void Skeleton::create()
{
    this->pos.clear();
    this->col.clear();
    this->idx.clear();
    this->N = 0;
    Joint* rootNode = this->root.get();
    jointCreate(rootNode);
    boneCreate(rootNode);

    this->count = idx.size();

    generateIdx();
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufIdx);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx.size()*sizeof(GLuint), idx.data(), GL_STATIC_DRAW);

    generatePos();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufPos);
    mp_context->glBufferData(GL_ARRAY_BUFFER, pos.size()*sizeof(glm::vec4), pos.data(), GL_STATIC_DRAW);

    generateCol();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufCol);
    mp_context->glBufferData(GL_ARRAY_BUFFER, col.size()*sizeof(glm::vec4), col.data(), GL_STATIC_DRAW);
}
