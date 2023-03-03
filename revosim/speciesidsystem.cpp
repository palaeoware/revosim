#include "speciesidsystem.h"
#include "simmanager.h"

SpeciesIDSystem::SpeciesIDSystem() : System("Species ID system")
{
    //Set up masks
    //mask 0 is all bits
    //mask 1 is every other bit
    //mask 2 is every third bit etc
    for (int i=0; i<MAX_MASKS; i++)
    {
        quint32 m = 0;
        int c =0;
        for (int j=0; j<32; j+=(i+1))
        {
            m |= tweakers[j];
            c++;
        }
        mask[i]=m;
        maskMaxBitCountPerWord[i]=c+1;
    }

}

//does what it says on the tin
int SpeciesIDSystem::bitcountAll(quint32 *genome)
{
    int bitcount=0;
    for (int i=0; i<useGenomeWordsCount; i++)
        bitcount += bitCount(genome[useGenomeWords[i]]);

    return bitcount;
}

//bitcount with level mask applied (defaults to bitcountall if maskID is 0, assumes this is all bits
int SpeciesIDSystem::bitcountAllMasked(quint32 *genome, int maskID)
{

    if (maskID==0) return bitcountAll(genome);

    int bitcount=0;
    quint32 thisMask = mask[maskID];
    for (int i=0; i<useGenomeWordsCount; i++)
        bitcount += bitCount(genome[useGenomeWords[i]] & thisMask);

    return bitcount;
}



bool SpeciesIDSystem::isCompatible(quint32 *genome, quint32 *partnerGenome, int maxDifference)
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
