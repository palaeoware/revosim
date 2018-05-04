/**
 * @file
 * Header: Log Species
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

#ifndef LOGSPECIES_H
#define LOGSPECIES_H
#include <QList>
#include <QString>
#include "logspeciesdataitem.h"

//used to log all species that appear. species class is for internal simulation purposes. This is for output.
class LogSpecies
{

public:
    LogSpecies();
    ~LogSpecies();

    quint64 ID;
    LogSpecies *parent;
    quint64 timeOfFirstAppearance;
    quint64 timeOfLastAppearance;
    QList<LogSpeciesDataItem *>data_items;
    QList<LogSpecies *>children;
    quint32 maxsize;
    QString newickstring(int childindex, quint64 last_time_base, bool killfluff);
    bool isfluff();
    int maxsize_inc_children();
    QString dump_data(int childindex, quint64 last_time_base, bool killfluff, quint64 parentid=0);
    QString my_data_line(quint64 start, quint64 end, quint64 myid, quint64 pid);
};

#endif // LOGSPECIES_H
