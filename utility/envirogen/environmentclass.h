/**
 * @file
 * Header: Environment header
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


#ifndef ENVIRONMENTCLASS_H
#define ENVIRONMENTCLASS_H

#define PI 3.141592654

#include <QMessageBox>
#include <QString>
#include "math.h"

//Definitions of images size
//#define GRID_X 100
//#define GRID_Y 100

//RJG This is the base class that a bunch of the environmental generators inheret, which includes save and random number functions
class environmentclass
{

public:
    environmentclass();
    virtual ~environmentclass();

    virtual void regenerate();

    void save(int generations);

    void path(QString files_directory,  bool doSave);

    //Hard limits of 1000 - right now.
    quint8 environment[1000][1000][3];

    bool error = false;

protected:
    QString dir;
    bool saveMe;
    quint8 Rand8();

private:

};

#endif // ENVIRONMENTCLASS_H
