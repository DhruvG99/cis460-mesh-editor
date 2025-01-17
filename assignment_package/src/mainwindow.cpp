#include "mainwindow.h"
#include <ui_mainwindow.h>
#include "cameracontrolshelp.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonArray>
#include <unordered_set>

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
    connect(ui->pushButton_4, SIGNAL(clicked(bool)),
            this, SLOT(on_pushButtonCatmullClark()));
    connect(ui->pushButton_5, SIGNAL(clicked(bool)),
            this, SLOT(on_pushButtonJson()));
    connect(ui->pushButton_6, SIGNAL(clicked(bool)),
            this, SLOT(on_pushButtonSkinMesh()));

    //little difficult to send additional parameters with signals
    //instead using three different slots.
    connect(ui->vertPosXSpinBox, SIGNAL(valueChanged(double)),
            this, SLOT(slot_changeVertexPos()));
    connect(ui->vertPosYSpinBox, SIGNAL(valueChanged(double)),
            this, SLOT(slot_changeVertexPos()));
    connect(ui->vertPosZSpinBox, SIGNAL(valueChanged(double)),
            this, SLOT(slot_changeVertexPos()));


    connect(ui->faceRedSpinBox, SIGNAL(valueChanged(double)),
            this, SLOT(slot_changeFaceCol()));
    connect(ui->faceGreenSpinBox, SIGNAL(valueChanged(double)),
            this, SLOT(slot_changeFaceCol()));
    connect(ui->faceBlueSpinBox, SIGNAL(valueChanged(double)),
            this, SLOT(slot_changeFaceCol()));

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

