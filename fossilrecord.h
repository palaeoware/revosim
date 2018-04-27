/**
 * @file
 * Header: Fossil Record Dock
 *
 * All REvoSim code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All REvoSim code is Copyright 2018 by Mark Sutton, Russell Garwood,
 * and Alan R.T. Spencer.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version. This program is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY.
 */

#ifndef FOSSILRECORD_H
#define FOSSILRECORD_H

#include <QList>
#include <QString>

struct Fossil
{
    quint64 genome;
    quint64 timestamp;
    quint8 env[3]; //environment recorded
};

//Encapsulates a virtual fossil locality - continuous record at some point on the grid
//Record is of genomes and environment
class FossilRecord
{
public:
    FossilRecord(int x, int y, int sparse, QString name);
    ~FossilRecord();
    void WriteRecord(QString fname);
    void MakeFossil();
    void StartRecording();
    void StopRecording();
    int xpos; //grid pos
    int ypos;
    int sparsity; //quality of the record - iterations between records of fossil/env
    int startage; //iteration count when record started
    QString name;
    bool recording;
    bool writtenonce;
    int recorded;

    QList<Fossil *> fossils;
};

#endif // FOSSILRECORD_H
