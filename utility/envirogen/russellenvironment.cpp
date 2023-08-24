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
    laplace();
}

void russellenvironment::laplace()
{

    //Interpolate
    //First fill colours

    //TODO - fix all multidimensional arrays to what we see in
    //https://stackoverflow.com/questions/1946830/multidimensional-variable-size-array-in-c



    double colourMap[MainWin->ui->spinSize->value()][MainWin->ui->spinSize->value()][3];
    //Do it all in double colourMap so don't get errors from using environment (integers)
    int laplace[MainWin->ui->spinSize->value()][MainWin->ui->spinSize->value()];
    double eTotal, e[3];
    //Laplacian = residual, total and then residual for R,G and B

    //Initialise colourmap from environment to make laplacian faster
    for (int n = 0; n < MainWin->ui->spinSize->value(); n++)
        for (int m = 0; m < MainWin->ui->spinSize->value(); m++)
        {
            laplace[n][m] = 0;
            for (int i = 0; i < 3; i++)colourMap[n][m][i] = environment[n][m][i];
        }

    double x, y;
    for (int l = 0; l < nSeed; l++)
    {
        bool templaplace[MainWin->ui->spinSize->value()][MainWin->ui->spinSize->value()];
        for (int n = 0; n < MainWin->ui->spinSize->value(); n++)
            for (int m = 0; m < MainWin->ui->spinSize->value(); m++)
                templaplace[n][m] = false;

        for (double z = -PI; z < PI; z += .01)
        {
            //Draw circles...
            x = seeds[l].n + (seeds[l].size * cos(z));
            y = seeds[l].m + (seeds[l].size * sin(z));

            if (periodic)
            {
                if (y > (MainWin->ui->spinSize->value() - 1)) y -= (MainWin->ui->spinSize->value() - 1);
                if (y < 0)y += (MainWin->ui->spinSize->value() - 1);
            }

            int radius = seeds[l].n - x;

            for (double newX = x; newX < seeds[l].n + radius; newX++)
            {
                if (newX > (MainWin->ui->spinSize->value() - 1))
                {
                    if (periodic)
                    {
                        templaplace[(int)newX - (MainWin->ui->spinSize->value() - 1)][(int)y] = true;
                        for (int i = 0; i < 3; i++)colourMap[(int)newX - (MainWin->ui->spinSize->value() - 1)][(int)y][i] = seeds[l].colour[i];
                    }
                }

                else if (newX < 0)
                {
                    if (periodic)
                    {
                        templaplace[(int)newX + (MainWin->ui->spinSize->value() - 1)][(int)y] = true;
                        for (int i = 0; i < 3; i++)colourMap[(int)newX + (MainWin->ui->spinSize->value() - 1)][(int)y][i] = seeds[l].colour[i];
                    }
                }

                else
                {
                    if (y > 0 && y < MainWin->ui->spinSize->value())
                    {
                        templaplace[(int)newX][(int)y] = true;
                        for (int i = 0; i < 3; i++)colourMap[(int)newX][(int)y][i] = seeds[l].colour[i];
                    }
                }
            }

        }

        //Create laplace matrix which counts how many spots are overlapping in any given area
        for (int n = 0; n < MainWin->ui->spinSize->value(); n++)
            for (int m = 0; m < MainWin->ui->spinSize->value(); m++)
                if (templaplace[n][m])laplace[n][m]++;

        if (buffer == 0) //But not if buffer is set to zero - easy way of turning off system
            for (int n = 0; n < MainWin->ui->spinSize->value(); n++)
                for (int m = 0; m < MainWin->ui->spinSize->value(); m++)
                    if (laplace[n][m] > 1)laplace[n][m] = 1;

        if (blur) //Set all pixels to laplace
            for (int n = 0; n < MainWin->ui->spinSize->value(); n++)
                for (int m = 0; m < MainWin->ui->spinSize->value(); m++)
                    laplace[n][m] = 0;
    }


//Dilate overlapped selection if needed
    for (int n = 0; n < MainWin->ui->spinSize->value(); n++)
        for (int m = 0; m < MainWin->ui->spinSize->value(); m++)
            if (laplace[n][m] > 1)
                //Current implementation is simple square which enlarges each overlapping pixel by amount buffer
                for (int i = (n - buffer); i < (n + buffer); i++)
                    for (int j = (m - buffer); j < (m + buffer); j++)
                    {
                        if (!periodic && i > 0 && j > 0 && i < (MainWin->ui->spinSize->value() - 1) && j < (MainWin->ui->spinSize->value() - 1) && laplace[i][j] == 1)laplace[i][j] = -1;
                        else laplace[(i + MainWin->ui->spinSize->value()) % MainWin->ui->spinSize->value()][(j + MainWin->ui->spinSize->value()) % MainWin->ui->spinSize->value()] = -1;
                    }

//Now smooth/interpolate
    int count = 0;
    do
    {
        eTotal = 0.0; //This is the residual
        for (int n = 0; n < MainWin->ui->spinSize->value(); n++)
            for (int m = 0; m < MainWin->ui->spinSize->value(); m++)
                if ((n + m) % 2 == count % 2) //Calculate it chess-board style
                    if (laplace[n][m] != 1) //If needs to be laplaced
                        for (int i = 0; i < 3; i++)
                        {
                            //Average difference surounding four pixels. Modulus to make periodic. Calculate laplacian residual.
                            if (periodic)e[i] = colourMap[(n + 1) % (MainWin->ui->spinSize->value() - 1)][m][i] + colourMap[(n - 1 + (MainWin->ui->spinSize->value() - 1)) % (MainWin->ui->spinSize->value() - 1)][m][i] +
                                                    colourMap[n][(m + 1) % (MainWin->ui->spinSize->value() - 1)][i] + colourMap[n][(m - 1 + (MainWin->ui->spinSize->value() - 1)) % (MainWin->ui->spinSize->value() - 1)][i]
                                                    - 4.0 * colourMap[n][m][i];
                            else
                            {
                                if (n == 0 && m == 0)e[i] = (colourMap[n][m + 1][i] + colourMap[n + 1][m][i]) - 2 * colourMap[n][m][i];
                                else if (n == (MainWin->ui->spinSize->value() - 1) && m == 0)e[i] = (colourMap[n - 1][m][i] + colourMap[n][m + 1][i]) - 2 * colourMap[n][m][i];
                                else if (n == 0 && m == (MainWin->ui->spinSize->value() - 1))e[i] = (colourMap[n][m - 1][i] + colourMap[n + 1][m][i]) - 2 * colourMap[n][m][i];
                                else if (n == (MainWin->ui->spinSize->value() - 1) && m == (MainWin->ui->spinSize->value() - 1))e[i] = (colourMap[n - 1][m][i] + colourMap[n][m - 1][i]) - 2 * colourMap[n][m][i];
                                else if (n == 0)e[i] = (colourMap[n][m + 1][i] + colourMap[n][m - 1][i] + colourMap[n + 1][m][i]) - 3 * colourMap[n][m][i];
                                else if (m == 0)e[i] = (colourMap[n][m + 1][i] + colourMap[n - 1][m][i] + colourMap[n + 1][m][i]) - 3 * colourMap[n][m][i];
                                else if (n == (MainWin->ui->spinSize->value() - 1))e[i] = (colourMap[n][m + 1][i] + colourMap[n - 1][m][i] + colourMap[n][m - 1][i]) - 3 * colourMap[n][m][i];
                                else if (m == (MainWin->ui->spinSize->value() - 1))e[i] = (colourMap[n - 1][m][i] + colourMap[n + 1][m][i] + colourMap[n][m - 1][i]) - 3 * colourMap[n][m][i];
                                else e[i] = colourMap[n + 1][m][i] + colourMap[n - 1][m][i] +
                                                colourMap[n][m + 1][i] + colourMap[n][m - 1][i]
                                                - 4.0 * colourMap[n][m][i];
                            }
                            colourMap[n][m][i] += 1.2 * e[i] / 4.0; //Colour = average of surrounding pixels
                            //1.2 == factor to speed up the calculation.
                            eTotal += fabs(e[i]);
                        }


        count++;
        eTotal = eTotal / (3.0 * ((double) MainWin->ui->spinSize->value()) * ((double) MainWin->ui->spinSize->value()));

        //Ideally still need to implement some kind of status bar / update
        if (count % 1000 == 0)
        {
            QString prog = QString("Residual is currently %1 ").arg(eTotal);
            MainWin->ui->statusBar->showMessage(prog);
        }
    }
    while (eTotal > converge);

    for (int n = 0; n < MainWin->ui->spinSize->value(); n++)
        for (int m = 0; m < MainWin->ui->spinSize->value(); m++)
            for (int i = 0; i < 3; i++)
            {
                environment[n][m][i] = (quint8)colourMap[n][m][i];
            }

}
