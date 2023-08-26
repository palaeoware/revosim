/**
 * @file
 * Source: Russell Environment
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

#include <QTextStream>
#include <QString>
#include <QFile>
#include <QApplication>
#include <QVarLengthArray>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "russellenvironment.h"
#include "environmentclass.h"

russellenvironment::russellenvironment(EnvironmentSettings constructorSettings) : EnvironmentClass(constructorSettings)
{

    //Setup
    nSeed = constructorSettings.russellEnvironmentSettings.nSeed;
    buffer = constructorSettings.russellEnvironmentSettings.buffer;
    maxSize = constructorSettings.russellEnvironmentSettings.maxSize;
    sizeVelocity = constructorSettings.russellEnvironmentSettings.sizeVelocity;
    maximumVelocity = constructorSettings.russellEnvironmentSettings.maximumVelocity;
    periodic = constructorSettings.russellEnvironmentSettings.periodic;
    blur = constructorSettings.russellEnvironmentSettings.blur;
    converge = constructorSettings.russellEnvironmentSettings.converge;
    numbGenerations = constructorSettings.russellEnvironmentSettings.numbGenerations;
    factor = constructorSettings.russellEnvironmentSettings.factor;
    maxAcceleration = 1;

    for (int i = 0; i < nSeed; i++)
    {
        //Initialise here for seeds at start
        for (int j = 0; j < 3; j++)seeds[i].colour[j] = (double)simulationRandoms.rand8();
        seeds[i].n = ((double)simulationRandoms.rand8() * ((double)x / 256.));
        seeds[i].m = ((double)simulationRandoms.rand8() * ((double)x / 256.));
        seeds[i].nv = 0.;
        seeds[i].mv = 0.;
        int r = simulationRandoms.rand8();
        seeds[i].size = ((double)r * (double)maxSize) / 256.;
        seeds[i].initialised = true;
    }

    //Allocate structures
    for (int i = 0; i < x; i++)
    {
        colourMap.append(QVector<QVector<double>>());
        templaplace.append(QVector<QVector<bool>>());
        laplace.append(QVector<int>());
        for (int j = 0; j < y; j++)
        {
            colourMap[i].append(QVector <double>());
            templaplace[i].append(QVector<bool>());
            laplace[i].append(0);
            for (int k = 0; k < 3; k++)
            {
                colourMap[i][j].append(static_cast<double>(environment[i][j][k]));
            }

            for (int k = 0; k < nSeed; k++)
            {
                templaplace[i][j].append(false);
            }
        }
    }
}


void russellenvironment::regenerate()
{
    //This code iterates the environment - for each it adds the required factors for an iteration
    for (int i = 0; i < nSeed; i++)
    {
        //Check initialised - do this so can add more seeds during run if needed
        if (!seeds[i].initialised)
        {
            for (int j = 0; j < 3; j++)seeds[i].colour[j] = (double)simulationRandoms.rand8();
            seeds[i].n = ((double)simulationRandoms.rand8() * ((double)x / 256.));
            seeds[i].m = ((double)simulationRandoms.rand8() * ((double)y / 256.));
            seeds[i].nv = 0.;
            seeds[i].mv = 0.;
            int r = simulationRandoms.rand8();
            seeds[i].size = ((double)r * (double)maxSize) / 256.;
        }

        //na to be added to velocity n - first come up with this iteration's value
        //+/-RAND    //limit it to max acceleration //apply factor
        na = ((double)simulationRandoms.rand8() - 128.) * ((double)maxAcceleration / 128.) * factor;
        //Apply soft limit if velocity is above/below max and acc is in wrong direction//
        if (fabs(seeds[i].nv) > maximumVelocity && (seeds[i].nv * na) > 0)na *= (1. / ((fabs(seeds[i].nv) - maximumVelocity + 1) * 5.));
        // 5 == 'strength' of soft limit

        ma = ((double)simulationRandoms.rand8() - 128.) * ((double)maxAcceleration / 128.) * factor;
        if (fabs(seeds[i].mv) > maximumVelocity && (seeds[i].mv * ma) > 0)ma *= (1. / ((fabs(seeds[i].mv) - maximumVelocity + 1) * 5.));

        //Accelerations to apply to nv/mv are now sorted.... Apply next
        seeds[i].nv += na;
        seeds[i].mv += ma;

        seeds[i].n += (seeds[i].nv * factor);
        seeds[i].m += (seeds[i].mv * factor);
        for (int j = 0; j < 3; j++)seeds[i].colour[j] += factor * ((double)((simulationRandoms.rand8() - 128.) * ((double)maxColourVelocity / 128.)));
        for (int j = 0; j < 3; j++)if ((int)seeds[i].colour[j] > 255)seeds[i].colour[j] = 255.;
        for (int j = 0; j < 3; j++)if ((int)seeds[i].colour[j] <= 0)seeds[i].colour[j] = 0.;
        seeds[i].size += factor * ((simulationRandoms.rand8() - 128) * ((double)sizeVelocity / 128.));


        if (periodic)
        {
            if (seeds[i].n > (x - .1))seeds[i].n = 0.;
            if (seeds[i].n < 0)seeds[i].n = (x - .1);
        }
        else
        {
            if (seeds[i].n > (x - .1))seeds[i].n = (x - .1);
            if (seeds[i].n < 0)seeds[i].n = 0.;
        }

        if (periodic)
        {
            if (seeds[i].m > (y - .1))seeds[i].m = 0.;
            if (seeds[i].m < 0)seeds[i].m = (y - .1);
        }
        else
        {
            if (seeds[i].m > (y - .1))seeds[i].m = (y - .1);
            if (seeds[i].m < 0)seeds[i].m = 0.;
        }

        if (seeds[i].size > maxSize)seeds[i].size = (double)maxSize;
        if (seeds[i].size < 1)seeds[i].size = 1.;
    }

    //Interpolation then occurs in the laplace function
    doLaplace();
}

void russellenvironment::doLaplace()
{

    //Interpolate
    double eTotal, e[3];
    //Laplacian = residual, total and then residual for R,G and B

    //First fill colours
    //Initialise colourmap from environment to make laplacian faster
    for (int n = 0; n < x; n++)
        for (int m = 0; m < y; m++)
        {
            laplace[n][m] = 0;
            for (int i = 0; i < 3; i++)colourMap[n][m][i] = environment[n][m][i];
            for (int i = 0; i < nSeed; i++)templaplace[n][m][i] = false;
        }

    double localX, localY;

    for (int l = 0; l < nSeed; l++)
    {
        for (double z = -PI; z < PI; z += .01)
        {
            //Draw circles...
            localX = seeds[l].n + (seeds[l].size * cos(z));
            localY = seeds[l].m + (seeds[l].size * sin(z));

            if (periodic)
            {
                if (localY > (y - 1)) localY -= (y - 1);
                if (localY < 0)localY += (y - 1);
            }

            int radius = seeds[l].n - localX;

            for (double newX = localX; newX < seeds[l].n + radius; newX++)
            {
                if (newX > (x - 1))
                {
                    if (periodic)
                    {
                        templaplace[(int)newX - (x - 1)][(int)localY][l] = true;
                        for (int i = 0; i < 3; i++)colourMap[(int)newX - (x - 1)][(int)localY][i] = seeds[l].colour[i];
                    }
                }

                else if (newX < 0)
                {
                    if (periodic)
                    {
                        templaplace[(int)newX + (x - 1)][(int)localY][l] = true;
                        for (int i = 0; i < 3; i++)colourMap[(int)newX + (x - 1)][(int)localY][i] = seeds[l].colour[i];
                    }
                }

                else
                {
                    if (localY > 0 && localY < y)
                    {
                        templaplace[(int)newX][(int)localY][l] = true;
                        for (int i = 0; i < 3; i++)colourMap[(int)newX][(int)localY][i] = seeds[l].colour[i];
                    }
                }
            }

        }

        //Create laplace matrix which counts how many spots are overlapping in any given area
        for (int n = 0; n < x; n++)
            for (int m = 0; m < y; m++)
                if (templaplace[n][m][l])laplace[n][m]++;

        if (buffer == 0) //But not if buffer is set to zero - easy way of turning off system
            for (int n = 0; n < x; n++)
                for (int m = 0; m < y; m++)
                    if (laplace[n][m] > 1) laplace[n][m] = 1;

        if (blur) //Set all pixels to laplace
            for (int n = 0; n < x; n++)
                for (int m = 0; m < y; m++)
                    laplace[n][m] = 0;
    }


//Dilate overlapped selection if needed
    for (int n = 0; n < x; n++)
        for (int m = 0; m < y; m++)
            if (laplace[n][m] > 1)
                //Current implementation is simple square which enlarges each overlapping pixel by amount buffer
                for (int i = (n - buffer); i < (n + buffer); i++)
                    for (int j = (m - buffer); j < (m + buffer); j++)
                    {
                        if (!periodic && i > 0 && j > 0 && i < (x - 1) && j < (y - 1) && laplace[i][j] == 1)laplace[i][j] = -1;
                        else laplace[(i + x) % x][(j + y) % y] = -1;
                    }

//Now smooth/interpolate
    int count = 0;
    do
    {
        eTotal = 0.0; //This is the residual
        for (int n = 0; n < x; n++)
            for (int m = 0; m < y; m++)
                if ((n + m) % 2 == count % 2) //Calculate it chess-board style
                    if (laplace[n][m] != 1) //If needs to be laplaced
                        for (int i = 0; i < 3; i++)
                        {
                            //Average difference surounding four pixels. Modulus to make periodic. Calculate laplacian residual.
                            if (periodic)e[i] = colourMap[(n + 1) % (x - 1)][m][i] + colourMap[(n - 1 + (x - 1)) % (x - 1)][m][i] +
                                                    colourMap[n][(m + 1) % (y - 1)][i] + colourMap[n][(m - 1 + (y - 1)) % (y - 1)][i]
                                                    - 4.0 * colourMap[n][m][i];
                            else
                            {
                                if (n == 0 && m == 0) e[i] = (colourMap[n][m + 1][i] + colourMap[n + 1][m][i]) - 2 * colourMap[n][m][i];
                                else if (n == (x - 1) && m == 0)e[i] = (colourMap[n - 1][m][i] + colourMap[n][m + 1][i]) - 2 * colourMap[n][m][i];
                                else if (n == 0 && m == (y - 1))e[i] = (colourMap[n][m - 1][i] + colourMap[n + 1][m][i]) - 2 * colourMap[n][m][i];
                                else if (n == (x - 1) && m == (y - 1))e[i] = (colourMap[n - 1][m][i] + colourMap[n][m - 1][i]) - 2 * colourMap[n][m][i];
                                else if (n == 0)e[i] = (colourMap[n][m + 1][i] + colourMap[n][m - 1][i] + colourMap[n + 1][m][i]) - 3 * colourMap[n][m][i];
                                else if (m == 0)e[i] = (colourMap[n][m + 1][i] + colourMap[n - 1][m][i] + colourMap[n + 1][m][i]) - 3 * colourMap[n][m][i];
                                else if (n == (x - 1))e[i] = (colourMap[n][m + 1][i] + colourMap[n - 1][m][i] + colourMap[n][m - 1][i]) - 3 * colourMap[n][m][i];
                                else if (m == (y - 1))e[i] = (colourMap[n - 1][m][i] + colourMap[n + 1][m][i] + colourMap[n][m - 1][i]) - 3 * colourMap[n][m][i];
                                else e[i] = colourMap[n + 1][m][i] + colourMap[n - 1][m][i] +
                                                colourMap[n][m + 1][i] + colourMap[n][m - 1][i]
                                                - 4.0 * colourMap[n][m][i];
                            }
                            colourMap[n][m][i] += 1.2 * e[i] / 4.0; //Colour = average of surrounding pixels
                            //1.2 == factor to speed up the calculation.
                            eTotal += fabs(e[i]);
                        }


        count++;
        eTotal = eTotal / (3.0 * ((double) x) * ((double) y));

        //Ideally still need to implement some kind of status bar / update
        if (count % 1000 == 0 && !batch)
        {
            QString prog = QString("Residual is currently %1 ").arg(eTotal);
            mainWindowPointer->ui->statusBar->showMessage(prog);
        }
    }
    while (eTotal > converge);

    for (int n = 0; n < x; n++)
        for (int m = 0; m < y; m++)
            for (int i = 0; i < 3; i++)
            {
                environment[n][m][i] = (quint8)colourMap[n][m][i];
            }

}
