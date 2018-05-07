#include <QFile>
#include <QApplication>
#include <QDebug>
#include <QImage>
#include <QStringList>
#include "environmentclass.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "combine.h"

combine::combine()
{
    ReadSettings();
    if(!stackOne.exists()||!stackTwo.exists()){QMessageBox::warning(0,"Error","Either stack one or two has failed to load.", QMessageBox::Ok);error=true;return;}

    //Create new QImages to then check the relative size of stacks
    QStringList filterList, dirList, dirList2;
    filterList << "*.bmp" << "*.jpg" << "*.jpeg" << "*.png";

    dirList = stackOne.entryList(filterList,QDir::Files, QDir::Name);
    QImage stackOneImage(stackOne.absolutePath() + "/" + dirList[0]);

    dirList2 = stackTwo.entryList(filterList,QDir::Files, QDir::Name);
    QImage stackTwoImage(stackTwo.absolutePath() + "/" + dirList2[0]);

    if(stackOneImage.size()!=stackTwoImage.size()){QMessageBox::warning(0,"No dice","Stack images are or a different size, and "+QString(PRODUCTNAME)+" is not yet set up to deal with this. To request this feature, please email RJG (details in about window)", QMessageBox::Ok);error=true;}
}

void combine::ReadSettings()
{
    stackOne.setPath(MainWin->ui->stackOneText->toPlainText());
    stackTwo.setPath(MainWin->ui->stackTwoText->toPlainText());
    start=MainWin->ui->combineStart->value();
    currentGen=MainWin->currentGeneration;
    percentStart=MainWin->ui->percentStartSpin->value();
    percentEnd=MainWin->ui->percentEndSpin->value();
}

void combine::regenerate()
{
        ReadSettings();

        //Blank environment so it's obvious if something has gone wrong
        for (int n=0; n<MainWin->ui->spinSize->value(); n++)
           for (int m=0; m<MainWin->ui->spinSize->value(); m++)
                for (int i=0;i<3;i++)
                    environment[n][m][i]=0;

        //Create new QImages
        QStringList filterList, dirList, dirList2;
        filterList << "*.bmp" << "*.jpg" << "*.jpeg" << "*.png";

        dirList = stackOne.entryList(filterList,QDir::Files, QDir::Name);
        QImage stackOneImage;
        if(currentGen<dirList.count())
                    stackOneImage.load(stackOne.absolutePath() + "/" + dirList[currentGen]);

        end=dirList.length();

        dirList2 = stackTwo.entryList(filterList,QDir::Files, QDir::Name);
        QImage stackTwoImage;
        if(currentGen>=start && (currentGen-start)<dirList2.count())
                stackTwoImage.load(stackTwo.absolutePath() + "/" + dirList2[currentGen-start]);

        //Work out percentages for combine
        double currentPercent;
        int percDiff = percentEnd-percentStart;
        int currentPos = (currentGen-start);

        currentPercent = (double) percentStart + (((double)currentPos/(end-start-1))*percDiff);
        currentPercent /= 100;

        //The combine images - ensuring that when only one exists, the existing one is caried through
        //e.g. if stack two finishes after stack one and you want a transitioned combine from one to two
        //Also it means that the software doesn't crash if user does something unexpected
        if(stackOneImage.isNull() && stackTwoImage.isNull()) return;
        else if (stackOneImage.isNull())combineImages(stackTwoImage, 1., stackTwoImage);
        else if (stackTwoImage.isNull())combineImages(stackOneImage, 1., stackOneImage);
        else combineImages(stackOneImage, currentPercent, stackTwoImage);
}

void combine::combineImages(QImage sOne, double pOne, QImage sTwo)
{

    for (int n=0; n<MainWin->ui->spinSize->value(); n++)
       for (int m=0; m<MainWin->ui->spinSize->value(); m++)
       {
           QColor nmPixelOne = sOne.pixel(n,m);
           QColor nmPixelTwo = sTwo.pixel(n,m);
           environment[n][m][0]=(int)((double)nmPixelOne.red()*pOne)+((double)nmPixelTwo.red()*(1.-pOne));
           environment[n][m][1]=(int)((double)nmPixelOne.green()*pOne)+((double)nmPixelTwo.green()*(1.-pOne));
           environment[n][m][2]=(int)((double)nmPixelOne.blue()*pOne)+((double)nmPixelTwo.blue()*(1.-pOne));
        }

}
