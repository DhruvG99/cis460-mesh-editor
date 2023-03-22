#include "joint.h"
#include <iostream>

Joint::Joint()
{

}

Joint::Joint(QString n, glm::vec3 p, glm::vec4 rot, Joint* node)
    : parent(node),
      pos(p),
      rotQuat(glm::angleAxis(glm::degrees(rot[0]),glm::vec3(rot[1], rot[2], rot[3]))),
      name(n)
{
}

glm::mat4 Joint::getLocalTransformation() const
{
    float x = this->rotQuat.x;
    float y = this->rotQuat.y;
    float z = this->rotQuat.z;
    float w = this->rotQuat.w;
    glm::mat4 transMat = glm::mat4(1.0f);
    transMat[3] = glm::vec4(this->pos, 1.0f);

    glm::mat4 rotMat = glm::mat4(glm::vec4(w*w + x*x - y*y - z*z, 2*(x*y + w*z), 2*(x*z - w*y), 0.0f),
                                 glm::vec4(2*(x*y - w*z), w*w - x*x + y*y - z*z, 2*(y*z + w*x), 0.0f),
                                 glm::vec4(2*(x*z + w*y), 2*(y*z - w*x), w*w - x*x - y*y + z*z, 0.0f),
                                 glm::vec4(0.0f,0.0f,0.0f,1.0f));
    return rotMat*transMat;
}

glm::mat4 Joint::getOverallTransformation() const
{
    glm::mat4 overallMat;
    std::vector<glm::mat4> transforms;
    transforms.push_back(this->getLocalTransformation());
    Joint* parentNode = this->parent;
    while(parentNode != nullptr)
    {
        transforms.push_back(parentNode->getLocalTransformation());
        parentNode = parentNode->parent;
    }
    /*
     * neat function!
     * multiplies all transformation matrices in reverse order, starting from root.
    */
    std::reverse(transforms.begin(), transforms.end());
    overallMat = std::accumulate(transforms.begin(), transforms.end(),glm::mat4(1.0f), std::multiplies<glm::mat4>());
    return overallMat;
}


