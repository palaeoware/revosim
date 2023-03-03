#ifndef GROUPDATA_H
#define GROUPDATA_H

#include <QVector>
#include "speciesbinentry.h"
#include "logspecies.h"
class GroupData
{
public:
    GroupData(int gID);
    int genomeCount;
    int occurrenceCount;
    SpeciesBinEntry *modalGenome;
    int groupID;
    int speciesID;
    LogSpecies *logSpecies;
    QVector<SpeciesBinEntry *> allGenomes;
};

#endif // GROUPDATA_H
