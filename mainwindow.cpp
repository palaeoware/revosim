#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDockWidget>
#include <QProgressDialog>
#include <QColorDialog>
#include <QStandardPaths>

/*
 *
 *
 */

MainWindow *MainWin;
randoms *simulation_randoms;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
   ui->setupUi(this);
   this->setWindowTitle("REvoSim Environmental Generator");
   setWindowIcon(QIcon (":/icon.png"));
   showMaximized();

   //RJG - Globals for simulation
   generations=500;
   currentGeneration=0;
   save=false;
   stackOneSize=0;
   stackTwoSize=0;

   Directory.setPath(QStandardPaths::writableLocation(QStandardPaths::DesktopLocation));
   ui->path->setText(Directory.path());

   MainWin=this;

   env_image=new QImage(":/palaeoware_logo_square.png");

   env_item= new QGraphicsPixmapItem(QPixmap::fromImage(*env_image));

   envscene = new EnvironmentScene;
   envscene->mw=this;
   envscene->addItem(env_item);

   ui->enviroView->setScene(envscene);
   QBrush brush(QColor(42,42,42));
   ui->enviroView->setBackgroundBrush(brush);
   //ui->enviroView->setAlignment(Qt::AlignCenter);
   ui->enviroView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
   ui->enviroView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

   ui->pushButtonStackTwo->setEnabled(false);
   ui->combineEnd->setEnabled(false);

   //RJG - Populate labels in GUI as required
   ui->envSettings->setWindowTitle("Environmental Settings");

   ui->russell_label->setAlignment(Qt::AlignJustify);
   ui->russell_label->setWordWrap(true);
   ui->russell_label->setText("Please enter the settings for the dynamic environment below. Most are obvious. Convergence sets the amount of smoothing between spots and background. 0.01 is very smooth, but will initially take about a minute to create environment.");

   ui->combine_label->setAlignment(Qt::AlignJustify);
   ui->combine_label->setWordWrap(true);
   ui->combine_label->setText("Combines stack two with stack one from  Start Slice to End Slice. Percentage start and end for influence of stack one, interpolate between. Limit for End Slice is end of stack one. If number of generations is larger than this it will duplicate stack two. If start and end slice are maximum it will concatenate two stacks.");

   ui->settings_tab_widget->setCurrentIndex(0);

   //RJG - Sort GUI
   startButton = new QAction(QIcon(QPixmap(":/darkstyle/icon_play_button_green.png")), QString("Run"), this);
   startButton->setEnabled(true);
   startButton->setToolTip(tr("<font>Use this button to generate and environment.</font>"));
   ui->toolBar->setIconSize(QSize(25,25));
   ui->toolBar->addAction(startButton);
   QObject::connect(startButton, SIGNAL(triggered()), this, SLOT(generateEnvironment()));

   //RJG - Load random numbers
   simulation_randoms = new randoms();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::newEnvironmentImage()
{
    //RJG - Add images to the scenes
    if(!env_image->isNull())delete env_image;
    env_image=new QImage(MainWin->ui->spinSize->value(), MainWin->ui->spinSize->value(), QImage::Format_RGB32);
    env_image->fill(0);
    env_item->setPixmap(QPixmap::fromImage(*env_image));
}

void MainWindow::generateEnvironment()
{
    if(!Directory.exists()&&save){QMessageBox::warning(0,"Error","No such directory.", QMessageBox::Ok);return;}

    newEnvironmentImage();

    //ui->enviroView->resetMatrix();
    //ui->enviroView->resetTransform();

    if (ui->settings_tab_widget->currentIndex()==0) //russellenv
        environmentobject = new russellenvironment;

    if (ui->settings_tab_widget->currentIndex()==1) //markenv
        environmentobject = new markenvironment;

    if (ui->settings_tab_widget->currentIndex()==2) //noise
    {
        environmentobject = new noiseenvironment;
        if(MainWin->ui->noiseMin->value()>=MainWin->ui->noiseMax->value())
        {
        QMessageBox::warning(0,"Error","Min is greater than Max - fix.", QMessageBox::Ok);
        return;
        }
    }

    if (ui->settings_tab_widget->currentIndex()==3) //combine
        environmentobject = new combine;

    if (ui->settings_tab_widget->currentIndex()==4) //colour
        environmentobject = new colour;

    if (ui->settings_tab_widget->currentIndex()==5) //stack
        environmentobject = new makestack;

    generations=MainWin->ui->numGenerations->value();

    QProgressDialog prDialogue("Progress of generation","Cancel",0,generations);
    prDialogue.show();

    for(int i=0;i<generations;i++)
    {
        currentGeneration=i;
        environmentobject->regenerate();
        prDialogue.setValue(i);

        RefreshEnvironment();

        qApp->processEvents();
        if(save)
        {
            QImage saveImage(MainWin->ui->spinSize->value(),MainWin->ui->spinSize->value(),QImage::Format_RGB32);
            for (int n=0; n<MainWin->ui->spinSize->value(); n++)
                for (int m=0; m<MainWin->ui->spinSize->value(); m++)
                    saveImage.setPixel(n,m,qRgb(environmentobject->environment[n][m][0], environmentobject->environment[n][m][1], environmentobject->environment[n][m][2]));
            QString dir2;
            //TODO - update this to be sensible
            if(i<10)dir2 = QString(Directory.path() + "/000%1.bmp").arg(i);
            if(i>9&&i<100)dir2 = QString(Directory.path() + "/00%1.bmp").arg(i);
            if(i>99&&i<1000)dir2 = QString(Directory.path() + "/0%1.bmp").arg(i);
            if(i>999)dir2 = QString(Directory.path() + "/%1.bmp").arg(i);
            saveImage.save(dir2);
        }

        if (prDialogue.wasCanceled()) break;

    }

    delete environmentobject;
}

void MainWindow::RefreshEnvironment()
{
    for (int n=0; n<MainWin->ui->spinSize->value(); n++)
        for (int m=0; m<MainWin->ui->spinSize->value(); m++)
           env_image->setPixel(n,m,qRgb(environmentobject->environment[n][m][0], environmentobject->environment[n][m][1], environmentobject->environment[n][m][2]));

    env_item->setPixmap(QPixmap::fromImage(*env_image));

    ui->enviroView->resetMatrix();
    if (ui->actionResize_environment->isChecked()) ui->enviroView->fitInView(env_item,Qt::KeepAspectRatio);
    float middle = (float)(ui->spinSize->value())/2.;
    ui->enviroView->centerOn(0.,0.);
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

void MainWindow::on_checkBox_2_toggled(bool checked)
{
    save=checked;
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
