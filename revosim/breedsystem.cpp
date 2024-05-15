#include "breedsystem.h"

BreedSystem::BreedSystem() : System("Breed System")
{

}

//Return true if breed compatible
bool BreedSystem::tryBreed(quint32 *genome, quint32 *partnerGenome, int maxDifference)
{
    return breedDistance(genome,partnerGenome)<=maxDifference;
}

int BreedSystem::breedDistance(quint32 *genome, quint32 *partnerGenome)
{
    int bitcount=0;
    for (int i=0; i<useGenomeWordsCount; i++)
    {
        //Get XORed genome of the two critters
        quint32 genomeWord = genome[useGenomeWords[i]] ^ partnerGenome[useGenomeWords[i]];

        //add to cumulative bit difference count
        bitcount += bitCount(genomeWord);
    }
    return bitcount;
}
