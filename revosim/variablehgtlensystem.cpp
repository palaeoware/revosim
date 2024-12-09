#include "variablehgtlensystem.h"
#include "simmanager.h"


VariableHgtLenSystem::VariableHgtLenSystem() : System("Variable HGT System")
{

}

//PG- Create probability of transfer distribution based on genome words selected
bool VariableHgtLenSystem::setGenomeWordsFromString(QString s, int maxsize)
{
    bool returnValue = System::setGenomeWordsFromString(s, maxsize);
    if (returnValue) createCumulativeLinearDistribution();
    return returnValue;
}


// PG - Use genome to determine the length of transfer
quint32 VariableHgtLenSystem::transferLength(const quint32 *genome){

    quint32 bitcount = 0;
    //PG- use values for the second variable hgt system to make sure right genome words are used
    for (int i = 0; i < useGenomeWordsCount; i++)
    {
        quint32 genomeWord = genome[useGenomeWords[i]];
        bitcount += bitCount(genomeWord);
    }
    //PG - stolen safety check in case user has not set genomeWords to use this
    if (bitcount >= static_cast<quint32>(cumulativeDistribution.length()))
    {
        qInfo() << "Error - returning false in variable HGT len";
        return false;
    }
   // qDebug() << returnUseGenomeWordsCount();
    return cumulativeDistribution[bitcount];
}


void VariableHgtLenSystem::createCumulativeLinearDistribution()
{
    cumulativeDistribution.clear();

    quint32 max =  useGenomeWordsCount * 32.0; // change to make reactive to user input
    quint32 step = max / (useGenomeWordsCount * 32);

    for (int i = 0; i < useGenomeWordsCount * 32; i++) {
        cumulativeDistribution.append(max - (step * i));
    }
    //qDebug() << cumulativeDistribution;
 }
