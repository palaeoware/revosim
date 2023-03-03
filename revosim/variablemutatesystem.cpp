#include "variablemutatesystem.h"
#include "simmanager.h"

VariableMutateSystem::VariableMutateSystem() : System("Variable Mutate System")
{

}

bool VariableMutateSystem::willMutate(const quint32 *genome)
{
    //RJG - get bitcounts for the words assigned to this system
    quint32 bitcount = 0;
    for (int i = 0; i < useGenomeWordsCount; i++)
    {
        quint32 genomeWord = genome[useGenomeWords[i]];
        bitcount += bitCount(genomeWord);
    }
    //RJG - safety check in case user has not set genomeWords to use this
    if (bitcount >= static_cast<quint32>(cumulativeNormalDistribution.length()))
    {
        qInfo() << "Error - returning false in variableMutate";
        return false;
    }
    //RJG - probability of breeding follows the standard normal distribution from -3 to +3 ceated above
    //More 1's in non coding genome == higher probability of sexual reproduction - see documentation.
    return (simulationManager->simulationRandoms->rand32() >= cumulativeNormalDistribution[bitcount]);
}

// RJG - This modifies the inhereted function in order to redo the probability distribution when genome length is changed
bool VariableMutateSystem::setGenomeWordsFromString(QString s, int maxsize)
{
    bool returnValue = System::setGenomeWordsFromString(s, maxsize);
    redoCumulativeNormalDistribution();
    return returnValue;
}

#ifndef M_SQRT1_2
#define M_SQRT1_2 0.7071067811865475
#endif

// RJG - This is called from above, so is updated to match the required gene countwithout us having to worry about it any more
void VariableMutateSystem::redoCumulativeNormalDistribution()
{
    //RJG - fill cumulativeNormalDistribution with numbers for variable breeding
    //These are a cumulative standard normal distribution from -3 to 3, created using the math.h complementary error function
    //This if first equation here:https://en.wikipedia.org/wiki/Error_function#Cumulative_distribution_function - it create a standard normal cumulative distribution
    //Then scaled to zero to 32 bit rand max, to allow for probabilities within each simulationManager->iteration through a random number
    cumulativeNormalDistribution.clear();
    double x = -3.;
    double inc = (6. / static_cast<double>(useGenomeWordsCount * 32));

    for (int i = 0; i < useGenomeWordsCount * 32; i++)
    {
        double NSDF = (0.5 * erfc(-(static_cast<double>(x)) * M_SQRT1_2));
        cumulativeNormalDistribution.append(static_cast<quint32>(4294967296 * NSDF));
        x += inc;
    }
}

//RJG - to maintain functionality in log
quint32 VariableMutateSystem::returnCumulativeDistributionAtN(int n)
{
    if (n >= cumulativeNormalDistribution.length())
    {
        qInfo() << "Error returning cumulative distribution - n out of bounds";
        return 0;
    }
    else return cumulativeNormalDistribution[n];
}

//RJG - to maintain functionality in log
quint32 VariableMutateSystem::returnBitcount(const quint32 *genome)
{
    quint32 bitcount = 0;
    for (int i = 0; i < useGenomeWordsCount; i++)
    {
        quint32 genomeWord = genome[useGenomeWords[i]];
        bitcount += bitCount(genomeWord);
    }
    return bitcount;
}

