/**
 * @file
 * Source: Enivronment class
 *
 * All REvoSim code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All REvoSim code is Copyright 2008-2019 by Mark Sutton, Russell Garwood,
 * and Alan R.T. Spencer.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version. This program is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY.
 */

#include "mainwindow.h"
#include "environmentclass.h"
#include "ui_mainwindow.h"

EnvironmentClass::EnvironmentClass()
{

    for (int n = 0; n < MainWin->ui->spinSize->value(); n++)
        for (int m = 0; m < MainWin->ui->spinSize->value(); m++)
        {

            environment[n][m][2] = (quint8)200;
            environment[n][m][1] = (quint8)110;
            environment[n][m][0] = (quint8)0;

        }
    saveMe = false;
}

EnvironmentClass::~EnvironmentClass()
{

}


void EnvironmentClass::regenerate()
{

}

void EnvironmentClass::path(QString files_directory, bool doSave)
{
    dir = files_directory;
    saveMe = doSave;
}

void EnvironmentClass::save(int generations)
{
    if (dir == "")
    {
        QMessageBox::warning(0, "Error", "No directory. Please select to save once images once more", QMessageBox::Ok);
        return;
    }
    QImage saveImage(MainWin->ui->spinSize->value(), MainWin->ui->spinSize->value(), QImage::Format_RGB32);
    for (int n = 0; n < MainWin->ui->spinSize->value(); n++)
        for (int m = 0; m < MainWin->ui->spinSize->value(); m++)
            saveImage.setPixel(n, m, qRgb(environment[n][m][0], environment[n][m][1], environment[n][m][2]));
    QString dir2 = QString(dir + "/%1.bmp").arg(generations);
    saveImage.save(dir2);
}
