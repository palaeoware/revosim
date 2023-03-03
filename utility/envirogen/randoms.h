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
#include <QRandomGenerator>

class randoms
{
public:
    randoms();
    quint8 rand8();
    quint16 rand16();
    quint32 rand32();
    quint64 rand64();
    double randDouble();

private:
    quint8 randoms8[65536];
    quint16 randoms16[65536];
    quint32 randoms32[65536];
    quint64 randoms64[65536];
    const quint8 mask8 = ~0;
    const quint16 mask16 = ~0;
    void newRandoms8();
    void newRandoms16();
    void newRandoms32();
    void newRandoms64();
    quint16 nextRandom8, nextRandom16, nextRandom32, nextRandom64;
};

#endif // RANDOMS_H
