#include "camera.h"

#include <la.h>
#include <iostream>


Camera::Camera():
    Camera(400, 400)
{
    look = glm::vec3(0,0,-1);
    up = glm::vec3(0,1,0);
    right = glm::vec3(1,0,0);
}

Camera::Camera(unsigned int w, unsigned int h):
    Camera(w, h, glm::vec3(0,0,10), glm::vec3(0,0,0), glm::vec3(0,1,0))
{}

Camera::Camera(unsigned int w, unsigned int h, const glm::vec3 &e, const glm::vec3 &r, const glm::vec3 &worldUp):
    fovy(45),
    width(w),
    height(h),
    near_clip(0.1f),
    far_clip(1000),
    eye(e),
    ref(r),
    world_up(worldUp)
{
    RecomputeAttributes();
}

Camera::Camera(const Camera &c):
    fovy(c.fovy),
    width(c.width),
    height(c.height),
    near_clip(c.near_clip),
    far_clip(c.far_clip),
    aspect(c.aspect),
    eye(c.eye),
    ref(c.ref),
    look(c.look),
    up(c.up),
    right(c.right),
    world_up(c.world_up),
    V(c.V),
    H(c.H),
    theta(c.theta),
    phi(c.phi)
{}


void Camera::RecomputeAttributes()
{
    look = glm::normalize(ref - eye);
    right = glm::normalize(glm::cross(look, world_up));
    up = glm::cross(right, look);

    float tan_fovy = tan(glm::radians(fovy/2));
    float len = glm::length(ref - eye);
    aspect = width / static_cast<float>(height);
    V = up*len*tan_fovy;
    H = right*len*aspect*tan_fovy;
}

glm::mat4 Camera::getViewProj()
{
    glm::mat4 rottheta = glm::rotate(glm::mat4(1.0f), theta, world_up);
    glm::mat4 rotphi = glm::rotate(glm::mat4(1.0f), phi, look);
    glm::vec3 translation = look * zoom;

    eye += translation;
    look += translation;
    right += translation;
    up += translation;

    eye = glm::vec3(rotphi*glm::vec4(eye,1));
    look = glm::vec3(rotphi*glm::vec4(look,1));
    right = glm::vec3(rotphi*glm::vec4(right,1));
    up = glm::vec3(rotphi*glm::vec4(up,1));

    eye = glm::vec3(rottheta*glm::vec4(eye,1));
    look = glm::vec3(rottheta*glm::vec4(look,1));
    right = glm::vec3(rottheta*glm::vec4(right,1));
    up = glm::vec3(rottheta*glm::vec4(up,1));
    return glm::perspective(glm::radians(fovy), width / (float)height, near_clip, far_clip) * glm::lookAt(eye, ref, up);
}

void Camera::RotateAboutUp(float deg)
{
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(deg), up);
    ref = ref - eye;
    ref = glm::vec3(rotation * glm::vec4(ref, 1));
    ref = ref + eye;
    RecomputeAttributes();
}
void Camera::RotateAboutRight(float deg)
{
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(deg), right);
    ref = ref - eye;
    ref = glm::vec3(rotation * glm::vec4(ref, 1));
    ref = ref + eye;
    RecomputeAttributes();
}

void Camera::TranslateAlongLook(float amt)
{
    glm::vec3 translation = look * amt;
    eye += translation;
    ref += translation;
}

void Camera::TranslateAlongRight(float amt)
{
    glm::vec3 translation = right * amt;
    eye += translation;
    ref += translation;
}
void Camera::TranslateAlongUp(float amt)
{
    glm::vec3 translation = up * amt;
    eye += translation;
    ref += translation;
}

void Camera::RotateTheta(float deg)
{
    theta += deg;
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), deg, up);
    eye = eye - ref;
    eye = glm::vec3(rotation * glm::vec4(eye, 1.f));
    eye = eye + ref;
    RecomputeAttributes();
}

void Camera::RotatePhi(float deg)
{
    phi += deg;
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), deg, right);
    eye = eye - ref;
    eye = glm::vec3(rotation * glm::vec4(eye, 1.f));
    eye = eye + ref;
    RecomputeAttributes();
}

void Camera::Zoom(float amt)
{
    zoom +=amt;
    glm::vec3 translation = look * amt;
    eye += translation;
}
