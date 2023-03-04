#include "mygl.h"
#include <la.h>
#include <iostream>
#include <QApplication>
#include <QKeyEvent>


MyGL::MyGL(QWidget *parent)
    : OpenGLContext(parent),
      m_geomSquare(this),
      m_progLambert(this), m_progFlat(this),
      m_glCamera(),
      isMeshCreated(false),
      isVertSelected(false),
      isEdgeSelected(false),
      isFaceSelected(false),
      m_mesh(this),
      m_vert(this),
      m_edge(this),
      m_face(this)
{
    setFocusPolicy(Qt::StrongFocus);
}

MyGL::~MyGL()
{
    makeCurrent();
    glDeleteVertexArrays(1, &vao);
    m_geomSquare.destroy();
    m_mesh.destroy();
    m_vert.destroy();
    m_edge.destroy();
    m_face.destroy();
}

void MyGL::initializeGL()
{
    // Create an OpenGL context using Qt's QOpenGLFunctions_3_2_Core class
    // If you were programming in a non-Qt context you might use GLEW (GL Extension Wrangler)instead
    initializeOpenGLFunctions();
    // Print out some information about the current OpenGL context
    debugContextVersion();

    // Set a few settings/modes in OpenGL rendering
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);

    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    // Set the size with which points should be rendered
    glPointSize(5);
    // Set the color with which the screen is filled at the start of each render call.
    glClearColor(0.5, 0.5, 0.5, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    printGLErrorLog();

    // Create a Vertex Attribute Object
    glGenVertexArrays(1, &vao);

    //Create the instances of Cylinder and Sphere.
    m_geomSquare.create();

    // Create and set up the diffuse shader
    m_progLambert.create(":/glsl/lambert.vert.glsl", ":/glsl/lambert.frag.glsl");
    // Create and set up the flat lighting shader
    m_progFlat.create(":/glsl/flat.vert.glsl", ":/glsl/flat.frag.glsl");


    // We have to have a VAO bound in OpenGL 3.2 Core. But if we're not
    // using multiple VAOs, we can just bind one once.
    glBindVertexArray(vao);
}

void MyGL::resizeGL(int w, int h)
{
    //This code sets the concatenated view and perspective projection matrices used for
    //our scene's camera view.
    m_glCamera = Camera(w, h);
    glm::mat4 viewproj = m_glCamera.getViewProj();

    // Upload the view-projection matrix to our shaders (i.e. onto the graphics card)

    m_progLambert.setViewProjMatrix(viewproj);
    m_progFlat.setViewProjMatrix(viewproj);

    printGLErrorLog();
}

//This function is called by Qt any time your GL window is supposed to update
//For example, when the function update() is called, paintGL is called implicitly.
void MyGL::paintGL()
{
    if(isMeshCreated)
    {
        glClearColor(0.8,0.7,0.6,1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_progFlat.setViewProjMatrix(m_glCamera.getViewProj());
        m_progFlat.setModelMatrix(glm::mat4(1.f));
        m_progFlat.draw(m_mesh);
        glDisable(GL_DEPTH_TEST);
        if(isFaceSelected)
            m_progFlat.draw(m_face);
        if(isEdgeSelected)
            m_progFlat.draw(m_edge);
        if(isVertSelected)
            m_progFlat.draw(m_vert);
        glEnable(GL_DEPTH_TEST);
    }
    else
    {
        glClearColor(0.2,0.0,0.4,1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
//    m_progLambert.setViewProjMatrix(m_glCamera.getViewProj());
//    m_progLambert.setCamPos(m_glCamera.eye);
//    m_progLambert.setModelMatrix(model);
//    //Draw the example sphere using our lambert shader
//    m_progLambert.draw(m_geomSquare);

//    model = glm::translate(glm::mat4(1.0f), glm::vec3(2,2,0)) * glm::rotate(glm::mat4(1.0f), glm::radians(-45.0f), glm::vec3(0,0,1));
//    m_progLambert.setModelMatrix(model);
//    m_progLambert.draw(m_geomSquare);
}

void MyGL::keyPressEvent(QKeyEvent *e)
{
    float amount = 2.0f;
    if(e->modifiers() & Qt::ShiftModifier){
        amount = 10.0f;
    }
    // http://doc.qt.io/qt-5/qt.html#Key-enum
    // This could all be much more efficient if a switch
    // statement were used
    if (e->key() == Qt::Key_Escape) {
        QApplication::quit();
    } else if (e->key() == Qt::Key_Right) {
        m_glCamera.RotateAboutUp(-amount);
    } else if (e->key() == Qt::Key_Left) {
        m_glCamera.RotateAboutUp(amount);
    } else if (e->key() == Qt::Key_Up) {
        m_glCamera.RotateAboutRight(-amount);
    } else if (e->key() == Qt::Key_Down) {
        m_glCamera.RotateAboutRight(amount);
    } else if (e->key() == Qt::Key_1) {
        m_glCamera.fovy += amount;
    } else if (e->key() == Qt::Key_2) {
        m_glCamera.fovy -= amount;
    } else if (e->key() == Qt::Key_W) {
        m_glCamera.TranslateAlongLook(amount);
    } else if (e->key() == Qt::Key_S) {
        m_glCamera.TranslateAlongLook(-amount);
    } else if (e->key() == Qt::Key_D) {
        m_glCamera.TranslateAlongRight(amount);
    } else if (e->key() == Qt::Key_A) {
        m_glCamera.TranslateAlongRight(-amount);
    } else if (e->key() == Qt::Key_Q) {
        m_glCamera.TranslateAlongUp(-amount);
    } else if (e->key() == Qt::Key_E) {
        m_glCamera.TranslateAlongUp(amount);
    } else if (e->key() == Qt::Key_R) {
        m_glCamera = Camera(this->width(), this->height());
    } else if (e->key() == Qt::Key_N) {
        if(isEdgeSelected)
        {
            HalfEdge* nextEdge = m_edge.getEdge()->getNext();
            m_edge.updateEdge(nextEdge);
            m_edge.create();
            isEdgeSelected = true;
        }
    } else if (e->key() == Qt::Key_M) {
        if(isEdgeSelected)
        {
            HalfEdge* symEdge = m_edge.getEdge()->getSym();
            m_edge.updateEdge(symEdge);
            m_edge.create();
            isEdgeSelected = true;
        }
    } else if (e->key() == Qt::Key_F) {
        if(isEdgeSelected)
        {
            Face* newFace = m_edge.getEdge()->getFace();
            m_face.updateFace(newFace);
            m_face.create();
            isFaceSelected = true;
        }
    } else if (e->key() == Qt::Key_V) {
        if(isEdgeSelected)
        {
            Vertex* newVert = m_edge.getEdge()->getVert();
            m_vert.updateVertex(newVert);
            m_vert.create();
            isVertSelected = true;
        }
    } else if (e->key() == Qt::Key_H) {
        //e->keyCombination() == QKeyCombination(Qt::ShiftModifier, Qt::Key_H
        if(e->modifiers() == Qt::ShiftModifier)
        {
            if(isFaceSelected)
            {
                HalfEdge* newEdge = m_face.getFace()->getEdge();
                m_edge.updateEdge(newEdge);
                m_edge.create();
                isEdgeSelected = true;
            }
        }
        else if(isVertSelected)
        {
            HalfEdge* newEdge = m_vert.getVertex()->halfedge;
            m_edge.updateEdge(newEdge);
            m_edge.create();
            isEdgeSelected = true;
        }
    }
    m_glCamera.RecomputeAttributes();
    update();  // Calls paintGL, among other things
}
