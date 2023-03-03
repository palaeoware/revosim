#include "environmentfitnesssystem.h"
#include "simmanager.h"
#include <cstring>

EnvironmentFitnessSystem::EnvironmentFitnessSystem() : System("Environment Fitnesss System")
{
    //RJG - set up xor masks for 3 variables
    //Used for each of R G and B to work out fitness
    //Start with random bit pattern for each
    xorMasks[0][0] = QRandomGenerator::global()->generate();
    xorMasks[0][1] = QRandomGenerator::global()->generate();
    xorMasks[0][2] = QRandomGenerator::global()->generate();

    //RJG - to populate, flip a random bit each time. Will slowly modify from 0 to 255
    for (int n = 1; n < 256; n++)
    {
        xorMasks[n][0] = xorMasks[n - 1][0] ^ tweakers[QRandomGenerator::global()->bounded(32)];
        xorMasks[n][1] = xorMasks[n - 1][1] ^ tweakers[QRandomGenerator::global()->bounded(32)];
        xorMasks[n][2] = xorMasks[n - 1][2] ^ tweakers[QRandomGenerator::global()->bounded(32)];
    }


}

quint32 EnvironmentFitnessSystem::xorAndCount(quint32 genomeWord, const quint8 env, const int colour)
{
    return bitCount(xorMasks[env][colour] ^ genomeWord);
}

quint32 EnvironmentFitnessSystem::calculateFitness(const quint32 *genome, const quint8 *environment)
{
    quint32 bitcount = 0;
    for (int i = 0; i < useGenomeWordsCount; i++)
    {
        quint32 genomeWord = genome[useGenomeWords[i]];

        //xor and bitcount for R,G,B
        bitcount += xorAndCount(genomeWord, environment[0], 0);
        bitcount += xorAndCount(genomeWord, environment[1], 1);
        bitcount += xorAndCount(genomeWord, environment[2], 2);
    }
    return bitcount;
}


//find something that works - return false if failed
bool EnvironmentFitnessSystem::findDualViableGenome(quint32 *genome, const quint8 *environment, const quint8 *environment2, int target, int settleTolerance)
{
    quint32 newGenome[MAX_GENOME_WORDS];
    for (int i = 0; i < MAX_GENOME_WORDS; i++)newGenome[i] = 0;

    int geneWords = simulationManager->simulationSettings->genomeSize;
    for (int i = 0; i < 500; i++)
    {
        if (!findRandomViableGenome(genome, environment, target, settleTolerance)) return false;
        else
        {
            //Start with one that will live in the first environment
            for (int j = 0; j < geneWords; j++) newGenome[j] = genome[j] ;
            int baseScore1, baseScore2;
            for (int j = 0; j < 500; j++)
            {
                baseScore1 = scoreForFind(genome, environment, target);
                baseScore2 = scoreForFind(genome, environment2, target);
                doMutation(genome, newGenome, geneWords);
                int newScore1 = scoreForFind(newGenome, environment, target);
                int newScore2 = scoreForFind(newGenome, environment2, target);
                if (newScore1 < baseScore1 + 2 && newScore2 < baseScore2) //lower is better - so use new one
                    std::memcpy(genome, newGenome, 4 * geneWords);
            }
            if (baseScore1 < settleTolerance && baseScore2 < settleTolerance)
                return true; //found!
        }
    }
    return false;
}


//find something that works - return false if failed
bool EnvironmentFitnessSystem::findRandomViableGenome(quint32 *genome, const quint8 *environment, int target, int settleTolerance)
{
    quint32 newGenome[MAX_GENOME_WORDS];
    //RJG - prevent anything > genomeSize being uninitialised
    for (int i = 0; i < MAX_GENOME_WORDS; i++)newGenome[i] = 0;
    int geneWords = simulationManager->simulationSettings->genomeSize;

    if (simulationManager->simulationSettings->randomReseedBeforeGenetic)
    {
        //random start point
        for (int j = 0; j < geneWords; j++) newGenome[j] = genome[j] = simulationManager->simulationRandoms->rand32();
        int score = scoreForFind(genome, environment, target);
        if (score < settleTolerance) return true;
    }

    for (int i = 0; i < 100000; i++) //random seeds
    {
        //random start point
        for (int j = 0; j < geneWords; j++)
            newGenome[j] = genome[j] = simulationManager->simulationRandoms->rand32();

        int baseScore;
        for (int j = 0; j < 500; j++)
        {
            baseScore = scoreForFind(genome, environment, target);
            doMutation(genome, newGenome, geneWords);
            int newScore = scoreForFind(newGenome, environment, target);

            if (newScore < baseScore) //lower is better - so use new one
                std::memcpy(genome, newGenome, 4 * geneWords);
        }
        if (baseScore < settleTolerance)
            return true; //found!
    }
    return false;
}

//score for findRandomViableGenome
int EnvironmentFitnessSystem::scoreForFind(quint32 *genome, const quint8 *environment, int target)
{
    int v = target - calculateFitness(genome, environment);
    return qAbs(v);
}

//mutation for findRandomViableGenome - copying old to new
void EnvironmentFitnessSystem::doMutation(quint32 *genome, quint32 *newGenomes, int geneWords)
{
    int word = simulationManager->simulationRandoms->rand32() % geneWords;
    int bit = simulationManager->simulationRandoms->rand8() & 31;
    for (int i = 0; i < geneWords; i++) newGenomes[i] = genome[i];
    newGenomes[word] ^= tweakers[bit];
}

quint32 EnvironmentFitnessSystem::returnXOR(int n, int m)
{
    return xorMasks[n][m];
}

void EnvironmentFitnessSystem::setXOR(int n, int m, quint32 number)
{
    xorMasks[n][m] = number;
}
