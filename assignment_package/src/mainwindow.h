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

    void on_pushButtonObj();

    void on_actionQuit_triggered();

    void setSelected();

    void on_actionCamera_Controls_triggered();

private:
    Ui::MainWindow *ui;
};


#endif // MAINWINDOW_H
