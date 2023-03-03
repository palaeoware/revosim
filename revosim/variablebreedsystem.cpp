#include "variablebreedsystem.h"
#include "simmanager.h"

VariableBreedSystem::VariableBreedSystem() : System("Variable Breed System")
{

}

// RJG - This modifies the inhereted function in order to redo the probability distribution when genome length is changed
bool VariableBreedSystem::setGenomeWordsFromString(QString s, int maxsize)
{
    bool returnValue = System::setGenomeWordsFromString(s, maxsize);
    if (returnValue) createCumulativeNormalDistribution();
    //if (returnValue) createCumulativeLinearDistribution();
    return returnValue;
}

#ifndef M_SQRT1_2
#define M_SQRT1_2 0.7071067811865475
#endif

//RJG - This is called from above, so is updated to match the required gene countwithout us having to worry about it any more
void VariableBreedSystem::createCumulativeNormalDistribution()
{
    //RJG - fill cumulativeNormalDistribution with numbers for variable breeding
    //These are a cumulative standard normal distribution from -3 to 3, created using the math.h complementary error function
    //This if first equation here:https://en.wikipedia.org/wiki/Error_function#Cumulative_distribution_function - it create a standard normal cumulative distribution
    //Then scaled to zero to 32 bit rand max, to allow for probabilities within each simulationManager->iteration through a random number
    cumulativeDistribution.clear();
    double x = -3.;
    double inc = (6. / static_cast<double>(useGenomeWordsCount * 32));

    for (int i = 0; i < useGenomeWordsCount * 32; i++)
    {
        double NSDF = (0.5 * erfc(-(static_cast<double>(x)) * M_SQRT1_2));
        cumulativeDistribution.append(static_cast<quint32>(4294967296 * NSDF));
        x += inc;
    }
}

//RJG - This is called from above, so is updated to match the required gene countwithout us having to worry about it any more
void VariableBreedSystem::createCumulativeLinearDistribution()
{
    cumulativeDistribution.clear();
    quint32 step = ~0;
    step = step / (useGenomeWordsCount * 32);
    quint32 max = ~0;
    for (int i = 0; i < useGenomeWordsCount * 32; i++)
    {
        cumulativeDistribution.append((max - (step / 2)) - (step * i));
    }
}

//RJG - if this returns true, then the critter will be asexual for this breed attempt
bool VariableBreedSystem::returnAsexual(const quint32 *genome)
{
    //RJG - get bitcounts for the words assigned to this system
    quint32 bitcount = 0;
    for (int i = 0; i < useGenomeWordsCount; i++)
    {
        quint32 genomeWord = genome[useGenomeWords[i]];
        bitcount += bitCount(genomeWord);
    }
    //RJG - safety check in case user has not set genomeWords to use this
    if (bitcount >= static_cast<quint32>(cumulativeDistribution.length()))
    {
        qInfo() << "Error - returning false in variableBreed";
        return false;
    }

    //RJG - probability of breeding follows the standard normal distribution from -3 to +3 ceated above *or* linear
    return (simulationManager->simulationRandoms->rand32() >= cumulativeDistribution[bitcount]);
}

QString VariableBreedSystem::printDistribution() const
{
    QString logString;
    QTextStream out(&logString);

    // Info on simulation setup
    out << "Number of ones\tProbability of **sexual** duplication\tPercent of sexual breeds\n";

    for (int i = 0; i < useGenomeWordsCount * 32; i++)
    {
        quint32 n = cumulativeDistribution.at(i);
        double percentProb =  ((4294967296. - static_cast<double>(n)) / 4294967296.) * 100.;
        out << i << "\t" << (4294967296 - n) << "/4294967296\t" << percentProb << "\n";
    }

    return logString;
}
