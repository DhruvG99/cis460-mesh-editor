#ifndef MYGL_H
#define MYGL_H

#include <openglcontext.h>
#include <utils.h>
#include <shaderprogram.h>
#include <scene/squareplane.h>
#include "camera.h"

#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <mesh.h>
#include <skeleton.h>
#include <vertexdisplay.h>
#include <halfedgedisplay.h>
#include <facedisplay.h>


class MyGL
    : public OpenGLContext
{
    Q_OBJECT
private:
    SquarePlane m_geomSquare;// The instance of a unit cylinder we can use to render any cylinder
    ShaderProgram m_progLambert;// A shader program that uses lambertian reflection
    ShaderProgram m_progFlat;// A shader program that uses "flat" reflection (no shadowing at all)
    GLuint vao; // A handle for our vertex array object. This will store the VBOs created in our geometry classes.
                // Don't worry too much about this. Just know it is necessary in order to render geometry.

    Camera m_glCamera;
public:
    explicit MyGL(QWidget *parent = nullptr);
    ~MyGL();
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

    bool isSkeletonCreated;
    bool isMeshCreated;
    bool isVertSelected;
    bool isEdgeSelected;
    bool isFaceSelected;
    Skeleton m_skeleton;
    Mesh m_mesh;
    VertexDisplay m_vert;
    HalfEdgeDisplay m_edge;
    FaceDisplay m_face;

protected:
    void keyPressEvent(QKeyEvent *e);


};


#endif // MYGL_H
