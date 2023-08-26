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

#include "environmentclass.h"

EnvironmentClass::EnvironmentClass(EnvironmentSettings constructorSettings)
{
    x = constructorSettings.x;
    y = constructorSettings.y;

    savePath = constructorSettings.savePath;
    batch = constructorSettings.batch;

    mainWindowPointer = constructorSettings.mainWindowPointer;

    for (int n = 0; n < x; n++)
        for (int m = 0; m < y; m++)
        {
            environment[n][m][2] = (quint8)200;
            environment[n][m][1] = (quint8)110;
            environment[n][m][0] = (quint8)0;
        }
}

EnvironmentClass::~EnvironmentClass()
{

}


void EnvironmentClass::regenerate()
{

}
