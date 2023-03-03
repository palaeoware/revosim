/**
 * @file
 * Source: MarkEnvironment
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

#include <QTextStream>
#include <QString>
#include <QFile>
#include <QApplication>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "markenvironment.h"

markenvironment::markenvironment()
{
    ReadSettings();

    for (int i = 0; i < objectcount; i++)
    {
        objxpos[i] = MainWin->simulationRandoms->randDouble() * MainWin->ui->spinSize->value();
        objypos[i] = MainWin->simulationRandoms->randDouble() * MainWin->ui->spinSize->value();
        objxvel[i] = MainWin->simulationRandoms->randDouble() * maxvel * 2 - maxvel;
        objyvel[i] = MainWin->simulationRandoms->randDouble() * maxvel * 2 - maxvel;
        objsize[i] = MainWin->simulationRandoms->randDouble() * (maxsize - minsize) + minsize;
        objsizevel[i] = MainWin->simulationRandoms->randDouble() * maxsizevel * 2 - maxsizevel;
        for (int o = 0; o < 3; o++)
            objcolours[i][o] = (double)((int)MainWin->simulationRandoms->rand8() - 127);

        for (int o = 0; o < 3; o++)
            objcolvel[i][o] = MainWin->simulationRandoms->randDouble() * maxcolvel * 2 - maxcolvel;

        objtightness[i] = MainWin->simulationRandoms->randDouble() * (maxtight - mintight) + mintight;
        objtightvel[i] = MainWin->simulationRandoms->randDouble() * maxtightvel * 2 - maxtightvel;
    }

    iter_to_accel = iter_reset;

}

void markenvironment::ReadSettings()
{
    objectcount = MainWin->ui->ME_Objects->value();
    maxsize = (double)(MainWin->ui->ME_MaxSize->value());
    minsize = (double)(MainWin->ui->ME_MinSize->value());
    maxvel = MainWin->ui->ME_MaxVel->value();
    maxsizevel = MainWin->ui->ME_MaxSizeVel->value();
    maxcolvel = MainWin->ui->ME_MaxColVel->value();
    maxtightvel = MainWin->ui->ME_MaxTightVel->value();
    maxtight = MainWin->ui->ME_MaxTight->value();
    mintight = MainWin->ui->ME_MinTight->value();
    speedfactor = MainWin->ui->ME_SpeedFactor->value();
    tighttweak = MainWin->ui->ME_MaxTightAccel->value();
    coltweak = MainWin->ui->ME_MaxColAccel->value();
    veltweak = MainWin->ui->ME_MaxAccel->value();
    sizetweak = MainWin->ui->ME_MaxSizeAccel->value();
    iter_reset = MainWin->ui->ME_Iterations->value();

}

void markenvironment::regenerate()
{

    ReadSettings();

    //reset to black
    for (int n = 0; n < MainWin->ui->spinSize->value(); n++)
        for (int m = 0; m < MainWin->ui->spinSize->value(); m++)
        {

            environment[n][m][2] = 127;
            environment[n][m][1] = 127;
            environment[n][m][0] = 127;
        }

    //iterate and apply lights
    for (int i = 0; i < objectcount; i++)
    {
        //add velocities
        objxpos[i] += (objxvel[i] * speedfactor);
        objypos[i] += (objyvel[i] * speedfactor);
        //handle bouncing
        if (objxpos[i] < 0)
        {
            objxpos[i] = 0 - objxpos[i];
            objxvel[i] = 0 - objxvel[i];
        }
        if (objypos[i] < 0)
        {
            objypos[i] = 0 - objypos[i];
            objyvel[i] = 0 - objyvel[i];
        }
        if (objxpos[i] > (MainWin->ui->spinSize->value() - 1))
        {
            objxpos[i] = (2 * (MainWin->ui->spinSize->value() - 1)) - objxpos[i];
            objxvel[i] = 0 - objxvel[i];
        }
        if (objypos[i] > (MainWin->ui->spinSize->value() - 1))
        {
            objypos[i] = (2 * (MainWin->ui->spinSize->value() - 1)) - objypos[i];
            objyvel[i] = 0 - objyvel[i];
        }

        //add size change, apply limits
        objsize[i] += (objsizevel[i] * speedfactor);
        if (objsize[i] > maxsize)
        {
            objsize[i] = maxsize;
            objsizevel[i] = 0;
        }
        if (objsize[i] < minsize)
        {
            objsize[i] = minsize;
            objsizevel[i] = 0;
        }

        //do colour
        for (int o = 0; o < 3; o++)
        {
            objcolours[i][o] += (objcolvel[i][o] * speedfactor);
            if (objcolours[i][o] > 128)
            {
                objcolours[i][o] = 128;
                objcolvel[i][o] = 0;
            }
            if (objcolours[i][o] < -127)
            {
                objcolours[i][o] = -127;
                objcolvel[i][o] = 0;
            }
        }

        //do tightness
        objtightness[i] += (objtightvel[i] * speedfactor);
        if (objtightness[i] > maxtight)
        {
            objtightness[i] = maxtight;
            objtightvel[i] = 0;
        }
        if (objtightness[i] < mintight)
        {
            objtightness[i] = mintight;
            objtightvel[i] = 0;
        }

        //tweak velocities
        iter_to_accel--;
        if (iter_to_accel <= 0)
        {
            objxvel[i] += (MainWin->simulationRandoms->randDouble() * veltweak * 2 - veltweak) * speedfactor / (double)iter_reset;
            objyvel[i] += (MainWin->simulationRandoms->randDouble() * veltweak * 2 - veltweak) * speedfactor / (double)iter_reset;
            objsizevel[i] += (MainWin->simulationRandoms->randDouble() * sizetweak * 2 - sizetweak) * speedfactor / (double)iter_reset;
            objtightvel[i] += (MainWin->simulationRandoms->randDouble() * tighttweak * 2 - tighttweak) * speedfactor / (double)iter_reset;
            for (int o = 0; o < 3; o++) objcolvel[i][o] += (MainWin->simulationRandoms->randDouble() * 2 * coltweak - coltweak) * speedfactor / (double)iter_reset;
            iter_to_accel = iter_reset;
        }


        //limit velocities
        if (objxvel[i] > maxvel) objxvel[i] = maxvel;
        if (objxvel[i] < (0 - maxvel)) objxvel[i] = (0 - maxvel);
        if (objyvel[i] > maxvel) objyvel[i] = maxvel;
        if (objyvel[i] < (0 - maxvel)) objyvel[i] = (0 - maxvel);
        if (objsizevel[i] > maxsizevel) objsizevel[i] = maxsizevel;
        if (objsizevel[i] < (0 - maxsizevel)) objsizevel[i] = (0 - maxsizevel);
        for (int o = 0; o < 3; o++) if (objcolvel[i][o] < (0 - maxcolvel)) objcolvel[i][o] = (0 - maxcolvel);
        for (int o = 0; o < 3; o++) if (objcolvel[i][o] > maxcolvel) objcolvel[i][o] = maxcolvel;


        for (int n = 0; n < MainWin->ui->spinSize->value(); n++)
            for (int m = 0; m < MainWin->ui->spinSize->value(); m++)
            {
                //work out distance
                double dist = sqrt((n - objxpos[i]) * (n - objxpos[i]) + (m - objypos[i]) * (m - objypos[i]));
                if (dist < objsize[i])
                {
                    for (int o = 0; o < 3; o++)
                        environment[n][m][o] = add_and_limit(environment[n][m][o], (int)(objcolours[i][o]), dist, objsize[i], objtightness[i]);
                }
            }
    }


    saveMe = false;
}

quint8 markenvironment::add_and_limit(int oldvalue, int color, double dist, double maxdist, double tightness)
{
    int retval;

    double angledist = PI * .5 * dist / maxdist; //works out distance as radians - so can use cosine dropoff
    retval = oldvalue + (int)(pow(cos(angledist), tightness) * (double)color);

    if (retval < 0) retval = 0;
    if (retval > 255) return 255;
    else return (quint8)retval;
}
