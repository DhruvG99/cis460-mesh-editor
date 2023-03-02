#include "halfedgedisplay.h"

HalfEdgeDisplay::HalfEdgeDisplay(OpenGLContext* context)
    :Drawable(context)
{}

HalfEdge* HalfEdgeDisplay::getEdge()
{
    return this->repEdge;
}

void HalfEdgeDisplay::create()
{
    std::vector<glm::vec4> pos = {repEdge->getVert()->pos};
    std::vector<glm::vec4> col {glm::vec4(1.0, 0.9, 0.0, 1),
                               glm::vec4(1.0, 0.0, 0.0, 1)};
    std::vector<GLuint> idx = {0,1};
    //finding the other vertex (previous edge's vertex)
    HalfEdge* curr = repEdge;
    do
    {
        curr = curr->getNext();
    }while(curr->getNext() != repEdge);

    pos.push_back(curr->getVert()->pos);

    //number of vertices being drawn
    count = 2;

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

void HalfEdgeDisplay::updateEdge(HalfEdge* newEdge)
{
    this->repEdge = newEdge;
}

GLenum HalfEdgeDisplay::drawMode()
{
    return GL_LINES;
}

