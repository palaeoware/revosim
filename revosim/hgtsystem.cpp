#include "hgtsystem.h"
#include "simmanager.h"

HgtSystem::HgtSystem() : System("HGT System")
{

}


bool HgtSystem::willTransform()
{
    // PG- determine if HGT will occur if chance preset from GUI
    quint64 transformDem = simulationManager->simulationRandoms->rand64 () % simulationManager->cellSettingsMaster->hgtTransferChance;
    return (transformDem == 1);
}

int HgtSystem::generateTransferLength(quint32* genome)
{
    // PG- set transfer length from user settings - either random, variable or preset from GUI
    quint64 transferlength=0;

    if (simulationManager->simulationSettings->hgtrandomlength) transferlength= simulationManager->simulationRandoms->rand64() % 32;
    else if (simulationManager ->simulationSettings->hgtvariable) transferlength = simulationManager->variableHgtLenSystem->transferLength(genome);
    else  transferlength= simulationManager->cellSettingsMaster->hgtTransferLength;
    return transferlength;
}

quint64 HgtSystem::generateMask(quint32* genome)
{
    //PG- generate transfer genome mask from donor genome and mask of target area used in transformation
    quint32 mask =~(0); //mask genomes of 1s
    int transferlength= generateTransferLength(genome);

    //qDebug() << transferlength ;
    int startposition= simulationManager->simulationRandoms->rand32() % (32 - transferlength);

    // need to think more about edge effects (maybe once in the in the genome and wrapped around)
    //think about it array of 2 64 numbers (make sure go through in the right direction) and (limit transfer to 64 bits)

    mask= (mask >> (32 - transferlength));

    mask= mask << (32 - (transferlength + startposition));
    return mask;
}


quint32 HgtSystem::GenerateTransform(quint32 genometransfer, quint32 mask)
{
    //PG- make mask contain the transfer segment from donor genome
    //if NONSYNOYMOUS shift mask and therefore transfer position
    if (simulationManager->simulationSettings->hgtMode == HGT_SYNOYMOUS) genometransfer = mask & genometransfer;
    else
    {
        mask = mask >> 1;
        // bad- introduce directional effect (make random direction would be beter) maybe get rid of the random bitshift)
        // maybe get rid of bitshift, keep random place and same place

        genometransfer = mask & genometransfer;
    }
    return genometransfer;
}


void HgtSystem::Transform(quint32* genome, quint32 genometransfer, quint32 mask)
{
  //transform organism genome to include transfer segement
   *genome = (*genome & ~(mask)) | genometransfer;


    // if (simulationManager->iteration%100 == 0) {
    //     quint32 x =(*genome & ~(mask));
    //     x = x|genometransfer;
    //     qDebug() << simulationManager->printGenome(*genome) << simulationManager->printGenome(genometransfer) << simulationManager->printGenome(x);
    // }
}
