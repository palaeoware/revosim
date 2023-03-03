/**
 * @file
 * Header: Log Species
 *
 * All REvoSim code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All REvoSim code is Copyright 2008-2019 by Mark D. Sutton, Russell J. Garwood,
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

#include "logspeciesdataitem.h"

#include <QList>
#include <QString>

//used to log all species that appear. species class is for internal simulation purposes. This is for output.
class LogSpecies
{

public:
    LogSpecies();
    ~LogSpecies();

    bool isFluff();
    quint32 maxSizeIncludingChildren();
    QString writeNewickString(int childIndex, quint64 lastTimeBase, bool killFluff);
    QString writeData(int childIndex, quint64 lastTimeBase, bool killFluff, quint64 parentID = 0);
    QString writeDataLine(quint64 start, quint64 end, quint64 speciesID, quint64 parentID);

    quint32 maxSize{};
    quint64 ID{};
    quint64 timeOfFirstAppearance{};
    quint64 timeOfLastAppearance{};
    LogSpecies *parent{};
    QList<LogSpeciesDataItem *>dataItems;
    QList<LogSpecies *>children;
};

#endif // LOGSPECIES_H
