#include "hgtsystem.h"
#include "simmanager.h"

HgtSystem::HgtSystem() : System("HGT System")
{

}

//PG- determine if HGT will occur if chance preset from GUI
bool HgtSystem::willTransform()
{
    quint64 transformChance = simulationManager->simulationRandoms->rand64 () % simulationManager->cellSettingsMaster->hgtTransferChance;
    return (transformChance == 1);
}


//PG- generate transfer genome mask for donor genome with bits set in transformation position
void HgtSystem::generateMask(quint32* genome, quint32* mask, size_t size)
{
    quint32 genomeLength = size * 32;
    for (size_t i = 0; i < size; i++) mask[i] = ~(0); //- create mask with same number of elements as genome words, all bits set
    quint32 transferLength = generateTransferLength(genome); //- determine transfer length based on settings
    quint32 startPosition = simulationManager->simulationRandoms->rand32() % (genomeLength - 1); //- select random start position betwee 0 and genome length
    bool wraps = (startPosition + transferLength > genomeLength); //- set the mask based on start position and transfer length
    quint32 wrappedLength = (startPosition + transferLength) %  genomeLength;

    //- loop through all bits in the mask
    for (int i = 0; i < simulationManager->simulationSettings->genomeSize; i++)
    {
        for (int j = 0; j < 32; j++)
        {
            quint32 index = (j + (i * 32)) % genomeLength;
            if ((index >= startPosition && !wraps && index < startPosition + transferLength) || // no wrapping and bit position within transfer segment
                (index >= startPosition && wraps) || //  wrapping and bit position greater or eqaul to start position
                (index < wrappedLength && wraps))  // wrapping and less than wrapped length
            {
                (mask[i] &= ~(1 << j)); // make bit = 0
            }
        }
    }
}

// PG- set transfer length from user settings - either random, variable dicatated by the genome or preset from GUI
quint32 HgtSystem::generateTransferLength(quint32* genome)
{ 
    quint32 transferLength = 0;
    if (simulationManager->simulationSettings->hgtrandomlength) transferLength = simulationManager->simulationRandoms->rand32() % (simulationManager->simulationSettings->genomeSize *32);
    else if (simulationManager ->simulationSettings->variableHgt) transferLength = simulationManager->variableHgtLenSystem->transferLength(genome);
    else  transferLength = simulationManager->cellSettingsMaster->hgtTransferLength;
    return transferLength;
}

//PG- make mask contain the transfer segment from donor genome
void HgtSystem::generateTransfer(quint32* donorGenome, quint32* mask, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        donorGenome[i] = ~(mask[i]) & donorGenome[i];
    }
}

//PG- transform organism genome to include transfer segement
void HgtSystem::transformRecipient(quint32* genome, quint32* donorGenome, quint32* mask, size_t size)
{
    //- if non-synomous transfer segment is either shifted left or right so it's put into the recipent genome in a different position
    if (simulationManager->simulationSettings->hgtMode==HGT_NON_SYNOYMOUS)
    {
        //- decide movement direction
        quint32 leftShift = simulationManager->simulationRandoms->rand32()% 2;
        for (size_t i = 0; i < size; i++)
        {
            int j =0;
            if (leftShift)
            {
                if (i != size -1) j = i + 1;  // make j the next genome word, unless last genome word where j stays 0
                donorGenome[i] = circularLeftShift(donorGenome[i], donorGenome[j]); // shift genome transfer
                mask[i] = circularLeftShift(mask[i], mask[j]); // shift mask (used to blank area of transfer in next step)
            } else {                                             //reverse of left shift
                if (i != 0) j = i - 1; else j = size -1; // make j the previous genome word, unless genomeword[0], then j = size - 1
                donorGenome[i] = circularRightShift(donorGenome[i], donorGenome[j]); // shift genome transfer
                mask[i] = circularRightShift(mask[i], mask[j]); // shift mask
            }
        }
    }

    // copy into the recipent genome, blank out target area with mask then or with the transfer genome
    for (size_t i = 0; i < size; i++)
    {
        genome[i] = (genome[i] & mask[i])| donorGenome[i];
    }
}

quint32 HgtSystem::circularLeftShift(quint32 currentWord, quint32 nextWord)
{
    quint32 forwardMask = 1 << 0; // make mask for first bit
    forwardMask = (nextWord & forwardMask) << 31; // copy least significant and move to most significant position
    return (currentWord << 1) | forwardMask; // shift word left and then or with the mask
}

quint32 HgtSystem::circularRightShift(quint32 currentWord, quint32 previousWord)
{
    quint32 backwardMask = 1 << 31; // make mask for last bit
    backwardMask = (previousWord & backwardMask) >> 31; // copy most significant and move to least significant position
    return (currentWord >> 1) | backwardMask; // shift word right and then or with the mask
}
