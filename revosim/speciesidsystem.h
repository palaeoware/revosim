#ifndef SPECIESIDSYSTEM_H
#define SPECIESIDSYSTEM_H

#include "system.h"

#define MAX_MASKS 5
#define MAX_BIN_ENTRIES 25

class SpeciesIDSystem : public System
{
public:
    SpeciesIDSystem();
    bool isCompatible(quint32 *genome, quint32 *partnerGenome, int maxDifference);
    quint32 mask[MAX_MASKS];
    int maskMaxBitCountPerWord[MAX_MASKS];
    int bitcountAll(quint32 *genome);
    int bitcountAllMasked(quint32 *genome, int maskID);
};

#endif // SPECIESIDSYSTEM_H
