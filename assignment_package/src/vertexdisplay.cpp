#include "vertexdisplay.h"

VertexDisplay::VertexDisplay(OpenGLContext* context)
    :Drawable(context)
{}

void VertexDisplay::create()
{
    std::vector<glm::vec4> pos {repVertex->pos};
    std::vector<glm::vec4> col {glm::vec4(1, 1, 1, 1)};
    //number of vertices being drawn
    count = 1;

    generateIdx();
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufIdx);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint), 0, GL_STATIC_DRAW);

    generatePos();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufPos);
    mp_context->glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4), pos.data(), GL_STATIC_DRAW);

    generateCol();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufCol);
    mp_context->glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4), col.data(), GL_STATIC_DRAW);

}

void VertexDisplay::updateVertex(Vertex* newVert)
{
    this->repVertex = newVert;
}

Vertex* VertexDisplay::getVertex()
{
    return this->repVertex;
}

GLenum VertexDisplay::drawMode()
{
    return GL_POINTS;
}
