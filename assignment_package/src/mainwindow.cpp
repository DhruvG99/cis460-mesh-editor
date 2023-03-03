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
    connect(ui->pushButton_2, SIGNAL(clicked(bool)),
            this, SLOT(on_pushButtonSplit()));
    connect(ui->pushButton_3, SIGNAL(clicked(bool)),
            this, SLOT(on_pushButtonTriangulate()));

    connect(ui->vertsListWidget, SIGNAL(itemClicked(QListWidgetItem*)),
                         this, SLOT(slot_selectVert(QListWidgetItem*)));

    connect(ui->halfEdgesListWidget, SIGNAL(itemClicked(QListWidgetItem*)),
                         this, SLOT(slot_selectEdge(QListWidgetItem*)));

    connect(ui->facesListWidget, SIGNAL(itemClicked(QListWidgetItem*)),
                         this, SLOT(slot_selectFace(QListWidgetItem*)));

}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slot_selectVert(QListWidgetItem *item)
{
    QListWidgetItem* currentItem = item;
    Vertex* v =  static_cast<Vertex*>(currentItem);
    ui->mygl->m_vert.updateVertex(v);
    ui->mygl->m_vert.create();
    ui->mygl->isVertSelected = true;
    ui->mygl->update();
    //use selectedItems()
}

void MainWindow::slot_selectEdge(QListWidgetItem* item)
{
    QListWidgetItem* currentItem = item;
    HalfEdge* e =  static_cast<HalfEdge*>(currentItem);
    ui->mygl->m_edge.updateEdge(e);
    ui->mygl->m_edge.create();
    ui->mygl->isEdgeSelected = true;
    ui->mygl->update();
    //use selectedItems()
}

void MainWindow::slot_selectFace(QListWidgetItem* item)
{
    QListWidgetItem* currentItem = item;
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
    ui->mygl->m_mesh.vertexCollection.clear();
    ui->mygl->m_mesh.halfedgeCollection.clear();
    ui->mygl->m_mesh.faceCollection.clear();
    ui->mygl->m_mesh.edgeBounds.clear();
    //reset list widget and all bool vals
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
            HalfEdge* prev;
            uPtr<Face> f = mkU<Face>(faceCount);
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

void MainWindow::on_pushButtonSplit()
{
    if(ui->mygl->isEdgeSelected)
    {
        HalfEdge* selectedEdge = ui->mygl->m_edge.getEdge();
        HalfEdge* symEdge = selectedEdge->getSym();
        HalfEdge* currEdge = selectedEdge;
        do
        {
            currEdge = currEdge->getNext();

        }while(currEdge->getNext() != selectedEdge);

        Vertex* prevV = currEdge->getVert();
        Vertex* nextV = selectedEdge->getVert();
        glm::vec4 newPos = 0.5f*(prevV->pos + nextV->pos);

        uPtr<Vertex> newV = mkU<Vertex>(newPos);
        uPtr<HalfEdge> newEdge = mkU<HalfEdge>();
        uPtr<HalfEdge> newSymEdge = mkU<HalfEdge>();

        //setting next ptr for new edge
        newEdge->setNext(selectedEdge->getNext());
        //setting vertex for new edge
        newEdge->setVert(nextV);
        //setting face pointer for new edge
        newEdge->setFace(selectedEdge->getFace());
        //setting next ptr for old edge
        selectedEdge->setNext(newEdge.get());
        //setting new vertex for old edge
        selectedEdge->setVert(newV.get());

        //same operations as above, but for the symmetric edge
        newSymEdge->setNext(symEdge->getNext());
        newSymEdge->setVert(prevV);
        newSymEdge->setFace(symEdge->getFace());
        symEdge->setNext(newSymEdge.get());
        symEdge->setVert(newV.get());

        //setting new symmetric edges
        selectedEdge->setSym(newSymEdge.get());
        symEdge->setSym(newEdge.get());

        ui->vertsListWidget->addItem(newV.get());
        ui->halfEdgesListWidget->addItem(newEdge.get());
        ui->halfEdgesListWidget->addItem(newSymEdge.get());

        //adding new objects to the mesh
        ui->mygl->m_mesh.vertexCollection.push_back(std::move(newV));
        ui->mygl->m_mesh.halfedgeCollection.push_back(std::move(newEdge));
        ui->mygl->m_mesh.halfedgeCollection.push_back(std::move(newSymEdge));

        ui->mygl->m_mesh.create();
        ui->mygl->update();
    }
}

void MainWindow::on_pushButtonTriangulate()
{
    if(ui->mygl->isFaceSelected)
    {
        Face* selectedFace = ui->mygl->m_face.getFace();
        HalfEdge* faceEdge = selectedFace->getEdge();
        Vertex* initV = faceEdge->getVert(); //common vertex to all triangles

        HalfEdge* currEdge = faceEdge->getNext()->getNext();
        //maybe change to while()
        while(currEdge->getNext() != faceEdge)
        {
            //third vertex of triangle
            Vertex* v = currEdge->getVert();
            uPtr<HalfEdge> newEdge = mkU<HalfEdge>();
            uPtr<HalfEdge> newSymEdge = mkU<HalfEdge>();
            uPtr<Face> newFace = mkU<Face>();

            HalfEdge* tempEdge = currEdge->getNext();
            currEdge->setNext(newEdge.get());
            newEdge->setVert(initV);
            //only true for first face??
            newEdge->setNext(faceEdge->getNext());
            newEdge->setFace(selectedFace);
            newEdge->setSym(newSymEdge.get());
            selectedFace->setEdge(newEdge.get());

            faceEdge->setNext(newSymEdge.get());
            newSymEdge->setNext(tempEdge);
            newSymEdge->setVert(v);
            newSymEdge->setFace(newFace.get());
            newFace->setEdge(newSymEdge.get());

            //for next iteration
            selectedFace = newFace.get();
            currEdge = tempEdge;

            //add to list widgets
            ui->facesListWidget->addItem(newFace.get());
            ui->halfEdgesListWidget->addItem(newEdge.get());
            ui->halfEdgesListWidget->addItem(newSymEdge.get());
            //adding new objects to the mesh
            ui->mygl->m_mesh.halfedgeCollection.push_back(std::move(newEdge));
            ui->mygl->m_mesh.halfedgeCollection.push_back(std::move(newSymEdge));
            ui->mygl->m_mesh.faceCollection.push_back(std::move(newFace));

        }
        //end while
        ui->mygl->m_mesh.create();
        ui->mygl->update();
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