void MainWindow::addToJointTree(Joint* node)
{
    for(auto& child: node->children)
    {
        node->addChild(child.get());
        child->setText(0,child->name);
        addToJointTree(child.get());
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

void MainWindow::buildSkeleton(Joint* parent, QJsonObject obj)
{
    QString nodeName = obj["name"].toString();
    QJsonArray nodePos = obj["pos"].toArray();
    QJsonArray nodeRot = obj["rot"].toArray();
    QJsonArray nodeChildren = obj["children"].toArray();
    glm::vec3 pos (nodePos[0].toDouble(), nodePos[1].toDouble(), nodePos[2].toDouble());
    glm::vec4 rot (nodeRot[0].toDouble(), nodeRot[1].toDouble(), nodeRot[2].toDouble(), nodeRot[3].toDouble());
    //set to nullptr for root node
    uPtr<Joint> node = mkU<Joint>(nodeName, pos, rot, parent);
    //recursively add children
    for(int i = 0; i<nodeChildren.size(); i++)
        buildSkeleton(node.get(), nodeChildren[i].toObject());
    if(parent!=nullptr)
        parent->children.push_back(std::move(node));
    else
        ui->mygl->m_skeleton.root = std::move(node);
}

void MainWindow::testSkeleton(Joint* node)
{
    for(auto& child: node->children)
    {
        Joint* childPtr = child.get();
        std::cout << childPtr->name.toStdString() << std::endl;
        testSkeleton(childPtr);
    }
}

void MainWindow::on_pushButtonJson()
{
    QString filename = QFileDialog::getOpenFileName(0, QString("Load JSON"),
                                                    QDir::currentPath().append(QString("../..")),
                                                    QString("*.json"));
    QFile file = QFile(filename);
    if(!file.open(QIODevice::ReadOnly)){
            qWarning("Could not open the JSON file.");
            return;
    }
    QJsonDocument myjson(QJsonDocument::fromJson(file.readAll()));
    QJsonObject rootNode = myjson.object()["root"].toObject();
    buildSkeleton(nullptr, rootNode);
    Joint* root = ui->mygl->m_skeleton.root.get();
    ui->jointTree->addTopLevelItem(root);
    root->setText(0,root->name);
    for(auto& child: root->children)
    {
        root->addChild(child.get());
        child->setText(0,child->name);
        addToJointTree(child.get());
    }
    ui->mygl->m_skeleton.create();
    ui->mygl->isSkeletonCreated = true;
    ui->mygl->update();
    /*
     * To test the skeleton:
     * testSkeleton(ui->mygl->m_skeleton.root.get());
     */
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
        //to update selection
        ui->mygl->m_face.create();
        ui->mygl->m_mesh.create();
        ui->mygl->update();
    }
}

void MainWindow::smoothHE(HalfEdge* smoothEdge, glm::vec4 smoothPos)
{
    HalfEdge* symEdge = smoothEdge->getSym();
    HalfEdge* currEdge = smoothEdge;
    do
    {
        currEdge = currEdge->getNext();

    }while(currEdge->getNext() != smoothEdge);

    Vertex* prevV = currEdge->getVert();
    Vertex* nextV = smoothEdge->getVert();
    glm::vec4 newPos = smoothPos;

    uPtr<Vertex> newV = mkU<Vertex>(newPos);
    uPtr<HalfEdge> newEdge = mkU<HalfEdge>();
    uPtr<HalfEdge> newSymEdge = mkU<HalfEdge>();

    //setting next ptr for new edge
    newEdge->setNext(smoothEdge->getNext());
    //setting vertex for new edge
    newEdge->setVert(nextV);
    //setting face pointer for new edge
    newEdge->setFace(smoothEdge->getFace());
    //setting next ptr for old edge
    smoothEdge->setNext(newEdge.get());
    //setting new vertex for old edge
    smoothEdge->setVert(newV.get());

    //same operations as above, but for the symmetric edge
    newSymEdge->setNext(symEdge->getNext());
    newSymEdge->setVert(prevV);
    newSymEdge->setFace(symEdge->getFace());
    symEdge->setNext(newSymEdge.get());
    symEdge->setVert(newV.get());

    //setting new symmetric edges
    smoothEdge->setSym(newSymEdge.get());
    symEdge->setSym(newEdge.get());

    ui->vertsListWidget->addItem(newV.get());
    ui->halfEdgesListWidget->addItem(newEdge.get());
    ui->halfEdgesListWidget->addItem(newSymEdge.get());
    //adding new objects to the mesh
    ui->mygl->m_mesh.vertexCollection.push_back(std::move(newV));
    ui->mygl->m_mesh.halfedgeCollection.push_back(std::move(newEdge));
    ui->mygl->m_mesh.halfedgeCollection.push_back(std::move(newSymEdge));
    //removing create and update from here
}

void MainWindow::on_pushButtonCatmullClark()
{
    //STEP 2: Centroids
    std::unordered_map<Face*, glm::vec4> mapCentroid;
    for(auto& f: ui->mygl->m_mesh.faceCollection)
    {
        HalfEdge* faceEdge = f->getEdge();
        HalfEdge* currEdge = faceEdge;
        glm::vec4 centroidPos = glm::vec4(0.0f);
        int numVerts = 0;
        do
        {
            centroidPos += currEdge->getVert()->pos;
            numVerts++;
            currEdge = currEdge->getNext();
        } while(currEdge!=faceEdge);
        centroidPos /= numVerts;
        //add to map from face->centroid
        mapCentroid.insert(std::make_pair(f.get(),centroidPos));
    }

    //STEP 3: Getting the smooth mid points
    std::unordered_set<HalfEdge*> set_splitHE;
    int numHE = ui->mygl->m_mesh.halfedgeCollection.size();
    int numVert = ui->mygl->m_mesh.vertexCollection.size(); // used in step 4
    int numFace = ui->mygl->m_mesh.faceCollection.size();
    //this for loop ensures I don't iterate thru newly added split-edges
    for(int i = 0; i<numHE; i++)
    {
        HalfEdge* e = ui->mygl->m_mesh.halfedgeCollection[i].get();
        if(set_splitHE.find(e) == set_splitHE.end())
        {
            Face* edgeFace = e->getFace();
            Face* symEdgeFace = e->getSym()->getFace();
            glm::vec4 centroid1 = mapCentroid[edgeFace]; //use map from edgeFace->centroid
            glm::vec4 centroid2 = mapCentroid[symEdgeFace]; //use map from symEdgeFace->centroid
            glm::vec4 vert1 = e->getVert()->pos;
            glm::vec4 vert2 = e->getSym()->getVert()->pos;
            glm::vec4 smoothMidPos = 0.25f*(vert1 +
                                            vert2 +
                                            centroid1 +
                                            centroid2);
            //add indices before splitting
            //because sym changes after the splitting
            set_splitHE.insert(e);
            set_splitHE.insert(e->getSym());
            //call smooth/split method on half-edge
            smoothHE(e, smoothMidPos);
        }
    }

    //STEP 4: Smooth the original vertices
    for(int i = 0; i < numVert; i++)
    {
        Vertex* vert = ui->mygl->m_mesh.vertexCollection[i].get();
        glm::vec4 eSum = glm::vec4(0.0f);
        glm::vec4 fSum = glm::vec4(0.0f);
        float numAdj = 0.0f;
        HalfEdge* vertEdge = vert->halfedge->getSym();
        do
        {
            eSum += vertEdge->getVert()->pos;
            fSum += mapCentroid[vertEdge->getFace()];
            vertEdge = vertEdge->getSym()->getNext();
            numAdj++;
        }while(vertEdge != vert->halfedge->getSym());

        glm::vec4 newPos =
                (vert->pos*(numAdj-2.0f)/numAdj) +
                ((eSum+fSum)/(float)pow(numAdj,2));
        vert->setPos(newPos);
    }

    //STEP 5: QUADRANGULATIONNNNNN DUN DUN DUN
    //using a for loop to avoid repeat quading
    for(int i = 0; i< numFace; i++)
    {
        Face* f = ui->mygl->m_mesh.faceCollection[i].get();
        glm::vec4 centroidPos = mapCentroid[f];
        uPtr<Vertex> centroidVert = mkU<Vertex>(centroidPos);
        HalfEdge* faceEdge = f->getEdge();
        HalfEdge* currEdge = faceEdge;
        //starting at the last half of the edge
        HalfEdge* firstEdge = currEdge->getNext();
        HalfEdge* prevEdgeToVert = nullptr;
        HalfEdge* firstEdgeToVert = nullptr;
        do
        {
            uPtr<HalfEdge> edgeToVert = mkU<HalfEdge>();
            uPtr<HalfEdge> edgeFromVert = mkU<HalfEdge>();
            //new edge to centroid
            edgeToVert->setNext(edgeFromVert.get());
            edgeToVert->setVert(centroidVert.get());

            //new edge from centroid to different midpoint
            edgeFromVert->setVert(currEdge->getVert());
            edgeFromVert->setNext(firstEdge);

            //only create a new face if it's not the first quading
            if(prevEdgeToVert!=nullptr)
            {
                uPtr<Face> facePtr = mkU<Face>();
                //setting sym from previous face
                edgeFromVert->setSym(prevEdgeToVert);
                //set face pointers for all edges
                edgeToVert->setFace(facePtr.get());
                facePtr->setEdge(edgeToVert.get());
                edgeFromVert->setFace(facePtr.get());
                firstEdge->setFace(facePtr.get());
                firstEdge->getNext()->setFace(facePtr.get());
                ui->facesListWidget->addItem(facePtr.get());
                ui->mygl->m_mesh.faceCollection.push_back(std::move(facePtr));
            } else {
                firstEdgeToVert = edgeFromVert.get();
                //set face pointers for all edges
                edgeToVert->setFace(f);
                f->setEdge(edgeToVert.get());
                edgeFromVert->setFace(f);
                firstEdge->setFace(f);
                firstEdge->getNext()->setFace(f);
            }

            prevEdgeToVert = edgeToVert.get();
            currEdge = firstEdge->getNext();
            firstEdge = currEdge->getNext();
            //modifying next at the end
            currEdge->setNext(edgeToVert.get());

            //add to collections
            ui->halfEdgesListWidget->addItem(edgeToVert.get());
            ui->halfEdgesListWidget->addItem(edgeFromVert.get());
            ui->mygl->m_mesh.halfedgeCollection.push_back(std::move(edgeToVert));
            ui->mygl->m_mesh.halfedgeCollection.push_back(std::move(edgeFromVert));
        } while(currEdge != faceEdge);
        //setting last sym
        prevEdgeToVert->setSym(firstEdgeToVert);
        ui->vertsListWidget->addItem(centroidVert.get());
        ui->mygl->m_mesh.vertexCollection.push_back(std::move(centroidVert));
    }
    ui->mygl->m_mesh.create();
    ui->mygl->update();
}

//void MainWindow::jointInfluence(Joint* node)
//{
//    for(auto& vert: ui->mygl->m_mesh.vertexCollection)
//    {
//        vert->pos()
//    }
//    for(auto& child: node->children)
//    {
//        Joint* childPtr = child.get();

//        jointInfluence(childPtr);
//    }
//}

void MainWindow::on_pushButtonSkinMesh()
{
//    Joint* root = ui->mygl->m_skeleton.root.get();
//    for(auto& node: root->children)
//    {

//    }
    //for every joint in tree
    //calculate distance of all vertices from joint 1
    //assign distance as weight and jointID as influence
    //next iteration:
    //calculate distance of all vertices from joint 2
    //assign distance as weight and jointID as influence
    //next iteration:
    //calculate distance of all vertices from joint n
    //Pick minimum of (influence 1, influence 2, joint n)
    //assign distance_n as weight1/2 and joint_n as influence1/2
}


void MainWindow::slot_changeVertexPos()
{
    if(ui->mygl->isVertSelected)
    {
        double x = ui->vertPosXSpinBox->value();
        double y = ui->vertPosYSpinBox->value();
        double z = ui->vertPosZSpinBox->value();
        Vertex* selectedVert = ui->mygl->m_vert.getVertex();
        selectedVert->setPos(glm::vec4(x,y,z,1.0f));
        //to update any selections
        ui->mygl->m_vert.create();
        if(ui->mygl->isEdgeSelected)
            ui->mygl->m_edge.create();
        if(ui->mygl->isFaceSelected)
            ui->mygl->m_face.create();
        ui->mygl->m_mesh.create();
        ui->mygl->update();
    }
}

void MainWindow::slot_changeFaceCol()
{
    if(ui->mygl->isFaceSelected)
    {
        double r = ui->faceRedSpinBox->value();
        double g = ui->faceGreenSpinBox->value();
        double b = ui->faceBlueSpinBox->value();
        Face* selectedFace = ui->mygl->m_face.getFace();
        selectedFace->setColor(glm::vec4(r,g,b,0.0f));
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


