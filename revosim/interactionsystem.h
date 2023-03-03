#ifndef INTERACTIONSYSTEM_H
#define INTERACTIONSYSTEM_H

#include "system.h"

class InteractionSystem : public System
{
public:
    InteractionSystem();
    int performInteractFitness(quint32 *ownGenome, quint32 *targetGenome, int own_fitness, bool interactBlocks);
    int performInteractEnergy(quint32 *ownGenome, quint32 *targetGenome, int minPredatorDelta, int targetEnergy, bool interactBlocks);

};

#endif // INTERACTIONSYSTEM_H
