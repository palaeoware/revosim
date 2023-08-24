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

markenvironment::markenvironment(EnvironmentSettings constructorSettings) : EnvironmentClass(constructorSettings)
{
    //X and Y

    objectcount = constructorSettings.markEnvironmentSettings.objectcount;
    maxSize = constructorSettings.markEnvironmentSettings.maxSize;
    minsize = constructorSettings.markEnvironmentSettings.minsize;
    maximumVelocity = constructorSettings.markEnvironmentSettings.maximumVelocity;
    maxsizeVelocity = constructorSettings.markEnvironmentSettings.maxsizeVelocity;
    maxcolvel = constructorSettings.markEnvironmentSettings.maxcolvel;
    maxtightvel = constructorSettings.markEnvironmentSettings.maxtightvel;
    maxtight = constructorSettings.markEnvironmentSettings.maxtight;
    mintight = constructorSettings.markEnvironmentSettings.mintight;
    speedfactor = constructorSettings.markEnvironmentSettings.speedfactor;
    tighttweak = constructorSettings.markEnvironmentSettings.tighttweak;
    coltweak = constructorSettings.markEnvironmentSettings.coltweak;
    veltweak = constructorSettings.markEnvironmentSettings.veltweak;
    sizetweak = constructorSettings.markEnvironmentSettings.sizetweak;
    iter_reset = constructorSettings.markEnvironmentSettings.iter_reset;

    for (int i = 0; i < objectcount; i++)
    {
        objxpos[i] = simulationRandoms.randDouble() * x;
        objypos[i] = simulationRandoms.randDouble() * y;
        objxvel[i] = simulationRandoms.randDouble() * maximumVelocity * 2 - maximumVelocity;
        objyvel[i] = simulationRandoms.randDouble() * maximumVelocity * 2 - maximumVelocity;
        objsize[i] = simulationRandoms.randDouble() * (maxSize - minsize) + minsize;
        objsizeVelocity[i] = simulationRandoms.randDouble() * maxsizeVelocity * 2 - maxsizeVelocity;
        for (int o = 0; o < 3; o++)
            objcolours[i][o] = (double)((int)simulationRandoms.rand8() - 127);

        for (int o = 0; o < 3; o++)
            objcolvel[i][o] = simulationRandoms.randDouble() * maxcolvel * 2 - maxcolvel;

        objtightness[i] = simulationRandoms.randDouble() * (maxtight - mintight) + mintight;
        objtightvel[i] = simulationRandoms.randDouble() * maxtightvel * 2 - maxtightvel;
    }
    iter_to_accel = iter_reset;
}


void markenvironment::regenerate()
{

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
        objsize[i] += (objsizeVelocity[i] * speedfactor);
        if (objsize[i] > maxSize)
        {
            objsize[i] = maxSize;
            objsizeVelocity[i] = 0;
        }
        if (objsize[i] < minsize)
        {
            objsize[i] = minsize;
            objsizeVelocity[i] = 0;
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
            objxvel[i] += (simulationRandoms.randDouble() * veltweak * 2 - veltweak) * speedfactor / (double)iter_reset;
            objyvel[i] += (simulationRandoms.randDouble() * veltweak * 2 - veltweak) * speedfactor / (double)iter_reset;
            objsizeVelocity[i] += (simulationRandoms.randDouble() * sizetweak * 2 - sizetweak) * speedfactor / (double)iter_reset;
            objtightvel[i] += (simulationRandoms.randDouble() * tighttweak * 2 - tighttweak) * speedfactor / (double)iter_reset;
            for (int o = 0; o < 3; o++) objcolvel[i][o] += (simulationRandoms.randDouble() * 2 * coltweak - coltweak) * speedfactor / (double)iter_reset;
            iter_to_accel = iter_reset;
        }


        //limit velocities
        if (objxvel[i] > maximumVelocity) objxvel[i] = maximumVelocity;
        if (objxvel[i] < (0 - maximumVelocity)) objxvel[i] = (0 - maximumVelocity);
        if (objyvel[i] > maximumVelocity) objyvel[i] = maximumVelocity;
        if (objyvel[i] < (0 - maximumVelocity)) objyvel[i] = (0 - maximumVelocity);
        if (objsizeVelocity[i] > maxsizeVelocity) objsizeVelocity[i] = maxsizeVelocity;
        if (objsizeVelocity[i] < (0 - maxsizeVelocity)) objsizeVelocity[i] = (0 - maxsizeVelocity);
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
