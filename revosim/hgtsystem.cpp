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
void HgtSystem::generateMask(quint32* genome, quint32* mask)
{
    //- set max transferable length based on the words set in the GUI
    quint32 genomeTransformableLength = useGenomeWordsCount * 32;
    //- create mask with same number of elements as transformable genome words, all bits set
    for (int i =0; i < useGenomeWordsCount; i++ )
    {
        mask[i] = ~(0);
    }

    quint32 transferLength = generateTransferLength(genome); //- determine transfer length based on settings


    //- Transferable genome is treated as circular to allow wrapping
    quint32 startPosition = simulationManager->simulationRandoms->rand32() % (genomeTransformableLength - 1); //- select random start position betwee 0 and transformable genome length
    bool wraps = (startPosition + transferLength > genomeTransformableLength); //- set the mask based on start position and transfer length
    quint32 wrappedLength = (startPosition + transferLength) %  genomeTransformableLength;

    //- loop through all bits in the mask
    for (int i = 0; i < simulationManager->simulationSettings->genomeSize; i++)
    {
        for (int j = 0; j < 32; j++)
        {
            quint32 index = (j + (i * 32)) % genomeTransformableLength;
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
    if (simulationManager->simulationSettings->hgtrandomlength) transferLength = simulationManager->simulationRandoms->rand32() % (useGenomeWordsCount * 32);
    else if (simulationManager ->simulationSettings->variableHgtLen) transferLength = simulationManager->variableHgtLenSystem->transferLength(genome);
    else  transferLength = simulationManager->cellSettingsMaster->hgtTransferLength;
    return transferLength;
}

//PG- make mask contain the transfer segment from donor genome
void HgtSystem::generateTransfer(quint32* donorGenome, quint32* mask, quint32* maskofdonor)
{
    int maskWord=0;
    //- blank out donor genome based on the mask words used
    for (int i=0 ; i < simulationManager->simulationSettings->genomeSize; i++)
    {
        maskofdonor[i] = 0;
        if (returnGenomeWordInUse(i) && (maskWord<returnUseGenomeWordsCount()))
        {
            maskofdonor[i] = ~(mask[maskWord]) & donorGenome[i];
            maskWord++;
        }
    }
}

//PG- transform organism genome to include transfer segement
void HgtSystem::transformRecipient(quint32* genome, quint32* donorGenome, quint32* mask, quint32* maskofdonor)
{
    if ((simulationManager->simulationSettings->variableHgtId || simulationManager->simulationSettings->hgtId))
    {
        quint32 maskofgenome[simulationManager->simulationSettings->genomeSize];
        // TURN INTO FUNCTION AS REPEATED ABOVE
        int maskWord =0;
        for (int i=0; i < simulationManager->simulationSettings->genomeSize; i++)
        {
            if (returnGenomeWordInUse(i) && (maskWord<returnUseGenomeWordsCount()))
            {
                maskofgenome[i] = genome[i] & ~(mask[maskWord]);
                maskWord++;
            }
        }
        if (simulationManager->variableHgtIdSystem->tryTransform(genome, maskofgenome, maskofdonor)) transform(genome, mask, maskofdonor);
    } else {
            transform(genome, mask, maskofdonor);
    }
}


void HgtSystem::transform(quint32* genome, quint32* mask, quint32* mask)
{
    if (simulationManager->simulationSettings->hgtMode==HGT_NON_SYNOYMOUS)
    {
        shift(genome, mask, donorGenome);
    }
    // copy into the recipent genome, blank out target area with mask then or with the transfer genome
    int maskWord = 0;
    for (int i = 0; i < simulationManager->simulationSettings->genomeSize; i++)
    {
        if (returnGenomeWordInUse(i) && (maskWord<returnUseGenomeWordsCount()))
        {
            genome[i] = (genome[i] & mask[maskWord])| donorGenome[i];
            maskWord++;
        }
    }
}


void HgtSystem::shift(quint32* mask, quint32* donorGenome)
{
    //- if non-synomous transfer segment is either shifted left or right so it's put into the recipent genome in a different position
    if (simulationManager->simulationSettings->hgtMode==HGT_NON_SYNOYMOUS)
    {
        //- decide movement direction
        quint32 leftShift = simulationManager->simulationRandoms->rand32()% 2;
        int maskWord = 0;

        for (int i = 0; i < simulationManager->simulationSettings->genomeSize; i++)
        {
            if (returnGenomeWordInUse(i) && (maskWord<returnUseGenomeWordsCount()))
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
