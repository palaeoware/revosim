/**
 * @file
 * Source: MakeStack
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

#include <QFile>
#include <QApplication>
#include "environmentclass.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "makestack.h"

makestack::makestack()
{
    readSettings();
    if (filename.length() < 5)
    {
        QMessageBox::warning(0, "Error", "Image failed to load.", QMessageBox::Ok);
        error = true;
        return;
    }
}

void makestack::regenerate()
{
    readSettings();
    QImage original;
    original.load(filename);

    for (int n = 0; n < MainWin->ui->spinSize->value(); n++)
        for (int m = 0; m < MainWin->ui->spinSize->value(); m++)
        {
            QColor nmPixel = original.pixel(n, m);
            environment[n][m][0] = nmPixel.red();
            environment[n][m][1] = nmPixel.green();
            environment[n][m][2] = nmPixel.blue();
        }
}

void makestack::readSettings()
{
    filename = MainWin->ui->sFromImTxt->toPlainText();
}
