#ifndef JOINT_H
#define JOINT_H
#include <la.h>
#include <smartpointerhelp.h>
#include <glm/gtc/quaternion.hpp>
#include <drawable.h>
#include <QTreeWidgetItem>

class Joint
        : public QTreeWidgetItem
//        : public Drawable
{
private:

    Joint* parent;
    glm::vec3 pos;
    glm::quat rotQuat;
    glm::mat4 bindMat;
public:
    QString name;
    std::vector<uPtr<Joint>> children;

    Joint();
    Joint(QString, glm::vec3, glm::vec4, Joint*);
    glm::mat4 getLocalTransformation() const;
    glm::mat4 getOverallTransformation() const;
//    void create() override;
//    GLenum drawMode() override;
};

#endif // JOINT_H
