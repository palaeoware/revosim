#include "environmentclass.h"
#include "mainwindow.h"
#include <QDebug>


environmentclass::environmentclass()
{


    for (int n=0; n<GRID_X; n++)
    for (int m=0; m<GRID_Y; m++)
    {

        environment[n][m][2]=(quint8)200;
        environment[n][m][1]=(quint8)110;
        environment[n][m][0]=(quint8)0;

            /*if (n>70) r=30; else {if (n<40) r=90; else r=210;}
            environment[n][m][2]=(quint8)r;
            environment[n][m][1]=(quint8)r/2;
            environment[n][m][0]=(quint8)r/3;*/
    }

   saveMe=false;
}

void environmentclass::regenerate()
{

}

void environmentclass::settings()
{
QMessageBox::warning(0,"Error","No settings implemented for static environment", QMessageBox::Ok);
}

//This is a really shitty way of getting access functions to work, but can't be arsed to look at polymorphism again right now. Fix... Sometime.
void environmentclass::nseed_change(int value){}
void environmentclass::maxsize_change(int value){}
void environmentclass::sizevel_change(int value){}
void environmentclass::maxvel_change(int value){}
void environmentclass::maxcvel_change(int value){}
void environmentclass::periodic_change(bool value){}
void environmentclass::converge_change(double value){}
void environmentclass::numbGenerations_change(int value){}
void environmentclass::initialiseSeeds(int number){}

bool environmentclass::accessSave(){return saveMe;}

void environmentclass::path(QString files_directory, bool doSave)
{
dir=files_directory;
saveMe=doSave;
}

void environmentclass::save(int generations)
{

if(dir==""){QMessageBox::warning(0,"Error","No directory. Please select to save once images once more", QMessageBox::Ok);return;}
QImage saveImage(GRID_X,GRID_Y,QImage::Format_RGB32);
for (int n=0; n<GRID_X; n++)
    for (int m=0; m<GRID_Y; m++)
        saveImage.setPixel(n,m,qRgb(environment[n][m][0], environment[n][m][1], environment[n][m][2]));
QString dir2 = QString(dir + "/%1.bmp").arg(generations);
saveImage.save(dir2);
}

