#include "mutationsystem.h"
#include "simmanager.h"
MutationSystem::MutationSystem(): System("Mutation System")
{

}

void MutationSystem::doMutation(quint32 *genome)
{
   int word = simulationManager->simulationRandoms->rand32() % useGenomeWordsCount;
   int bit = simulationManager->simulationRandoms->rand8() & 31;
   genome[useGenomeWords[word]] ^= tweakers[bit];
}
