#include "breedsystem.h"

BreedSystem::BreedSystem() : System("Breed System")
{

}

//Return true if breed compatible
bool BreedSystem::tryBreed(quint32 *genome, quint32 *partnerGenome, int maxDifference)
{
    int bitcount=0;
    for (int i=0; i<useGenomeWordsCount; i++)
    {
        //Get XORed genome of the two critters
        quint32 genomeWord = genome[useGenomeWords[i]] ^ partnerGenome[useGenomeWords[i]];

        //add to cumulative bit difference count
        bitcount += bitCount(genomeWord);
        if (bitcount>maxDifference) return false; //get out if gone over limit (cut out some iterations)
    }
    return true; // got to end, so must have had max or fewer differences
}
