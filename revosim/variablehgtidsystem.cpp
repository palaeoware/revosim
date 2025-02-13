#include "variablehgtidsystem.h"
#include "simmanager.h"


VariableHgtIdSystem::VariableHgtIdSystem() : System("Variable HGT ID System")
{

}

bool VariableHgtIdSystem::setGenomeWordsFromString(QString s, int maxsize)
{
    bool returnValue = System::setGenomeWordsFromString(s, maxsize);
    if (returnValue) createLinearDistribution();
    return returnValue;
}


//PG - Bit Counting to see the difference between the donor transfer segment and the recipent genome
bool VariableHgtIdSystem::tryTransform(quint32* genome, quint32* maskofgenome, quint32* maskofdonor)
{
    quint32 bitcount=0;
    quint32 maxDifference=0;

    if (simulationManager->simulationSettings->variableHgtId)
    {
        //- Bit count for genome word used in the variable hgt id system
        for (int i = 0; i < useGenomeWordsCount; i++)
        {
            quint32 genomeWord =genome[useGenomeWords[i]];
            bitcount += bitCount(genomeWord);
        }

        //- stolen safety check in case user has not set genomeWords to use this
        if (bitcount >= static_cast<quint32>(linearDistribution.length()))
        {
            qInfo() << "Error - returning false in variable HGT ID mechanism";
            return false;
        }
        //- Make the max difference between the donor transfer segment and reciprical genomic place in the recipent
       maxDifference= linearDistribution[bitcount];
    } else {

        maxDifference= simulationManager->simulationSettings-> maxDifferenceHgt;  //ADD TO THE GUI
    }

    //- reset bit count to 0 for sequence difference evaluation
    bitcount=0;

    for (int i = 0; i < simulationManager->simulationSettings->genomeSize; i++) // CHECK WORD COUNT IS RIGHT HERE
    {
        //Get XORed genome of the two masks to get bit difference between the transfer sequence
        quint32 genomeWord = maskofgenome[i] ^ maskofdonor[i];

        //add to cumulative bit difference count
        bitcount += bitCount(genomeWord);
        //qDebug() << bitcount;
        if (bitcount > maxDifference) return false; //get out if gone over limit (cut out some iterations)
    }
    return true; // got to end, so must have had max or fewer differences
}


// PG - Linear differences allowed between donor and recipent transfer segment, between 0 and 30 differences. ID system is currently limited to 1 word in the genome to ensure whole numbers in bit count
void VariableHgtIdSystem::createLinearDistribution()
{
    linearDistribution.clear();
    quint32 step = 1;

    for (int i = 0; i < 32; i++)
    {
        linearDistribution.append(0 + (step * i));
    }
    //qDebug()<< linearDistribution;
 }





