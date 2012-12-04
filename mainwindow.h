#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsItem>
#include <QPixmap>
#include <QString>
#include <QDir>
#include <QFileDialog>
#include <QStatusBar>
#include "environmentclass.h"
#include "environmentscene.h"

#define GRID_X 256
#define GRID_Y 256

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
    void on_generateEnvironment_clicked();

    void on_numSeed_valueChanged(int arg1);

    void on_sMax_valueChanged(int arg1);

    void on_sMin_valueChanged(int arg1);

    void on_velMax2_valueChanged(double arg1);

    void on_cvelMax_valueChanged(int arg1);

    void on_buffSpin_valueChanged(int arg1);

    void on_periodic_clicked(bool checked);

    void on_blurChk_clicked(bool checked);

    void on_numGenerations_valueChanged(int arg1);

    void on_convergeAt_valueChanged(double arg1);

    void on_factorSpin_valueChanged(double arg1);

    void on_pushButton_clicked();

    void on_checkBox_2_toggled(bool checked);


private:

    QDockWidget *envOptions;
    environmentclass *environmentobject;
    EnvironmentScene *envscene;
    QDir Directory;
    QGraphicsPixmapItem *env_item;
    QImage *env_image;
    int generations;
    bool save;
    Ui::MainWindow *ui;

    void RefreshEnvironment();
};

#endif // MAINWINDOW_H

