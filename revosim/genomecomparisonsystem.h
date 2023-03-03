#ifndef GENOMECOMPARISONSYSTEM_H
#define GENOMECOMPARISONSYSTEM_H

#include "system.h"

class GenomeComparisonSystem : public System
{
public:
    GenomeComparisonSystem();
    int findModalGenome(int x, int y);
};

#endif // GENOMECOMPARISONSYSTEM_H
