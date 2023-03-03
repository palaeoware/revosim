#include "groupdata.h"
#include "logspecies.h"

GroupData::GroupData(int gID)
{
    groupID = gID;
    genomeCount = 0;
    occurrenceCount = 0;
    modalGenome = nullptr;
    speciesID = -1;
    LogSpecies *logSpecies;
    Q_UNUSED(logSpecies);
}
