/**
 * @file
 * Source: Colour
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
#include "colour.h"

colour::colour(EnvironmentSettings constructorSettings) : EnvironmentClass(constructorSettings)
{
    colourRed = constructorSettings.colourEnvironmentSettings.colourRed;
    colourGreen = constructorSettings.colourEnvironmentSettings.colourGreen;
    colourBlue = constructorSettings.colourEnvironmentSettings.colourBlue;
}


void colour::regenerate()
{
    for (int n = 0; n < x; n++)
        for (int m = 0; m < y; m++)
        {
            environment[n][m][0] = colourRed;
            environment[n][m][1] = colourGreen;
            environment[n][m][2] = colourBlue;
        }
}
