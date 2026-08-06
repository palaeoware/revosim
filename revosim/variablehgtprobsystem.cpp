#include "variablehgtprobsystem.h"
#include "simmanager.h"


VariableHgtProbSystem::VariableHgtProbSystem() : System("Variable HGT System")
{

}

//PG - variable HGT probability system set up
bool VariableHgtProbSystem::setGenomeWordsFromString(QString s, int maxsize)
{
    bool returnValue = System::setGenomeWordsFromString(s, maxsize);
    if (returnValue) createCumulativeLinearDistribution();
    return returnValue;
}

//PG - if this returns true HGT will take place based on organism's genome
bool VariableHgtProbSystem::variableWillTransform(const quint32 *genome)
{
    //- get bitcount for the words assigned to this system
    quint32 bitcount = 0;

    for (int i = 0; i < useGenomeWordsCount; i++)
    {
        quint32 genomeWord = genome[useGenomeWords[i]];
        bitcount += bitCount(genomeWord);
    }

    //- stolen safety check in case user has not set genomeWords to use this
    if (bitcount >= static_cast<quint32>(cumulativeDistribution.length()))
    {
        qInfo() << "Error - returning false in variable HGT prob";
        return false;
    }

    //generate random number between 1 and value from 1000, transform if number is less than distrubution value
    quint64 number = (simulationManager->simulationRandoms->rand64() % 1000);
    return (number < cumulativeDistribution[bitcount]);

    //qDebug() << cumulativeDistribution;
}


//PG - create log linear distribution between 2 and 500 for bit count transformation probability
void VariableHgtProbSystem::createCumulativeLinearDistribution()
{

    cumulativeDistribution.clear();
    double max = 482.0;
    double min = 2.0;

    //- number of bits + 1 possible outcomes
    int steps = (useGenomeWordsCount * 32) + 1;

    for (int i = 0; i < steps; ++i) {

        //- get normalised multiplier by dividing by number of bits
        double normalised = double(i) / double(steps - 1);
        //- scale with multiplier, starting at 2 not 0 to allow the modulus function later
        double value = min + (normalised * double(max - min));
        //- append rounded quint64 for the randomised number
        cumulativeDistribution.append(quint64(std::round(value)));
    }
    //qDebug() << cumulativeDistribution;
}

//PG - stolen for logging
quint32 VariableHgtProbSystem::returnBitcount(const quint32 *genome)
{
    quint32 bitcount = 0;
    for (int i = 0; i < useGenomeWordsCount; i++)
    {
        quint32 genomeWord = genome[useGenomeWords[i]];
        bitcount += bitCount(genomeWord);
    }
    return bitcount;
}

//PG - for logging
quint32 VariableHgtProbSystem::returnCumulativeDistributionAtN(int n)
{
    if (n >= cumulativeDistribution.length())
    {
        qInfo() << "Error returning cumulative distribution - n out of bounds";
        return 0;
    }
    else return cumulativeDistribution[n];
}


