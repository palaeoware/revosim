#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDockWidget>
#include <QDebug>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
   ui->setupUi(this);
   this->setWindowTitle("EvoSim Environmental Generator");
   ui->envSettings->setWindowTitle("Environtmental Settings");

   generations=500;
   save=false;
   environmentobject = new russellenvironment;
   environmentobject->initialiseSeeds(4);
   Directory.setPath("C:/tmp/");

   envscene = new EnvironmentScene;
   ui->enviroView->setScene(envscene);
   envscene->mw=this;

   //Add images to the scenes
   env_item= new QGraphicsPixmapItem();
   envscene->addItem(env_item);

   env_image=new QImage(GRID_X, GRID_Y, QImage::Format_RGB32);
   env_image->fill(0);

   env_item->setPixmap(QPixmap::fromImage(*env_image));

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_generateEnvironment_clicked()
{
    if(!Directory.exists()&&save){QMessageBox::warning(0,"Error","No such directory.", QMessageBox::Ok);return;}


    for(int i=0;i<generations;i++)
    {


        environmentobject->regenerate();

        RefreshEnvironment();
        qApp->processEvents();

        if(save)
        {
            QImage saveImage(GRID_X,GRID_Y,QImage::Format_RGB32);
            for (int n=0; n<GRID_X; n++)
                for (int m=0; m<GRID_Y; m++)
                    saveImage.setPixel(n,m,qRgb(environmentobject->environment[n][m][0], environmentobject->environment[n][m][1], environmentobject->environment[n][m][2]));
            QString dir2;
            if(i<10)dir2 = QString(Directory.path() + "/000%1.bmp").arg(i);
            if(i>9&&i<100)dir2 = QString(Directory.path() + "/00%1.bmp").arg(i);
            if(i>99&&i<1000)dir2 = QString(Directory.path() + "/0%1.bmp").arg(i);
            if(i>999)dir2 = QString(Directory.path() + "/%1.bmp").arg(i);
            saveImage.save(dir2);
        }

    }
}

void MainWindow::RefreshEnvironment()
{

    for (int n=0; n<GRID_X; n++)
    for (int m=0; m<GRID_Y; m++)
            env_image->setPixel(n,m,qRgb(environmentobject->environment[n][m][0], environmentobject->environment[n][m][1], environmentobject->environment[n][m][2]));

    if (ui->actionResize_environment->isChecked())
    {
         env_item->setPixmap(QPixmap::fromImage(*env_image));
         ui->enviroView->fitInView(env_item,Qt::KeepAspectRatio);
    }
    else
    {
        env_item->setPixmap(QPixmap::fromImage(*env_image));
        ui->enviroView->resetMatrix();
    }

}


void MainWindow::on_buffSpin_valueChanged(int arg1)
{
   environmentobject->buff_change(arg1);
}

void MainWindow::on_numSeed_valueChanged(int arg1)
{
   environmentobject->nseed_change(arg1);
}

void MainWindow::on_sMax_valueChanged(int arg1)
{
    environmentobject->maxsize_change(arg1);
}

void MainWindow::on_sMin_valueChanged(int arg1)
{
    environmentobject->sizevel_change(arg1);
}

void MainWindow::on_velMax2_valueChanged(double arg1)
{
    environmentobject->maxvel_change(arg1);
}

void MainWindow::on_cvelMax_valueChanged(int arg1)
{
    environmentobject->maxcvel_change(arg1);
}

void MainWindow::on_periodic_clicked(bool checked)
{
    environmentobject->periodic_change(checked);
}

void MainWindow::on_blurChk_clicked(bool checked)
{
    environmentobject->blur_change(checked);
}

void MainWindow::on_numGenerations_valueChanged(int arg1)
{
    generations=arg1;
}

void MainWindow::on_convergeAt_valueChanged(double arg1)
{
    environmentobject->converge_change(arg1);
}

void MainWindow::on_factorSpin_valueChanged(double arg1)
{
    environmentobject->fact_change(arg1);

}

/*void MainWindow::statBar(double arg1)
{
    QString prog = QString("Residual is currently %1 ").arg(arg1);
    statusBar()->showMessage(prog);
}*/

void MainWindow::on_pushButton_clicked()
{
    QString files_directory = QFileDialog::getExistingDirectory(this, tr("Select folder containing image files"),
    "d:/", QFileDialog::ShowDirsOnly);
    if (files_directory=="") return;
    else Directory=files_directory;
    ui->textBrowser_2->setText(Directory.path());
}

void MainWindow::on_checkBox_2_toggled(bool checked)
{
    save=checked;
}

