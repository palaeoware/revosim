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
#include "about.h"
#include "version.h"

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
    void on_pushButtonStackOne_clicked();
    void on_pushButtonStackTwo_clicked();
    void on_selectColour_clicked();
    void on_sFromImPushbutton_clicked();
    void about();
    void stop();
    void pause();
    void settings();
    void change_path();

private:

    QDockWidget *envOptions;
    environmentclass *environmentobject;
    EnvironmentScene *envscene;
    QDir Directory;
    QGraphicsPixmapItem *env_item;
    QImage *env_image;
    int generations, stackOneSize, stackTwoSize;
    QAction *startButton, *stopButton, *aboutButton, *pauseButton, *settingsButton;
    bool stop_flag, pause_flag;

    void RefreshEnvironment();
    void newEnvironmentImage();
    void reset_gui();
};

extern MainWindow *MainWin;
#endif // MAINWINDOW_H

