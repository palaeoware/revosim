#ifndef ENVIRONMENTFITNESSSYSTEM_H
#define ENVIRONMENTFITNESSSYSTEM_H

#include "system.h"
#include "critter.h"

class EnvironmentFitnessSystem : public System
{
public:
    EnvironmentFitnessSystem();
    quint32 calculateFitness(const quint32 *genome, const quint8 *environmentt);
    bool findRandomViableGenome(quint32 *genome, const quint8 *environment, int target, int settleTolerance);
    bool findDualViableGenome(quint32 *genome, const quint8 *environment, const quint8 *environment2, int target, int settleTolerance);
    void setXOR(int n, int m, quint32 number);
    quint32 returnXOR(int n, int m);

private:
    quint32 xorAndCount(const quint32 genomeWord, const quint8 env, const int colour);
    quint32 xorMasks[256][3]; //determine fitness - just 32 bit (lower bit of genome)
    void doMutation(quint32 *genome, quint32 *newGenome, int geneWords);
    int scoreForFind(quint32 *genome, const quint8 *environment, int target);
};

#endif // ENVIRONMENTFITNESSSYSTEM_H
