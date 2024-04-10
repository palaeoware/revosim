#include "hgtsystem.h"
#include "simmanager.h"

HgtSystem::HgtSystem() : System("HGT System")
{

}


bool HgtSystem::willTransform()
{
    // determine propbability that HGT will occur
    int transformNum = 1;
    int transformDem= QRandomGenerator::global()->bounded(0,2000); //probability of transformation- need to think about
    if (transformNum == transformDem)
        {
            return true;
        }
    else return false;
}

int HgtSystem::generatetransferLength()
{
    // generate random transfer length if set or use user defined transfer length
    int transferlength=0;
    if (simulationManager->simulationSettings->hgtrandomlength)
    {
        transferlength= QRandomGenerator::global()->bounded(0,simulationManager->simulationSettings->genomeSize*32); // length of transfer segment
    }
    else
    {
        transferlength= simulationManager->cellSettingsMaster->hgtTransferLength;// length of transfer segment set by the user
    }
    return transferlength;
}

quint32 HgtSystem::GenerateMask(quint32 mask)
{
    //generate transfer genome mask from donor genome and mask of target area used in transformation
    int genomelength = simulationManager->simulationSettings->genomeSize*32; // check hgt applies to all the words
    int transferlength= generatetransferLength();
    int startpostion= QRandomGenerator::global()->bounded(0,simulationManager->simulationSettings->genomeSize*32- transferlength); // starting position from transfer genome
    mask= (mask >> (genomelength - transferlength));
    // qDebug() << mask << genomelength << startpostion << transferlength; -used to check
    mask= mask << (genomelength - (transferlength + startpostion)); //mask of transfer length
    //qDebug() << mask <<  genomelength << startpostion << transferlength; -used to check
    return mask;
}


quint32 HgtSystem::GenerateTransform(quint32 genometransfer, quint32 mask)
{
    // //generate the transfer genome
    // if (simulationManager->simulationSettings->hgtMode == HGT_SYNOYMOUS)
    // {
    //     genometransfer = mask & genometransfer;
    // }

    // else
    // {
    //     mask = mask >> 1;
    //     genometransfer = mask & genometransfer;
    // }

    // //qDebug() << mask << genometransfer;  -used to check
    // return genometransfer;
    genometransfer = mask & genometransfer;
    return genometransfer;

}


void HgtSystem::Transform(quint32* genome, quint32 genometransfer, quint32 mask)
{
  //transform organism genome to include transfer segement
   *genome = (*genome & ~(mask))|genometransfer;

    // //qDebug() << x <<  *genome << mask << genometransfer << "1" ;
    // x =(*genome & ~(mask));
    // //qDebug() << x << *genome << "2" ;
    // x = x|genometransfer;
    // //qDebug() << x << *genome << "3" ;
}
