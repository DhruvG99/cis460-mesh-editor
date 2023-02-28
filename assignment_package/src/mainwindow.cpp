#include "mainwindow.h"
#include <ui_mainwindow.h>
#include "cameracontrolshelp.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <QFileDialog>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)

{
    ui->setupUi(this);
    //context
    ui->mygl->setFocus();

    addToListWidgets();

    connect(ui->pushButton, SIGNAL(clicked(bool)),
            this, SLOT(on_pushButtonObj()));

    connect(ui->vertsListWidget, SIGNAL(itemSelectionChanged()),
                         this, SLOT(setSelected()));

    connect(ui->halfEdgesListWidget, SIGNAL(itemSelectionChanged()),
                         this, SLOT(setSelected()));

    connect(ui->facesListWidget, SIGNAL(itemSelectionChanged()),
                         this, SLOT(setSelected()));

}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setSelected()
{
    //use selectedItems()
}

void MainWindow::addToListWidgets()
{
    for(auto& vert: ui->mygl->m_mesh.vertexCollection)
    {
        ui->vertsListWidget->addItem(vert.get());
    }

    for(auto& edge: ui->mygl->m_mesh.halfedgeCollection)
    {
        ui->halfEdgesListWidget->addItem(edge.get());
    }

    for(auto& face: ui->mygl->m_mesh.faceCollection)
    {
        ui->facesListWidget->addItem(face.get());
    }
}

void MainWindow::on_pushButtonObj()
{
    QString filename = QFileDialog::getOpenFileName(0,
                                                    QString("Load OBJ"),
                                                    QDir::currentPath().append(QString("../..")),
                                                    QString("*.obj"));
    std::ifstream myobj (filename.toStdString());
    std::string line;
    ui->mygl->m_mesh.faceCollection.clear();
    ui->mygl->m_mesh.halfedgeCollection.clear();
    ui->mygl->m_mesh.vertexCollection.clear();

    //filling vertexCollection for Mesh
    while(myobj.good() && std::getline(myobj, line))
    {
        std::string val0;
        std::stringstream linestream(line);
        linestream >> val0;
        if(val0 == "v")
        {
            float px, py, pz;
            linestream >> px;
            linestream >> py;
            linestream >> pz;
            glm::vec4 pos = glm::vec4(px,py,pz,1);
            uPtr<Vertex> v = mkU<Vertex>(pos);

            ui->mygl->m_mesh.vertexCollection.push_back(std::move(v));;
        }
        else if(val0 == "vt")
            break;
    }

    myobj.seekg(0);
    //reading file to get vertex indices and link edges
    while(myobj.good() && std::getline(myobj, line))
    {
        std::string val0;
        std::stringstream linestream(line);
        linestream >> val0;
        if(val0 == "f")
        {
            HalfEdge* prev;
            uPtr<Face> f = mkU<Face>();
            Face* currFace = f.get();
            ui->mygl->m_mesh.faceCollection.push_back(std::move(f));

            std::string triplet;
            int vertInd; // vt, vn; excluded
            int tripCount = 0; //id of vertex
            while(linestream >> triplet)
            {
                vertInd = std::stoi(triplet.substr(0, triplet.find('/')));
                Vertex *v = ui->mygl->m_mesh.vertexCollection[vertInd-1].get();
                uPtr<HalfEdge> edge = mkU<HalfEdge>();
                HalfEdge* currEdge = edge.get();

                //sets the vertex as the leading vertex for edge,
                //and edge for vertex
                edge->setVert(v);
                //sets the face for edge
                edge->setFace(currFace);

                ui->mygl->m_mesh.halfedgeCollection.push_back(std::move(edge));

                if(tripCount != 0)
                {
                    prev->setNext(currEdge);
                }
                else
                {
                    currFace->setEdge(currEdge); //used for last edge
                }

                prev = currEdge;
                tripCount += 1;

            }
            //last edge to complete loop
            prev->setNext(currFace->getEdge());
        }
    }

    //Linking sym edges
    for(const auto& f: ui->mygl->m_mesh.faceCollection)
    {
        //The edge I'm trying to set a sym of
        //and also adding the bounds of
        HalfEdge* mapEdge = f->getEdge();
        do
        {
            //the edge behind my mapEdge
            HalfEdge* currEdge = mapEdge;
            do
            {
                currEdge = currEdge->getNext();

            }while(currEdge->getNext() != mapEdge);

            Vertex* prevV = currEdge->getVert();
            Vertex* nextV = mapEdge->getVert();
            //if the above pos already exist in my map
            //meaning I need to set a sym with that map value - edge
            //i'm comparing the reverse of the pair to because
            //the symEdges will have symmetrical positions
            std::pair<glm::vec4, glm::vec4> p = std::make_pair(prevV->pos, nextV->pos);
            if(ui->mygl->m_mesh.edgeBounds.find(std::make_pair(nextV->pos, prevV->pos)) != ui->mygl->m_mesh.edgeBounds.end())
            {
                mapEdge->setSym(ui->mygl->m_mesh.edgeBounds[p]);
            }

            ui->mygl->m_mesh.edgeBounds[p] = mapEdge; //map insertion
            mapEdge = mapEdge->getNext();
        }while(mapEdge != f->getEdge());
    }

    //calling create() to create mesh vbo data
    //use vbo data to render Mesh obj
    ui->mygl->m_mesh.create();
    ui->mygl->meshCreated = true;
    ui->mygl->update();
}

void MainWindow::on_actionQuit_triggered()
{
    QApplication::exit();
}

void MainWindow::on_actionCamera_Controls_triggered()
{
    CameraControlsHelp* c = new CameraControlsHelp();
    c->show();
}
