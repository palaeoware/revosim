/**
 * @file
 * Fossil Record Dock
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

#include "fossilrecord.h"
#include "simmanager.h"
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDebug>

FossilRecord::FossilRecord(int x, int y, int sparse, QString Name)
{
    fossils.clear();

    xpos=x; //grid pos
    ypos=y;
    sparsity=sparse; //quality of the record - iterations between records of fossil/env
    startage=generation; //iteration count when record started
    name=Name;
    recording=true;
    writtenonce=false; //set to true when written file header
    recorded=0;
}

FossilRecord::~FossilRecord()
{
    qDeleteAll(fossils);
}

void FossilRecord::StopRecording()
{
    recording=false;
}

void FossilRecord::StartRecording()
{
    recording=true;
}

void FossilRecord::MakeFossil()
{
    if (recording && ((generation % sparsity) == 0))
    {
        if (critters[xpos][ypos][0].age==0) return;

        Fossil *newfossil = new Fossil;

        newfossil->env[0]=environment[xpos][ypos][0];
        newfossil->env[1]=environment[xpos][ypos][1];
        newfossil->env[2]=environment[xpos][ypos][2];

        newfossil->genome=critters[xpos][ypos][0].genome;
        newfossil->timestamp=generation;

        fossils.append(newfossil);
        recorded++;
    }
}

void FossilRecord::WriteRecord(QString fname)
{
    QFile outputfile(fname);

    if (!writtenonce)
    {
        outputfile.open(QIODevice::WriteOnly);
        QTextStream out(&outputfile);

        out<<"'"<<name<<"'\n";
        out<<"'X',"<<xpos<<",'Y',"<<ypos<<",'start',"<<startage<<"\n";
        out<<"'Time','Red','Green','Blue','Genome',";

        for (int j=0; j<63; j++) out<<j<<",";
        out<<"63\n";
        outputfile.close();
    }

    outputfile.open(QIODevice::Append);
    QTextStream out(&outputfile);

    for (int i=0; i<fossils.length(); i++)
    {
        out<<fossils[i]->timestamp<<","<<fossils[i]->env[0]<<","<<fossils[i]->env[1]<<","<<fossils[i]->env[2]<<","<<fossils[i]->genome<<",";
        for (int j=0; j<63; j++)
           if (tweakers64[63-j] & fossils[i]->genome) out<<"1,"; else out<<"0,";

        if (tweakers64[0] & fossils[i]->genome) out<<"1\n"; else out<<"0\n";
    }
    outputfile.close();
    writtenonce=true;
    qDeleteAll(fossils);
    fossils.clear();
}
