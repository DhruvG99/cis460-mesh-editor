#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <mesh.h>
#include <QMainWindow>


namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void addToListWidgets();
    void resetLists();
    void smoothHE(HalfEdge*, glm::vec4);

    void on_pushButtonObj();
    void on_pushButtonSplit();
    void on_pushButtonTriangulate();
    void on_pushButtonCatmullClark();

    void on_actionQuit_triggered();

    void slot_changeVertexPos();
    void slot_changeFaceCol();

    void slot_selectVert(QListWidgetItem*);
    void slot_selectEdge(QListWidgetItem*);
    void slot_selectFace(QListWidgetItem*);

    void on_actionCamera_Controls_triggered();



private:
    Ui::MainWindow *ui;
};


#endif // MAINWINDOW_H
