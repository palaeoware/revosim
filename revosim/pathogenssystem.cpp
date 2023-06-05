#include "pathogenssystem.h"
#include "simmanager.h"

PathogensSystem::PathogensSystem() : System("Pathogen System")
{
    reset();
}

void PathogensSystem::reset()
{
    //RJG - define new pathogens here for now
    for (int i = 0; i < GRID_X; i++)
        for (int j = 0; j < GRID_Y; j++)
            for (int k = 0; k < PATH_DEPTH; k++)
            {
                newpathogens[i][j][k] = new quint32[MAX_GENOME_WORDS];
                //RJG - This is called from simmanager constructor, so simmanager randoms not set up yet.
                //For ease, just call QRandomGenerator...
                for (int l = 0; l < MAX_GENOME_WORDS; l++) newpathogens[i][j][k][l] = QRandomGenerator::global()->generate();
            }
}

bool PathogensSystem::willDie(const quint32 *genome, int n, int m, int c)
{
    //RJG - get bitcounts for the words assigned to this system
    quint32 bitcount = 0;
    quint32 *pathogens = newpathogens[n][m][c];

    for (int i = 0; i < useGenomeWordsCount; i++)
    {
        //xr = crit[c].genome ^ pathogens[n][m][0];
        quint32 genomeWord = genome[useGenomeWords[i]];
        quint32 xr = genomeWord ^ pathogens[useGenomeWords[i]];
        bitcount += bitCount(xr);
    }

    if (bitcount >= static_cast<quint32>(pathogenProbabilityDistribution.length()))
    {
        qInfo() << "Error - returning false in pathogens";
        return false;
    }
    //RJG - probability of breeding follows the standard normal distribution from -3 to +3 ceated above
    //More 1's in non coding genome == higher probability of sexual reproduction - see documentation.
    return (simulationManager->simulationRandoms->rand32() >= pathogenProbabilityDistribution[bitcount]);
}

//Mutate pathogens if in drift mode
void PathogensSystem::mutate()
{

    for (int n = 0; n < simulationManager->simulationSettings->gridX; n++)
        for (int m = 0; m < simulationManager->simulationSettings->gridY; m++)
        {
            //----RJG: User defined prob of mutation each iteration
            if (simulationManager->simulationRandoms->rand8() < simulationManager->cellSettings[n][m].pathogenMutate)
                //----RJG: Flip a bit.
            {
                int word = simulationManager->simulationRandoms->rand32() % useGenomeWordsCount;
                int bit = simulationManager->simulationRandoms->rand8() & 31;
                newpathogens[n][m][0][useGenomeWords[word]] ^= tweakers64[bit];
            }
        }
}

void PathogensSystem::replicate(int nlocal, int mlocal, int n, int m, int c)
{
    quint32 *babyPathogen =  new quint32[MAX_GENOME_WORDS];
    //RJG - Put contents of the selected pathogen into our baby
    *babyPathogen = *newpathogens[nlocal][mlocal][c];

    //For the moment, batch the mutate system - mutate a single bit irrespective of genome length if required
    if (simulationManager->simulationRandoms->rand8() < simulationManager->cellSettingsMaster->pathogenMutate)
    {
        int word = simulationManager->simulationRandoms->rand32() % useGenomeWordsCount;
        int bit = simulationManager->simulationRandoms->rand8() & 31;
        babyPathogen[useGenomeWords[word]] ^= tweakers[bit];
    }
    int newDepth = QRandomGenerator::global()->bounded(PATH_DEPTH);
    //RJG - free memory to avoid leaks
    delete (newpathogens[n][m][newDepth]);
    //RJG - point to baby
    newpathogens[n][m][newDepth] = babyPathogen;
}

QColor PathogensSystem::returnRGB(int n, int m, int word)
{
    //RJG - data structures
    quint32 genomes[PATH_DEPTH];
    int counts[PATH_DEPTH];
    int arraypos = 0; //pointer
    //RJG - genome for our critter
    quint32 genome;

    if (word >= useGenomeWordsCount)word = useGenomeWordsCount - 1;

    if (simulationManager->simulationSettings->pathogenMode == PATH_MODE_DRIFT) //RJG - if we're drifting, just use the first slot
        genome = newpathogens[n][m][0][useGenomeWords[word]];

    else  //RJG - otherwise find modal genome
    {
        for (int p = 0; p < PATH_DEPTH; p++)
        {
            quint32 g = newpathogens[n][m][p][useGenomeWords[word]];
            for (int i = 0; i < arraypos; i++)
            {
                if (genomes[i] == g)   //found it
                {
                    counts[i]++;
                    goto gotcounts;
                }
            }
            //didn't find it
            genomes[arraypos] = g;
            counts[arraypos++] = 1;
gotcounts:
            ;
        }

        //RJG - find max frequency
        int max = -1;
        quint64 maxg = 0;
        for (int i = 0; i < arraypos; i++)
            if (counts[i] > max)
            {
                max = counts[i];
                maxg = genomes[i];
            }

        genome = static_cast<quint32>((maxg & (static_cast<quint64>(65536) * static_cast<quint64>(65536) - static_cast<quint64>(1))));
    }

    //now convert first 32 bits to a colour
    // r,g,b each counts of 11,11,10 bits
    quint32 b = bitCounts[genome & 2047] * 23;
    genome /= 2048;
    quint32 g = bitCounts[genome & 2047] * 23;
    genome /= 2048;
    quint32 r = bitCounts[genome] * 25;
    return QColor(static_cast<int>(r), static_cast<int>(g), static_cast<int>(b));
}

// RJG - This modified the inhereted function in order to redo the probability distribution when genome length is changed
bool PathogensSystem::setGenomeWordsFromString(QString s, int maxsize)
{
    bool returnValue = System::setGenomeWordsFromString(s, maxsize);
    redoProbabilityDistribution();
    return returnValue;
}

// RJG - This is called from above, so is updated to match the required gene countwithout us having to worry about it any more
void PathogensSystem::redoProbabilityDistribution()
{
    //RJG - fill pathogen probability distribution as required so pathogens can kill critters
    //Start with linear, may want to change down the line.
    pathogenProbabilityDistribution.clear();
    quint32 length = static_cast<quint32>(useGenomeWordsCount * 32);
    for (quint32 cnt = 0; cnt < length; cnt++) pathogenProbabilityDistribution.append((4294967296 / 2) + (cnt * (4294967295 / (length * 2))));
}
