/**
 * @file
 * Source: Randoms
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

#include "randoms.h"

randoms::randoms()
{
    newRandoms8();
    newRandoms16();
    newRandoms32();
    newRandoms64();

    //Note that all are quint16 - so will just cycle from 0 to 65536 with a ++
    nextRandom8 = 0;
    nextRandom16 = 0;
    nextRandom32 = 0;
    nextRandom64 = 0;
}

quint8 randoms::rand8()
{
    if (nextRandom8 == 0) newRandoms8();
    return randoms8[nextRandom8++];
}

double randoms::randDouble()
{
    return QRandomGenerator::global()->generateDouble();
}

void randoms::newRandoms8()
{
    quint32 randomsToWrite[65536];
    QRandomGenerator::global()->fillRange(randomsToWrite);
    for (int i = 0; i < 65536; i++) randoms8[i] =  randomsToWrite[i] & mask8;
}

quint16 randoms::rand16()
{
    if (nextRandom16 == 0) newRandoms16();
    return randoms16[nextRandom16++];
}

void randoms::newRandoms16()
{

    quint32 randomsToWrite[65536];
    QRandomGenerator::global()->fillRange(randomsToWrite);
    for (int i = 0; i < 65536; i++)randoms16[i] = randomsToWrite[i] & mask16;
}

quint32 randoms::rand32()
{
    if (nextRandom32 == 0) newRandoms32();
    return randoms32[nextRandom32++];
}

void randoms::newRandoms32()
{
    QRandomGenerator::global()->fillRange(randoms32);
}

quint64 randoms::rand64()
{
    if (nextRandom64 == 0) newRandoms64();
    return randoms64[nextRandom64++];
}

void randoms::newRandoms64()
{
    QRandomGenerator::global()->fillRange(randoms64);
}


