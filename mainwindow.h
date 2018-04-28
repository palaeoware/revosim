#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsItem>
#include <QPixmap>
#include <QString>
#include <QDir>
#include <QFileDialog>
#include <QStatusBar>
#include <QColor>
#include <QDebug>

#include "environmentclass.h"
#include "environmentscene.h"
#include "markenvironment.h"
#include "noiseenvironment.h"
#include "combine.h"
#include "colour.h"
#include "makestack.h"
#include "randoms.h"
#include "russellenvironment.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    Ui::MainWindow *ui;
    int currentGeneration;

private slots:
    void generateEnvironment();
    void on_pushButton_clicked();
    void on_checkBox_2_toggled(bool checked);
    void on_pushButtonStackOne_clicked();
    void on_pushButtonStackTwo_clicked();
    void combineStart_valueChanged();
    void on_selectColour_clicked();
    void on_sFromImPushbutton_clicked();

private:

    QDockWidget *envOptions;
    environmentclass *environmentobject;
    EnvironmentScene *envscene;
    QDir Directory;
    QGraphicsPixmapItem *env_item;
    QImage *env_image;
    int generations, stackOneSize, stackTwoSize;
    bool save;
    QAction *startButton;

    void combinelimits(int startPoint, int stkOne, int stkTwo);
    void RefreshEnvironment();
    void newEnvironmentImage();
};

extern MainWindow *MainWin;
#endif // MAINWINDOW_H

