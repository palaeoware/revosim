#ifndef BREEDSYSTEM_H
#define BREEDSYSTEM_H

#include "system.h"

class BreedSystem : public System
{
public:
    BreedSystem();
    bool tryBreed(quint32 *genome, quint32 *partnerGenome, int maxDifference);
};

#endif // BREEDSYSTEM_H
