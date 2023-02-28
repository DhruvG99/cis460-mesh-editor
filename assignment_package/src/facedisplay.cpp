#include "facedisplay.h"

void FaceDisplay::create()
{
    std::vector<glm::vec4> pos;
    std::vector<GLuint> idx;
    int edgeCount = 0;
    //finding the other vertex
    HalfEdge* curr = repFace->getEdge();
    do
    {
        idx.push_back(edgeCount);
        idx.push_back(edgeCount+1);
        pos.push_back(curr->getVert()->pos);
        curr = curr->getNext();
        edgeCount++;
    }while(curr->getNext() != repFace->getEdge());
    //for the first edge.
    pos.push_back(curr->getVert()->pos);
    idx.push_back(edgeCount);
    idx.push_back(0);
    //number of vertices being drawn
    count = idx.size();

    generateIdx();
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufIdx);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint), idx.data(), GL_STATIC_DRAW);

    generatePos();
    //same thing as above, but with arrays, for position
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufPos);
    mp_context->glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4), pos.data(), GL_STATIC_DRAW);

    std::vector<glm::vec4> col {glm::vec4(1, 0, 0, 1),
                               glm::vec4(1, 0, 0, 1),
                               glm::vec4(1, 0, 0, 1),
                               glm::vec4(1, 0, 0, 1)};

    //random color. or leave black
    generateCol();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufCol);
    mp_context->glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4), col.data(), GL_STATIC_DRAW);
}

GLenum FaceDisplay::drawMode()
{
    return GL_LINES;
}

void FaceDisplay::updateFace(Face* face)
{
    this->repFace = face;
}
