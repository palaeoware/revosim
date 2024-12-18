#include "variablehgtprobsystem.h"
#include "simmanager.h"


VariableHgtProbSystem::VariableHgtProbSystem() : System("Variable HGT System")
{

}

bool VariableHgtProbSystem::setGenomeWordsFromString(QString s, int maxsize)
{
    bool returnValue = System::setGenomeWordsFromString(s, maxsize);
    if (returnValue) createCumulativeLogLinearDistribution();
    //if (returnValue) createCumulativeLinearDistribution();
    return returnValue;
}

//PG - if this returns true HGT will take place based on organism's genome
bool VariableHgtProbSystem::variableWillTransform(const quint32 *genome)
{
    //- get bitcounts for the words assigned to this system
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

    //generate random number between 1 and value from cumlative distribution, return true if number is 1
    quint64 number = (simulationManager->simulationRandoms->rand64() % cumulativeDistribution[bitcount]);
    return (number == 1);
}

//PG - create linear distribution between 10^-7 and 10^-3 chance for bit count
void VariableHgtProbSystem::createCumulativeLinearDistribution(){

    cumulativeDistribution.clear();

    quint64 max = 1e7;
    quint64 min = 1e3;
    quint64 step = (max - min) / ((useGenomeWordsCount * 32)-1);

    for (int i = 0; i < useGenomeWordsCount * 32; i++) cumulativeDistribution.append(max - (step * i));
}


//PG - create log linear (semi-log) distribution between 10^-7 and 10^-3 chance for bit count
void VariableHgtProbSystem::createCumulativeLogLinearDistribution()
{
    cumulativeDistribution.clear();

    quint64 max = 1e7;
    quint64 min = 1e3;
    double step = (log10(max) - log10(min)) / ((useGenomeWordsCount* 32) - 1);

    for (int i = 0; i < useGenomeWordsCount * 32; i++)
    {
        double LogValue = log10(max) - (step * i);
        double value = std::pow(10, LogValue);


        cumulativeDistribution.append(static_cast<quint64>(value));
    }
}


