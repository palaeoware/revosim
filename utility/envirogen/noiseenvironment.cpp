/**
 * @file
 * Source: Nois Enironment
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

#include "environmentclass.h"
#include "noiseenvironment.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <time.h>

noiseenvironment::noiseenvironment()
{
    qsrand(time(NULL));
    ReadSettings();
}


void noiseenvironment::ReadSettings()
{
    nMin = MainWin->ui->noiseMin->value();
    nMax = MainWin->ui->noiseMax->value();
}

void noiseenvironment::regenerate()
{
    ReadSettings();

    for (int n = 0; n < MainWin->ui->spinSize->value(); n++)
        for (int m = 0; m < MainWin->ui->spinSize->value(); m++)
        {
            for (int i = 0; i < 3; i++)
            {
                quint8 noiseVal;

                do
                {
                    noiseVal = Rand8();
                }
                while (noiseVal < nMin || noiseVal > nMax);

                environment[n][m][i] = noiseVal;
            }
        }

}
