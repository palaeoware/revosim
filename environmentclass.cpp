#include "mainwindow.h"
#include "environmentclass.h"
#include "ui_mainwindow.h"
#include <QDebug>

environmentclass::environmentclass()
{

    for (int n=0; n<MainWin->ui->spinSize->value(); n++)
    for (int m=0; m<MainWin->ui->spinSize->value(); m++)
    {

        environment[n][m][2]=(quint8)200;
        environment[n][m][1]=(quint8)110;
        environment[n][m][0]=(quint8)0;

    }
   saveMe=false;
}

environmentclass::~environmentclass()
{

}


void environmentclass::regenerate()
{

}

void environmentclass::path(QString files_directory, bool doSave)
{
dir=files_directory;
saveMe=doSave;
}

void environmentclass::save(int generations)
{
if(dir==""){QMessageBox::warning(0,"Error","No directory. Please select to save once images once more", QMessageBox::Ok);return;}
QImage saveImage(MainWin->ui->spinSize->value(),MainWin->ui->spinSize->value(),QImage::Format_RGB32);
for (int n=0; n<MainWin->ui->spinSize->value(); n++)
    for (int m=0; m<MainWin->ui->spinSize->value(); m++)
        saveImage.setPixel(n,m,qRgb(environment[n][m][0], environment[n][m][1], environment[n][m][2]));
QString dir2 = QString(dir + "/%1.bmp").arg(generations);
saveImage.save(dir2);
}

quint8 environmentclass::Rand8()
{
    //Random 8bit number
    return simulation_randoms->gen_Rand8();
}

