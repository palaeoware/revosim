#include <QFile>
#include <QApplication>
#include <QDebug>
#include "environmentclass.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "colour.h"

colour::colour()
{
    readSettings();
}


void colour::regenerate()
{
 readSettings();
 for (int n=0; n<MainWin->ui->spinSize->value(); n++)
     for (int m=0; m<MainWin->ui->spinSize->value(); m++)
       {
           environment[n][m][0]=colourRed;
           environment[n][m][1]=colourGreen;
           environment[n][m][2]=colourBlue;
        }
}


void colour::readSettings()
{
    colourRed=MainWin->ui->spinRed->value();
    colourGreen=MainWin->ui->spinGreen->value();
    colourBlue=MainWin->ui->spinBlue->value();
}
