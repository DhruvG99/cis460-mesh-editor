#include "edgedisplay.h"

EdgeDisplay::EdgeDisplay()
{

}

void EdgeDisplay::create()
{
    std::vector<glm::vec4> pos {repVertex->pos};
    //number of vertices being drawn
    count = 1;

    generateIdx();
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufIdx);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint), 0, GL_STATIC_DRAW);

    generatePos();
    //same thing as above, but with arrays, for position
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufPos);
    mp_context->glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4), pos.data(), GL_STATIC_DRAW);

    std::vector<glm::vec4> col {glm::vec4(1, 0, 0, 1)};

    //random color. or leave black
    generateCol();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufCol);
    mp_context->glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4), col.data(), GL_STATIC_DRAW);

}

void EdgeDisplay::updateEdge(HalfEdge* newEdge)
{
    this->repEdge = newEdge;
}

GLenum EdgeDisplay::drawMode()
{
    return GL_LINES;
}