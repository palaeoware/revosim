#include <QFile>
#include <QApplication>
#include <QDebug>
#include "environmentclass.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "makestack.h"

makestack::makestack()
{
   readSettings();
}

void makestack::regenerate()
{
readSettings();
if(filename.length()<5)return;
QImage original;
original.load(filename);

for (int n=0; n<MainWin->ui->spinSize->value(); n++)
    for (int m=0; m<MainWin->ui->spinSize->value(); m++)
      {
        QColor nmPixel = original.pixel(n,m);
        environment[n][m][0]=nmPixel.red();
        environment[n][m][1]=nmPixel.green();
        environment[n][m][2]=nmPixel.blue();
      }
}

void makestack::readSettings()
{
    filename=MainWin->ui->sFromImTxt->toPlainText();
}
