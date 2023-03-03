#include "bitcountsystem.h"
#include "simmanager.h"

// System to do bitcounting with random shift, for one of the ecology options

BitCountSystem::BitCountSystem() : System("Bitcount System")
{

}

// Method counts the number of 1s in the creature's genome, then adds some random variation.
int BitCountSystem::calculateBitCountWithJitter(const quint32 *genome)
{
    int bitcount = 1; //Starts at 1 so that, even if a bit is lost due to the random shift, the value returned is always >=0
    for (int i = 0; i < useGenomeWordsCount; i++)
    {
        quint32 genomeWord = genome[useGenomeWords[i]];
        bitcount += bitCount(genomeWord);
    }

    //add random shift (copied from ENF code)
    int shift = (simulationManager->simulationRandoms->rand16() % 2) - (simulationManager->simulationRandoms->rand16() % 2);
    bitcount += shift;

    //check boundaries and return (original had not bounds check)
    return qBound(0, useGenomeWordsCount * 32, bitcount);
}
