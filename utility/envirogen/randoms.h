/**
 * @file
 * Header: Randoms
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

#ifndef RANDOMS_H
#define RANDOMS_H

#include <QTime>
#include <QFile>
#include <QMessageBox>

class randoms
{
public:
    randoms();
    ~randoms();
    quint16 gen_Rand16();
    quint8 gen_Rand8();
    double gen_float();

private:
    quint16 random_array[65536];
    quint16 nextrandom;
    void load_randoms();
};

// This is key to making simulation randoms accessible from mainwindow.cpp
extern randoms *simulation_randoms;

#endif // RANDOMS_H
