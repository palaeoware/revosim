#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDockWidget>
#include <QProgressDialog>
#include <QProgressBar>
#include <QColorDialog>
#include <QStandardPaths>
#include <QShortcut>

/*
 * To do:
 *
 * Test with different image sizes
 */

MainWindow *MainWin;
randoms *simulation_randoms;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
   ui->setupUi(this);
   QString version;
   version.sprintf("%d.%d.%d",MAJORVERSION,MINORVERSION,PATCHVERSION);
   this->setWindowTitle(QString(PRODUCTNAME)+" v"+version+" - compiled - "+__DATE__);
   setWindowIcon(QIcon (":/icon.png"));
   showMaximized();

   //RJG - Globals for simulation
   generations=500;
   currentGeneration=0;
   stackOneSize=0;
   stackTwoSize=0;
   stop_flag=false;
   pause_flag=false;
   Directory.setPath(QStandardPaths::writableLocation(QStandardPaths::DesktopLocation));
   ui->path->setText(Directory.path());

   MainWin=this;

   env_image=new QImage(":/palaeoware_logo_square.png");

   env_item= new QGraphicsPixmapItem(QPixmap::fromImage(*env_image));

   envscene = new EnvironmentScene;
   envscene->mw=this;
   envscene->addItem(env_item);

   ui->enviroView->setScene(envscene);

   ui->pushButtonStackTwo->setEnabled(false);
   ui->combineEnd->setEnabled(false);

   //RJG - Populate labels in GUI as required
   ui->envSettings->setWindowTitle("Environmental Settings");
   ui->display_label->setStyleSheet("font-weight: bold");
   ui->output_label->setStyleSheet("font-weight: bold");

   ui->russell_label->setAlignment(Qt::AlignJustify);
   ui->russell_label->setWordWrap(true);
   ui->russell_label->setText("Please enter the settings for the dynamic environment below. Most are obvious. Convergence sets the amount of smoothing between spots and background. 0.01 is very smooth, but will initially take about a minute to create environment.");

   ui->m_environment_label->setAlignment(Qt::AlignJustify);
   ui->m_environment_label->setWordWrap(true);
   ui->m_environment_label->setText("Please enter the settings for the dynamic environment below. Most are obvious.");

   ui->noise_label->setAlignment(Qt::AlignJustify);
   ui->noise_label->setWordWrap(true);
   ui->noise_label->setText("This will provide a stack of noise images, which could be of utility for combining with other kinds of image stacks.");

   //stack_label

   ui->combine_label->setAlignment(Qt::AlignJustify);
   ui->combine_label->setWordWrap(true);
   ui->combine_label->setText("Combines stack two with stack one from  Start Slice to End Slice. Percentage start and end for influence of stack one, interpolate between. Limit for End Slice is end of stack one. If number of generations is larger than this it will duplicate stack two. If start and end slice are maximum it will concatenate two stacks.");

   ui->instructions_label->setAlignment(Qt::AlignJustify);
   ui->instructions_label->setWordWrap(true);
   ui->instructions_label->setText("Select output options here, and then one of the tabs to the right in order to define what type of environment is generated when you press play.");


   //RJG - Sort GUI
   ui->toolBar->setIconSize(QSize(25,25));
   ui->statusBar->setMinimumHeight(25);
   ui->settings_tab_widget->setCurrentIndex(0);

   startButton = new QAction(QIcon(QPixmap(":/darkstyle/icon_play_button_green.png")), QString("Run"), this);
   startButton->setToolTip(tr("<font>Use this button to generate an environment.</font>"));
   ui->toolBar->addAction(startButton);
   QObject::connect(startButton, SIGNAL(triggered()), this, SLOT(generateEnvironment()));

   stopButton = new QAction(QIcon(QPixmap(":/darkstyle/icon_stop_button_red.png")), QString("Stop"), this);
   stopButton->setToolTip(tr("<font>Use this button stop the environmental generation.</font>"));
   ui->toolBar->addAction(stopButton);
   QObject::connect(stopButton, SIGNAL (triggered()), this, SLOT (stop()));
   stopButton->setEnabled(false);

   pauseButton = new QAction(QIcon(QPixmap(":/darkstyle/icon_pause_button_orange.png")), QString("Pause"), this);
   pauseButton->setToolTip(tr("<font>Use this button to pause the environmental generation. Press again to unpause.</font>"));
   ui->toolBar->addAction(pauseButton);
   QObject::connect(pauseButton, SIGNAL (triggered()), this, SLOT (pause()));
   pauseButton->setEnabled(false);

    ui->toolBar->addSeparator();
   settingsButton = new QAction(QIcon(QPixmap(":/darkstyle/icon_settings_1_button.png")), QString("Settings"), this);
   settingsButton->setToolTip(tr("<font>Use this button to open the Settings Dock.</font>"));
   ui->toolBar->addAction(settingsButton);
   QObject::connect(settingsButton, SIGNAL (triggered()), this, SLOT (settings()));
   settingsButton->setCheckable(true);
   settingsButton->setChecked(true);

   ui->toolBar->addSeparator();
   aboutButton = new QAction(QIcon(QPixmap(":/darkstyle/icon_about_button.png")), QString("About"), this);
   aboutButton->setToolTip(tr("<font>Use this button to view information about this program.</font>"));
   ui->toolBar->addAction(aboutButton);
   QObject::connect(aboutButton, SIGNAL (triggered()), this, SLOT (about()));

   //RJG - Load random numbers
   simulation_randoms = new randoms();

   //RJG - shortcuts
   new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_R), this, SLOT(generateEnvironment()));
   new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_P), this, SLOT(pause()));
   new QShortcut(QKeySequence(Qt::Key_Escape), this, SLOT(stop()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::generateEnvironment()
{
    if(!Directory.exists() && ui->save_images_checkbox->isChecked()){QMessageBox::warning(0,"Error","No such directory.", QMessageBox::Ok);return;}
    if (ui->settings_tab_widget->currentIndex()==0){QMessageBox::warning(0,"Nope","Select one of the tabs to the right to tell the software what kind of environment you would like to generate.", QMessageBox::Ok);return;}
    ui->output_tab->setEnabled(false);

    newEnvironmentImage();
    stop_flag=false;
    startButton->setEnabled(false);
    pauseButton->setEnabled(true);
    stopButton->setEnabled(true);

    if (ui->settings_tab_widget->currentIndex()==1) //russellenv
        environmentobject = new russellenvironment;

    if (ui->settings_tab_widget->currentIndex()==2) //markenv
        environmentobject = new markenvironment;

    if (ui->settings_tab_widget->currentIndex()==3) //noise
    {
        environmentobject = new noiseenvironment;
        if(MainWin->ui->noiseMin->value()>=MainWin->ui->noiseMax->value())
        {
        QMessageBox::warning(0,"Error","Min is greater than Max - fix.", QMessageBox::Ok);
        return;
        }
    }

    if (ui->settings_tab_widget->currentIndex()==4) //combine
        environmentobject = new combine;

    if (ui->settings_tab_widget->currentIndex()==5) //colour
        environmentobject = new colour;

    if (ui->settings_tab_widget->currentIndex()==6) //stack
        environmentobject = new makestack;

    generations=MainWin->ui->numGenerations->value();

    //RJG - add progress bar
    QProgressBar prBar;
    prBar.setRange(0,generations);
    prBar.setAlignment(Qt::AlignCenter);
    ui->statusBar->addPermanentWidget(&prBar);

    for(int i=0;i<generations;i++)
    {

        currentGeneration=i;
        environmentobject->regenerate();

        prBar.setValue(i);
        RefreshEnvironment();

        qApp->processEvents();

        if(ui->save_images_checkbox->isChecked())
        {
            QImage saveImage(MainWin->ui->spinSize->value(),MainWin->ui->spinSize->value(),QImage::Format_RGB32);
            for (int n=0; n<MainWin->ui->spinSize->value(); n++)
                for (int m=0; m<MainWin->ui->spinSize->value(); m++)
                    saveImage.setPixel(n,m,qRgb(environmentobject->environment[n][m][0], environmentobject->environment[n][m][1], environmentobject->environment[n][m][2]));
            QString save_directory=QString(Directory.path()+"/%1.png").arg(i, 4, 10, QChar('0'));
            saveImage.save(save_directory);
        }

        if(pause_flag)
            while(pause_flag && !stop_flag)
                qApp->processEvents();

        if(stop_flag){stop_flag=false; generations=-1; break;}

    }

    ui->statusBar->removeWidget(&prBar);
    if (generations<0)ui->statusBar->showMessage("Generation cancelled.");
    else if (ui->save_images_checkbox->isChecked())ui->statusBar->showMessage(QString("Generation complete: %1 images were saved.").arg(generations));
    else ui->statusBar->showMessage("Generation complete; no images saved as save is not selected in the out tab.");

    reset_gui();
    delete environmentobject;
}

void MainWindow::newEnvironmentImage()
{
    //RJG - Add images to the scenes
    if(!env_image->isNull())delete env_image;
    env_image=new QImage(MainWin->ui->spinSize->value(), MainWin->ui->spinSize->value(), QImage::Format_RGB32);
    env_image->fill(0);
    env_item->setPixmap(QPixmap::fromImage(*env_image));
    //RJG - flag secene for deletion later, and then create a new one
    //If you don't do this the scene is too large, and doesn't center correctly.
    envscene->deleteLater();
    envscene = new EnvironmentScene;
    envscene->addItem(env_item);
    ui->enviroView->setScene(envscene);
}


void MainWindow::RefreshEnvironment()
{
    for (int n=0; n<MainWin->ui->spinSize->value(); n++)
        for (int m=0; m<MainWin->ui->spinSize->value(); m++)
           env_image->setPixel(n,m,qRgb(environmentobject->environment[n][m][0], environmentobject->environment[n][m][1], environmentobject->environment[n][m][2]));

    env_item->setPixmap(QPixmap::fromImage(*env_image));

    ui->enviroView->resetMatrix();
    if (ui->resize->isChecked()) ui->enviroView->fitInView(env_item,Qt::KeepAspectRatio);
    ui->enviroView->centerOn(0.,0.);
}


void MainWindow::reset_gui()
{

    //RJG - Add images to the scenes
    if(!env_image->isNull())delete env_image;
    env_image=new QImage(":/palaeoware_logo_square.png");
    env_item->setPixmap(QPixmap::fromImage(*env_image));

    envscene->deleteLater();

    envscene = new EnvironmentScene;
    envscene->mw=this;
    envscene->addItem(env_item);
    ui->enviroView->resetMatrix();
    ui->enviroView->setScene(envscene);

    //RJG - Sort flags
    pause_flag=false;
    stop_flag=false;

    //RJG - Sort buttons and GUI
    startButton->setEnabled(true);
    pauseButton->setEnabled(false);
    stopButton->setEnabled(false);
    ui->output_tab->setEnabled(true);
}


void MainWindow::on_pushButton_clicked()
{
    QString files_directory = QFileDialog::getExistingDirectory(this, tr("Select folder containing image files"),
    "d:/", QFileDialog::ShowDirsOnly);
    if (files_directory=="") return;
    else Directory=files_directory;
    ui->path->setText(Directory.path());
}

void MainWindow::on_pushButtonStackOne_clicked()
{
    QString files_directory = QFileDialog::getExistingDirectory(this, tr("Select folder containing image files"),
    "d:/", QFileDialog::ShowDirsOnly);

    QDir current = files_directory;
    QStringList filterList, dirList;
    filterList << "*.bmp" << "*.jpg" << "*.jpeg" << "*.png";
    dirList = current.entryList(filterList,QDir::Files, QDir::Name);
    if(dirList.count()==0)
           {
           QMessageBox::warning(0,"Error","No image files in this folder.", QMessageBox::Ok);
           return;
           }
    else ui->stackOneText->setText(files_directory);
    stackOneSize=dirList.count();

    combinelimits(MainWin->ui->combineStart->value(),stackOneSize,stackTwoSize);
    ui->pushButtonStackTwo->setEnabled(true);
}

void MainWindow::on_pushButtonStackTwo_clicked()
{
    QString files_directory = QFileDialog::getExistingDirectory(this, tr("Select folder containing image files"),
    "d:/", QFileDialog::ShowDirsOnly);

    QDir current = files_directory;
    QStringList filterList, dirList;
    filterList << "*.bmp" << "*.jpg" << "*.jpeg" << "*.png";
    dirList = current.entryList(filterList,QDir::Files, QDir::Name);
    if(dirList.count()==0)
           {
           QMessageBox::warning(0,"Error","No image files in this folder.", QMessageBox::Ok);
           return;
           }
    else ui->stackTwoText->setText(files_directory);

    stackTwoSize=dirList.count();

    combinelimits(MainWin->ui->combineStart->value(),stackOneSize,stackTwoSize);
    ui->combineEnd->setEnabled(true);
}

void MainWindow::combineStart_valueChanged()
{
    QString stackOneTextLength(MainWin->ui->stackTwoText->toPlainText());
    if (stackOneTextLength.length()>0)
        combinelimits(MainWin->ui->combineStart->value(),stackOneSize,stackTwoSize);
}

void MainWindow::combinelimits(int startPoint, int stkOne, int stkTwo)
{
    //Make limits of combine in combined environment sensible
    //Set current value to best guess (but values within sensible limits can be changed)
    int maxSize=startPoint+stkTwo;
    if(stkOne>maxSize) maxSize=stkOne;
    ui->numGenerations->setMaximum(maxSize);
    ui->numGenerations->setValue(maxSize);

    int minSize=startPoint+stkTwo;
    if(stkOne<minSize) minSize=stkOne;
    ui->combineEnd->setMaximum(minSize);
    ui->combineEnd->setValue(minSize);

    ui->combineStart->setMaximum(stkOne);
}

void MainWindow::on_selectColour_clicked()
{
    QColor mainWinColour(ui->spinRed->value(),ui->spinGreen->value(),ui->spinBlue->value());
    QColorDialog chooseColour;
    mainWinColour = chooseColour.getColor(mainWinColour);
    if (mainWinColour.isValid())
    {
        ui->spinRed->setValue(mainWinColour.red());
        ui->spinGreen->setValue(mainWinColour.green());
        ui->spinBlue->setValue(mainWinColour.blue());
    }
}

void MainWindow::on_sFromImPushbutton_clicked()
{
    QString file_name = QFileDialog::getOpenFileName(this, tr("Select image file"),
                                                    "d:/",
                                                    tr("Images (*.png *.bmp *.jpg)"));
    if(!file_name.isNull())
        ui->sFromImTxt->setText(file_name);

    if(file_name.length()<5)return;
    QImage original;
    original.load(file_name);

    ui->spinSize->setValue(original.width());
}

void MainWindow::about()
{
    About adialogue;
    adialogue.exec();
}

void MainWindow::stop()
{
    stop_flag=true;
}

void MainWindow::pause()
{
    pause_flag=!pause_flag;
}


void MainWindow::settings()
{
    if(ui->settings_tab_widget->isVisible())
        {
            ui->envSettings->hide();
            settingsButton->setChecked(false);
        }
    else
        {
            ui->envSettings->show();
            settingsButton->setChecked(true);
        }
}
