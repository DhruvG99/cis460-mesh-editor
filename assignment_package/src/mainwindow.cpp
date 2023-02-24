#include "mainwindow.h"
#include <ui_mainwindow.h>
#include "cameracontrolshelp.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <mesh.h>



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->mygl->setFocus();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButtonObj()
{
    std::ifstream myobj ("cube.obj");
    Mesh mesh;
    std::string line;
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
            glm::vec3 pos = glm::vec3(px,py,pz);
            uPtr<Vertex> v = mkU<Vertex>(pos);
            mesh.vertexCollection.push_back(std::move(v));;
        }
        else if(val0 == "vt") //saving time?
            break;
    }

    myobj.seekg(0);
    while(myobj.good() && std::getline(myobj, line))
    {
        std::string val0;
        std::stringstream linestream(line);
        linestream >> val0;
        if(val0 == "f")
        {
            uPtr<Face> f = mkU<Face>();
            Face* currFace = f.get();
            mesh.faceCollection.push_back(std::move(f));
            HalfEdge* prev;
            int vertInd; // vt, vn; excluded
            int tripCount = 0; //id of vertex
            while(linestream >> vertInd)
            {
                Vertex *v = mesh.vertexCollection[vertInd-1].get();
                uPtr<HalfEdge> edge = mkU<HalfEdge>();
                HalfEdge* currEdge = edge.get();

                //sets the vertex for edge, and edge for vertex
                edge->setVert(v);
                //sets the face for edge
                edge->setFace(currFace);

                mesh.halfedgeCollection.push_back(std::move(edge));

                if(tripCount != 0)
                    prev->setNext(currEdge);
                else
                    f->setEdge(currEdge); //used for last edge

                prev = currEdge;
                tripCount += 1;
            }
            //where is v pointing??
            //last edge to complete loop
            prev->setNext(f->getEdge());
        }
    }

    for(const auto& f: mesh.faceCollection)
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
            std::pair<glm::vec3, glm::vec3> p = std::make_pair(prevV->pos, nextV->pos);
            if(mesh.edgeBounds.find(std::make_pair(nextV->pos, prevV->pos)) == mesh.edgeBounds.end())
            {
                mapEdge -> setSym(mesh.edgeBounds[p]);
            }

            mesh.edgeBounds[p] = mapEdge; //map insertion
            mapEdge = mapEdge->getNext();

        }while(mapEdge != f->getEdge());
    }
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
