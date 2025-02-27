#include "hgtsystem.h"
#include "simmanager.h"

HgtSystem::HgtSystem() : System("HGT System")
{

}

//PG- determine if HGT will occur from user-set probability in GUI
bool HgtSystem::willTransform()
{
    quint64 transformChance = simulationManager->simulationRandoms->rand64 () % simulationManager->cellSettingsMaster->hgtTransferChance;
    return (transformChance == 1);
}


//PG- generate transfer genome mask for donor genome with bits set in transformation position
void HgtSystem::generateMask(quint32* genome, quint32* mask)
{
    //- set max transferable length based on the transformable words set in the GUI
    quint32 genomeTransformableLength = useGenomeWordsCount * 32;

    //- create mask with same number of elements as transformable genome words, all bits set
    for (int i =0; i < useGenomeWordsCount; i++ )
    {
        mask[i] = ~(0);
    }

    quint32 transferLength = generateTransferLength(genome); //- determine transfer length based on settings
    quint32 startPosition = simulationManager->simulationRandoms->rand32() % genomeTransformableLength; //- select random start position betwee 0 and transformable genome length

    //- set the mask based on start position and transfer length. Transferable genome is treated as circular and allows the transfer segment to wrap past genome word
    bool wraps = (startPosition + transferLength > genomeTransformableLength); //- determine if the transfer segement extends past the start genome word
    quint32 wrappedLength = (startPosition + transferLength) %  genomeTransformableLength; //- determine the length of wrapping

    //- iterate through the genome, flip bits to create a mask the length of the transfer segment from the start position
    for (int i=0 ; i < useGenomeWordsCount; i++)
    {
        for (int j = 0; j < 32; j++)
        {
            quint32 index = (j + (i * 32)) % genomeTransformableLength;
            if ((index >= startPosition && !wraps && index < startPosition + transferLength) || // no wrapping and bit position within transfer segment
                (index >= startPosition && wraps && index < genomeTransformableLength) || //  wrapping and bit position greater or equal to start position
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
void HgtSystem::generateTransfer(quint32* donorGenome, quint32* positionMask, quint32* donorMask)
{
    //- copy the the transfer segment from the donor genome into the donor mask
    int maskWord=0;
    for (int i=0 ; i < simulationManager->simulationSettings->genomeSize; i++)
    {
        if (returnGenomeWordInUse(i) && (maskWord<returnUseGenomeWordsCount()))
        {
            donorMask[maskWord] = ~(positionMask[maskWord]) & donorGenome[i];
            maskWord++;
        }
    }
}


//PG- transform organism genome to include transfer segement
void HgtSystem::transformRecipient(quint32* genome, quint32* donorGenome, quint32* positionMask, quint32* donorMask)
{
    //- if variable hgt id or max difference to HGT is enforced
    if ((simulationManager->simulationSettings->variableHgtId || simulationManager->simulationSettings->hgtId))
    {
        quint32 homologyMask[simulationManager->simulationSettings->genomeSize]; //- make new mask for id check
        int maskWord =0;

        for (int i=0; i < simulationManager->simulationSettings->genomeSize; i++)
        {
            if (returnGenomeWordInUse(i) && (maskWord<returnUseGenomeWordsCount()))
            {
                homologyMask[maskWord] = ~(positionMask[maskWord])  & genome[i]; //- make the id mask of the recipent genome in the position of the transfer
                maskWord++;
            }
        }
        if (simulationManager->variableHgtIdSystem->tryTransform(genome, homologyMask, donorMask)) transform(genome, positionMask, donorMask); //- if within max difference then transform the recipient genome
    }
    //- if max difference not enforced, transform the recipent genome
    else
    {
        transform(genome, positionMask, donorMask);
    }
}


//PG- do the transformation
void HgtSystem::transform(quint32* genome, quint32* positionMask, quint32* donorMask)
{
    //- if shifted transformation selected, shift the position mask and the donor mask
    if (simulationManager->simulationSettings->hgtMode==HGT_NON_SYNOYMOUS)
    {
        shift(positionMask, donorMask);
    }

    // copy into the recipent genome by blank out target area with mask then or with the donor mask with the transfer segment.
    int maskWord = 0;
    for (int i = 0; i < simulationManager->simulationSettings->genomeSize; i++)
    {
        if (returnGenomeWordInUse(i) && (maskWord<returnUseGenomeWordsCount()))
        {
            genome[i] = (genome[i] & positionMask[maskWord])|donorMask[maskWord];
            maskWord++;
        }
    }
}

//PG- shift the transferable segment and mask to allow non-homologous transformation
void HgtSystem::shift(quint32* positionMask, quint32* donorMask)
{
    quint32 leftShift = simulationManager->simulationRandoms->rand32() % 2;

    quint32 tempDonorMask[useGenomeWordsCount];
    quint32 tempPositionMask[useGenomeWordsCount];

    for (int i = 0; i < useGenomeWordsCount; i++)
    {
        int j =0;
        if (leftShift)
        {
            if (i != useGenomeWordsCount -1) j = i + 1;  // make j the next genome word, unless last genome word where j stays 0
            tempDonorMask[i] = circularLeftShift(donorMask[i], donorMask[j]); // shift genome transfer
            tempPositionMask[i] = circularLeftShift(positionMask[i], positionMask[j]); // shift mask (used to blank area of transfer in next step)
        } else {                                             //reverse of left shift
            if (i != 0) j = i - 1; else j = useGenomeWordsCount -1; // make j the previous genome word, unless genomeword[0], then j last word of the mask
            tempDonorMask[i] = circularRightShift(donorMask[i], donorMask[j]); // shift genome transfer
            tempPositionMask[i] = circularRightShift(positionMask[i], positionMask[j]); // shift mask
        }
    }

    //- copy the temporary back into the masks
    for (int i = 0; i < useGenomeWordsCount; i++)
    {
        donorMask[i] = tempDonorMask[i];
        positionMask[i] = tempPositionMask[i];
    }
}

quint32 HgtSystem::circularRightShift(quint32 currentWord, quint32 previousWord)
{
    quint32 carryBit = (previousWord & 1) << 31; // take first bit from previous word
    return (currentWord >> 1) | carryBit; // shift the current word, and place with bit from previous word in final position
}

quint32 HgtSystem::circularLeftShift(quint32 currentWord, quint32 nextWord)
{
    quint32 carryBit = (nextWord >> 31) & 1; // take last bit from previous word
    return (currentWord << 1) | carryBit; // shift the current word, and place with bit from previous word in first position
}





