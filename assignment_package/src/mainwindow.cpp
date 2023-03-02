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

    connect(ui->pushButton, SIGNAL(clicked(bool)),
            this, SLOT(on_pushButtonObj()));

    connect(ui->vertsListWidget, SIGNAL(itemSelectionChanged()),
                         this, SLOT(slot_selectVert()));

    connect(ui->halfEdgesListWidget, SIGNAL(itemSelectionChanged()),
                         this, SLOT(slot_selectEdge()));

    connect(ui->facesListWidget, SIGNAL(itemSelectionChanged()),
                         this, SLOT(slot_selectFace()));

}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slot_selectVert()
{
    QListWidgetItem* currentItem = ui->vertsListWidget->currentItem();
    Vertex* v =  static_cast<Vertex*>(currentItem);
    ui->mygl->m_vert.updateVertex(v);
    ui->mygl->m_vert.create();
    ui->mygl->isVertSelected = true;
    ui->mygl->update();
    //use selectedItems()
}

void MainWindow::slot_selectEdge()
{
    QListWidgetItem* currentItem = ui->halfEdgesListWidget->currentItem();
    HalfEdge* e =  static_cast<HalfEdge*>(currentItem);
    ui->mygl->m_edge.updateEdge(e);
    ui->mygl->m_edge.create();
    ui->mygl->isEdgeSelected = true;
    ui->mygl->update();
    //use selectedItems()
}

void MainWindow::slot_selectFace()
{
    QListWidgetItem* currentItem = ui->facesListWidget->currentItem();
    Face* f =  static_cast<Face*>(currentItem);
    ui->mygl->m_face.updateFace(f);
    ui->mygl->m_face.create();
    ui->mygl->isFaceSelected = true;
    ui->mygl->update();
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

void MainWindow::resetLists()
{
    ui->mygl->isMeshCreated = false;
    ui->mygl->isVertSelected = false;
    ui->mygl->isEdgeSelected = false;
    ui->mygl->isFaceSelected = false;

    ui->vertsListWidget->clear();
    ui->halfEdgesListWidget->clear();
    ui->facesListWidget->clear();
}

void MainWindow::on_pushButtonObj()
{
    //reset mesh
    //PROBLEMMMMMMMM
    ui->mygl->m_mesh.vertexCollection.clear();
    ui->mygl->m_mesh.halfedgeCollection.clear();
    ui->mygl->m_mesh.faceCollection.clear();
    ui->mygl->m_mesh.edgeBounds.clear();
    //reset list widget
    resetLists();

    QString filename = QFileDialog::getOpenFileName(0,
                                                    QString("Load OBJ"),
                                                    QDir::currentPath().append(QString("../..")),
                                                    QString("*.obj"));
    std::ifstream myobj (filename.toStdString());
    std::string line;
    //to update the static counts whenever a new mesh
    //is loaded. (used for list widgets)
    int vertCount = 0;
    int edgeCount = 0;
    int faceCount = 0;

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
            uPtr<Vertex> v = mkU<Vertex>(pos, vertCount);
            vertCount++;
            ui->mygl->m_mesh.vertexCollection.push_back(std::move(v));
        }
        else if(val0 == "vt")
            break;
    }

    myobj.seekg(0);
    //reading file to get vertex indices and link edges
    //and filling facecollection and halfedgecollection
    while(myobj.good() && std::getline(myobj, line))
    {
        std::string val0;
        std::stringstream linestream(line);
        linestream >> val0;
        if(val0 == "f")
        {
            glm::vec4 col = {(rand()%256)/255.0f,
                            (rand()%256)/255.0f,
                            (rand()%256)/255.0f,
                            1.0f};
            HalfEdge* prev;
            uPtr<Face> f = mkU<Face>(col, faceCount);
            faceCount++;
            Face* currFace = f.get();
            ui->mygl->m_mesh.faceCollection.push_back(std::move(f));

            std::string triplet;
            int vertInd; // vt, vn; excluded
            int tripCount = 0; //id of vertex
            while(linestream >> triplet)
            {
                vertInd = std::stoi(triplet.substr(0, triplet.find('/')));
                Vertex *v = ui->mygl->m_mesh.vertexCollection[vertInd-1].get();
                uPtr<HalfEdge> edge = mkU<HalfEdge>(edgeCount);
                edgeCount++;
                HalfEdge* currEdge = edge.get();

                //sets the vertex as the leading vertex for edge,
                //and edge for vertex
                edge->setVert(v);
                //sets the face for edge
                edge->setFace(currFace);

                ui->mygl->m_mesh.halfedgeCollection.push_back(std::move(edge));

                if(tripCount != 0)
                    prev->setNext(currEdge);
                else
                    currFace->setEdge(currEdge); //used for last edge

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
        std::cout<<"Face"<<std::endl;
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
            //meaning I need to set a sym with that unord_map value - edge
            //i'm comparing the reverse of the pair to because
            //the symEdges will have symmetrical positions
            std::pair<glm::vec4, glm::vec4> p = std::make_pair(prevV->pos, nextV->pos);
            std::pair<glm::vec4, glm::vec4> symp = std::make_pair(nextV->pos, prevV->pos);
            std::cout<<ui->mygl->m_mesh.edgeBounds.size()<<std::endl;
            if(ui->mygl->m_mesh.edgeBounds.find(symp) != ui->mygl->m_mesh.edgeBounds.end())
                mapEdge->setSym(ui->mygl->m_mesh.edgeBounds[symp]);
            else
                ui->mygl->m_mesh.edgeBounds[p] = mapEdge; //map insertion

            mapEdge = mapEdge->getNext();
        }while(mapEdge != f->getEdge());
    }
    //calling create() to create mesh vbo data
    //use vbo data to render Mesh obj
    addToListWidgets();
    ui->mygl->m_mesh.create();
    ui->mygl->isMeshCreated = true;
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
