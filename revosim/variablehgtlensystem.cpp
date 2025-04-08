#include "variablehgtlensystem.h"
#include "simmanager.h"


VariableHgtLenSystem::VariableHgtLenSystem() : System("Variable HGT System")
{

}

bool VariableHgtLenSystem::setGenomeWordsFromString(QString s, int maxsize)
{
    bool returnValue = System::setGenomeWordsFromString(s, maxsize);
    if (returnValue) createCumulativeLinearDistribution();
    return returnValue;
}

// PG - Use genome to determine the length of transfer
quint32 VariableHgtLenSystem::transferLength(const quint32 *genome){

    //- if the transformable genome size has changed, set to match then recreate the linear distribution
    if (transformableGenomeSize != simulationManager->hgtSystem->returnUseGenomeWordsCount())
    {
        transformableGenomeSize = simulationManager->hgtSystem->returnUseGenomeWordsCount();
        createCumulativeLinearDistribution();
    }

    quint32 bitcount = 0;

    //- use values for the second variable hgt system to make sure right genome words are used
    for (int i = 0; i < useGenomeWordsCount; i++)
    {
        quint32 genomeWord = genome[useGenomeWords[i]];
        bitcount += bitCount(genomeWord);
    }

    //- stolen safety check in case user has not set genomeWords to use this
    if (bitcount >= static_cast<quint32>(cumulativeDistribution.length()))
    {
        qInfo() << "Error - returning false in variable HGT len";
        return false;
    }

    //- return cumalative distribution value
    return (cumulativeDistribution[bitcount]);
}

// PG - Use transformable genome to determine the length of transfer
void VariableHgtLenSystem::createCumulativeLinearDistribution()
{
    cumulativeDistribution.clear();
    quint32 max = transformableGenomeSize * 32;
    quint32 step = (transformableGenomeSize * 32) / (useGenomeWordsCount * 32);  //- Need to think about this because might not work for genome sizes not divisible by genome words in use eg genome size 96 and there are 2 words in use
    for (int i = 0; i < useGenomeWordsCount * 32; i++)
    {
        cumulativeDistribution.append(max - (step * i));
    }
    //qDebug()<< cumulativeDistribution ;
 }

//PG - for logging
quint32 VariableHgtLenSystem::returnBitcount(const quint32 *genome)
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
quint32 VariableHgtLenSystem::returnCumulativeDistributionAtN(int n)
{
    if (n >= cumulativeDistribution.length())
    {
        qInfo() << "Error returning cumulative distribution - n out of bounds";
        return 0;
    }
    else return cumulativeDistribution[n];
}



