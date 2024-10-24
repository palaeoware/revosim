#include "hgtsystem.h"
#include "simmanager.h"

HgtSystem::HgtSystem() : System("HGT System")
{

}


bool HgtSystem::willTransform()
{
    // determine if HGT will occur
    int transformNum = 1;
    int transformDem = QRandomGenerator::global()->bounded(0, simulationManager->cellSettingsMaster->hgtTransferChance); //probability of transformation- need to think about
    if (transformNum == transformDem)
        {
            return true;
        }
    else return false;
}

int HgtSystem::generateTransferLength()
{
    // set transfer length from user settings or generate random transfer length
    int transferlength=0;

    if (!simulationManager->simulationSettings->hgtrandomlength)
    {
        transferlength= simulationManager->cellSettingsMaster->hgtTransferLength;

    }
    else {
        //*************Need to fix the random length
        // transferlength= simulationManager->simulationRandoms->rand32() % simulationManager->simulationSettings->genomeSize*32;
        transferlength= QRandomGenerator::global()->bounded(0,simulationManager->simulationSettings->genomeSize*32);
    }
    return transferlength;
}

quint32 HgtSystem::generateMask(quint32 mask)
{
    //generate transfer genome mask from donor genome and mask of target area used in transformation
    int genomelength = simulationManager->simulationSettings->genomeSize*32; // check hgt applies to all the words
    int transferlength= generateTransferLength();
    int startpostion= QRandomGenerator::global()->bounded(0,simulationManager->simulationSettings->genomeSize*32- transferlength);

    mask= (mask >> (genomelength - transferlength));
    mask= mask << (genomelength - (transferlength + startpostion));

    return mask;
}



quint32 HgtSystem::GenerateTransform(quint32 genometransfer, quint32 mask)
{
    //make mask contain the transfer segment from donor genome
    if (simulationManager->simulationSettings->hgtMode == HGT_SYNOYMOUS)
    {
        genometransfer = mask & genometransfer;
    }
    else
    {
        mask = mask >> 1;
        genometransfer = mask & genometransfer;
    }
    return genometransfer;
}


void HgtSystem::Transform(quint32* genome, quint32 genometransfer, quint32 mask)
{
  //transform organism genome to include transfer segement

    *genome = (*genome & ~(mask))|genometransfer;

    // if (simulationManager->iteration%100 == 0) {
    //     qDebug() << "\n 1 " <<  simulationManager->printGenome(*genome) << simulationManager->printGenome(mask) << simulationManager->printGenome(genometransfer);
    //     quint32 x =(*genome & ~(mask));
    //     x = x|genometransfer;
    //     qDebug() << simulationManager->printGenome(*genome);
    // }
}
