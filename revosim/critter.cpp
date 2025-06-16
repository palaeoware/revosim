/**
 * @file
 * Critter
 *
 * All REvoSim code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All REvoSim code is Copyright 2008-2019 by Mark D. Sutton, Russell J. Garwood,
 * and Alan R.T. Spencer.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version. This program is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY.
 */

#include "critter.h"
#include "simmanager.h"

/*!
 * \brief Critter::Critter
 */
Critter::Critter()
{
    age = 0; //this is the tested flag for dead
    fitness = 0;
    environmentalFitness = 0;
    genomeBitCountWithJitter = -1;
    codingGenomeBitCount = -1;
    energy = 0;
    lifetimeEnergy = 0;
    stolenEnergy = 0;
    speciesID = -1; //=not assigned
    trophicLevel = 0;

    genomeWords = nullptr;

    //Temporary variable breed tag. Probably better way of doing this but short on time
    int variableBreedAsex = 0;
    Q_UNUSED(variableBreedAsex);
}

/*!
 * \brief Critter::initialise
 * \param iteration
 * \param environment
 * \param x
 * \param y
 * \param z
 * \param species
 */
void Critter::initialise(quint32 *multiwordGenome, quint8 *environment, int x, int y, int z, quint64 species, quint64 trophic)
{
    //Restart a slot - set up properly

    //Assign genome memory if needed - done once at most for each critter
    //MIGHT introduce memory fragmentation - if needbe could do this from a
    //central memory pool

    if (genomeWords == nullptr) genomeWords = new quint32[MAX_GENOME_WORDS];
    //RJG - prevent values present uninitialised
    for (int i = 0; i < MAX_GENOME_WORDS; i++)genomeWords[i] = 0;
    for (int i = 0; i < simulationManager->simulationSettings->genomeSize; i++) genomeWords[i] = multiwordGenome[i];

    age = simulationManager->cellSettings[x][y].startAge;

    //RJG - start with 0 energy
    energy = 0;
    stolenEnergy = 0;
    lifetimeEnergy = 0;

    settings = &(simulationManager->cellSettings[xPosition][yPosition]);

    //RJG - Work out fitness
    calculateFitness(environment, 0, x,y);

    xPosition = x;
    yPosition = y;
    zPosition = z;

    speciesID = species;
    trophicLevel = trophic / 100;
}

//Convenience function to call bitcounting with shift in the bitcount system
void Critter::calculateBitCountWithJitter()
{
    genomeBitCountWithJitter = simulationManager->bitCountSystem->calculateBitCountWithJitter(genomeWords);
}

/*!
 * \brief Critter::returnRecombination
 * \return
 */
int Critter::returnRecombination()
{
    return variableBreedAsex;
}

//Calculcate fitness based upon random number of games with a random opponent
int Critter::PrisonersDilemmaFitness(NeuralNet *net, int x, int y)
{
    float weightsMe[100]; //TODO - better max for weights
    float weightsThem[100];
    float inputsMe[10];
    float inputsThem[10];

    //First, pick an opponent

    int target_slot;
    if (simulationManager->simulationRandoms->rand8()%2 == 1)
        target_slot = (zPosition + 1) % simulationManager->cellSettingsMaster->slotsPerSquare;
    else
    {
        target_slot = zPosition -1;
        if (target_slot<0) target_slot = simulationManager->cellSettingsMaster->slotsPerSquare-1;
    }

    if (critters[xPosition][yPosition][target_slot].age ==0
        || (simulationManager->cellSettingsMaster->interactWithinSpecies && critters[xPosition][yPosition][target_slot].speciesID != speciesID))
    {
        //If opponent is dead, or it's a different species - can't do game, return a default fitness
        return 5;  //TODO - configurable default

    }

    partersFound[x][y]+=1;
    int maxGames = 50, minGames = 5; //TODO  - make configuarable

    //We have an opponent. Now we run a certain number of PD games against this opponent
    int gameCount = simulationManager->simulationRandoms->rand32() % (maxGames-minGames) + minGames;

    //Get neural net weights
    simulationManager->neuralNetWeightsSystem->GetWeightsFromGenome(weightsMe, genomeWords);
    simulationManager->neuralNetWeightsSystem->GetWeightsFromGenome(weightsThem, critters[xPosition][yPosition][target_slot].genomeWords);

    inputsMe[0]=1; //input0 is static always 1
    inputsThem[0]=1;

    //Zero out other inputs (0 = no result)
    for (int i=2; i<10; i++)
    {
        inputsMe[i]=0;
        inputsThem[i]=0;
    }

    //Matrix to use is:
        //Both cooperate: 3
        //Both defect: 1
        //Defect v Coop: 5
        //Coop v Defect: 0

    float myPayOff = 0;
    int defectCount = 0;
    for (int gameNumber = 0; gameNumber < gameCount; gameNumber++)
    {
        //Input 1 is RNG, implemented as -1 to +1 range
        inputsMe[1]= (float)(((int)(simulationManager->simulationRandoms->rand32() % 1024)-512))/512.0;
        inputsThem[1]= (float)(((int)(simulationManager->simulationRandoms->rand32() % 1024)-512))/512.0;

        //treat cooperate as my output > .4
        float myNet = net->Evaluate(weightsMe, inputsMe);
        float theirNet = net->Evaluate(weightsThem, inputsThem);


        bool meCooperate = (myNet > 0.4);
        bool theyCooperate = (theirNet > 0.4);

        if (!meCooperate) defectCount++;

        //Correct matrix
        if (meCooperate && theyCooperate) myPayOff += 3;
        if (!meCooperate && !theyCooperate) myPayOff += 1;
        if (meCooperate && !theyCooperate) myPayOff += 0;
        if (!meCooperate && theyCooperate) myPayOff += 5;

        //Test matrix
        //if (meCooperate && theyCooperate) myPayOff += 3;
        //if (!meCooperate && !theyCooperate) myPayOff += 0;
        //if (meCooperate && !theyCooperate) myPayOff += 3;
        //if (!meCooperate && theyCooperate) myPayOff += 0;

        //if (x==50 && y==50 && (myNet!=0.5 || theirNet!=0.5))
         //   qDebug()<<"Me: "<<myNet<<" Them: "<<theirNet<<" MeC:"<<meCooperate<<" ThC"<<theyCooperate<<" my payoff "<<myPayOff;
        //Move games down list and insert new results
        inputsMe[9] = inputsMe[7];
        inputsMe[8] = inputsMe[6];
        inputsThem[9] = inputsThem[7];
        inputsThem[8] = inputsThem[6];

        inputsMe[7] = inputsMe[5];
        inputsMe[6] = inputsMe[4];
        inputsThem[7] = inputsThem[5];
        inputsThem[6] = inputsThem[4];

        inputsMe[5] = inputsMe[3];
        inputsMe[4] = inputsMe[2];
        inputsThem[5] = inputsThem[3];
        inputsThem[4] = inputsThem[2];

        //new results
        if (meCooperate) inputsMe[2] = 1; else inputsMe[2]=-1;
        if (theyCooperate) inputsMe[3] = 1; else inputsMe[3]=-1;

        //and the other matrix, but flip
        if (meCooperate) inputsThem[3] = 1; else inputsThem[3]=-1;
        if (theyCooperate) inputsThem[2] = 1; else inputsThem[2]=-1;

    }
    float meanPayoff = (myPayOff/(float)gameCount);
    meanScore[x][y]+=meanPayoff;
    proportionDefect[x][y]+= (float)defectCount/(float)gameCount;
    return meanPayoff * 10.0; //a fudge factor
}

/*!
 * \brief Critter::calculateFitness
 * \param environment
 * \return
 */
int Critter::calculateFitness(const quint8 *environment, int parallelIndex, int x, int y)
{

    calculateBitCountWithJitter(); // ENF Called here so that it is updated for breed lists later. Not related to bit counting in fitness algorithm.
    //RJG if no selection is enabled, just return max fitness.
    if (settings->noSelection)
    {
        fitness = settings->settleTolerance;
        environmentalFitness = fitness;
        return fitness;
    }

    if (settings->prisonersDilemmaFitness && neuralNets.count()>0)
    {
        //qDebug()<<parallelIndex<<"NN count"<<neuralNets.count();
        fitness = PrisonersDilemmaFitness(neuralNets[parallelIndex], x, y);
    }
    else
    {

        int fitnessBitCount = simulationManager->environmentalFitnessSytem->calculateFitness(genomeWords, environment);
        fitness = settings->settleTolerance - qAbs(fitnessBitCount - settings->target);
        if (fitness < 0) fitness = 0;
        if (fitness > settings->settleTolerance) qDebug() << "Oops - fitness too high?";

        environmentalFitness = fitness; // ENF This tracks the non-interaction fitness level for logging and GUI display purposes

        if (settings->interactFitness)
        {
            for (int num_interactions = 0; num_interactions < simulationManager->cellSettingsMaster->interactions; num_interactions++)
            {
                quint32 target_slot = simulationManager->simulationRandoms->rand32() % (simulationManager->cellSettingsMaster->slotsPerSquare);
                if (critters[xPosition][yPosition][target_slot].age && ((critters[xPosition][yPosition][target_slot].speciesID != speciesID)
                                                                        || (simulationManager->cellSettingsMaster->interactWithinSpecies))) // Interacts only if that slot is occupied by a living critter
                {
                    fitness = (simulationManager->interactionSystem->performInteractFitness(genomeWords, critters[xPosition][yPosition][target_slot].genomeWords, fitness, settings->interactBlocks));
                }
            }
        }
    }
    if ((fitness == 0) && !(settings->interactEnergy)) // Kill anything that, after all modifications, has zero fitness (unless we're doing energy interactions).
    {
        age = 0;
    }
    return fitness;
}

/*!
*  \brief Critter::iterateParallel
* \param killCountLocal
* \param addFood
* \return
*/
bool Critter::iterateParallel(int *killCountLocal, float addFood, int settleTolerance)
{
    //RJG - remove compile warning for unused settleTolerance, as in future versions we may need this again.
    (void) settleTolerance;

    if (age)
    {
        //RJG - Here is where an individual dies.
        if ((--age) == 0)
        {
            (*killCountLocal)++;
            genomeBitCountWithJitter = -1;
            codingGenomeBitCount = -1;
            totalFitness[xPosition][yPosition] -= fitness;
            fitness = 0;
            if (maxUsed[xPosition][yPosition] == zPosition)
            {
                for (int n = zPosition - 1; n >= 0; n--)
                    if (critters[xPosition][yPosition][n].age > 0)
                    {
                        maxUsed[xPosition][yPosition] = n;
                        goto past;
                    }
                maxUsed[xPosition][yPosition] = -1;
past:
                ;
            }
            return false;
        }

        if ((fitness * addFood) > 0)
        {
            /*
            energy +=  fitness * fitness * fitness * addFood / settleTolerance / settleTolerance; // Version with nonlinear absolute scaling
            trophicLevel = (trophicLevel * lifetimeEnergy / (lifetimeEnergy + (fitness * fitness * fitness * addFood / settleTolerance / settleTolerance))); // Version with nonlinear absolute scaling
            lifetimeEnergy +=  fitness * fitness * fitness * addFood / settleTolerance / settleTolerance; // Version with nonlinear absolute scaling
            */
            /*
            energy +=  fitness * fitness * addFood / settleTolerance; // Version with linear absolute scaling
            trophicLevel = (trophicLevel * lifetimeEnergy / (lifetimeEnergy + (fitness * fitness * addFood / settleTolerance))); // Version with linear absolute scaling
            lifetimeEnergy +=  fitness * fitness * addFood / settleTolerance; // Version with linear absolute scaling
            */

            energy +=  fitness * addFood; // Purely relative (original) verison
            trophicLevel = (trophicLevel * lifetimeEnergy / (lifetimeEnergy + (fitness * addFood))); // Purely relative (original) verison
            lifetimeEnergy +=  fitness * addFood; // Purely relative (original) verison

        }

        if (energy > (settings->breedThreshold + settings->breedCost))
        {
            energy -= settings->breedCost;
            return true;
        }
        return false;

    }
    return false;
}

/*!
 * \brief Critter::breedWithParallel
 * \param xPosition
 * \param yPosition
 * \param partner
 * \param newGenomeCountLocal
 * \return
 *
 * newGenomeCountLocal is position in genome array - arranged so can never overlap
 * returns 1 if it fails (so I can count fails)
 */
int Critter::breedWithParallel(int xPosition, int yPosition, Critter *partner, int *newGenomeCountLocal)
{
    bool breedsuccess1 = true; //for species restricted breeding
    bool breedsuccess2 = true; //for difference breeding

    if (settings->breedSpecies)
    {
        if (partner->speciesID != speciesID)
            breedsuccess1 = false;
    }

    if (settings->breedDifference)
    {
        breedsuccess2 = simulationManager->breedSystem->tryBreed(genomeWords, partner->genomeWords, simulationManager->simulationSettings->maxDifference);
        /*
        int t1 = 0;
        // - determine success.. use genetic similarity
        quint64 cg1x = genome ^ partner->genome; //XOR the two to compare

        //Coding half
        auto g1xl = quint32(cg1x & ((quint64)65536 * (quint64)65536 - (quint64)1)); //lower 32 bits
        t1 = bitCounts[g1xl / (quint32)65536] +  bitCounts[g1xl & (quint32)65535];

        //non-Coding half
        auto g1xu = quint32(cg1x / ((quint64)65536 * (quint64)65536)); //upper 32 bits
        t1 += bitCounts[g1xu / (quint32)65536] +  bitCounts[g1xu & (quint32)65535];
        if (t1 > simulationManager->simulationSettings->maxDifference)
        {
            breedsuccess2 = false;
        }
        */
    }

    if (breedsuccess1 && breedsuccess2)
    {
        //make memory for genome if not yet assigned
        if (newgenomes[*newGenomeCountLocal] == nullptr) newgenomes[*newGenomeCountLocal] = new quint32[MAX_GENOME_WORDS];

        //work out new genome - from ALL genes (no system for this)
        for (int i = 0; i < simulationManager->simulationSettings->genomeSize; i++)
        {
            //get a random set of 1s from the genex array
            quint32 g1x = genex[nextgenex++];
            if (nextgenex >= 65536) nextgenex = 0;

            //cross breed - take my genome word ANDed with the 1s, and OR it with partners word ANDed with inverse of 1s
            //store straight into the newgenomes array
            newgenomes[*newGenomeCountLocal][i] = (g1x & genomeWords[i]) | (~g1x & partner->genomeWords[i]);
        }

        bool local_mutate = false;

        //this is not threadsafe, but it doesn't matter - any value for nextrand is fine
        if (!settings->variableMutate)
            if ((simulationManager->simulationRandoms->rand8()) < settings->mutate)
                local_mutate = true;

        //RJG - Variable mutate implemented here.
        if (settings->variableMutate)
        {
            if (simulationManager->variableMutateSystem->willMutate(newgenomes[*newGenomeCountLocal]))
                local_mutate = true;
        }

        if (local_mutate) simulationManager->mutationSystem->doMutation(newgenomes[*newGenomeCountLocal]);

        //store it all

        newgenomeX[*newGenomeCountLocal] = xPosition;
        newgenomeY[*newGenomeCountLocal] = yPosition;
        newgenomespecies[*newGenomeCountLocal] = speciesID;
        newgenometrophic[*newGenomeCountLocal] = trophicLevel * 100;
        newgenomeDisp[(*newGenomeCountLocal)++] = settings->dispersal; //how far to disperse - low is actually far (it's a divider - max is 240, <10% are >30
        return 0;
    }
    //breeders get their energy back - this is an 'abort'
    //---- RJG: Note that this refund is different to and exclusive from that in Simmanager, which refunds if no partner found.
    energy += simulationManager->cellSettingsMaster->breedCost;
    return 1;
}

bool Critter::operator == (const Critter &c) const
{
    if (xPosition != c.xPosition) return false;
    if (yPosition != c.yPosition) return false;
    if (zPosition != c.zPosition)return false;
    if (age != c.age)return false;
    if (fitness != c.fitness)return false;
    if (environmentalFitness != c.environmentalFitness)return false;
    if (energy != c.energy)return false;
    if (lifetimeEnergy != c.lifetimeEnergy)return false;
    if (stolenEnergy != c.stolenEnergy)return false;
    if (variableBreedAsex != c.variableBreedAsex)return false;
    if (genomeBitCountWithJitter != c.genomeBitCountWithJitter)return false;
    if (codingGenomeBitCount != c.codingGenomeBitCount)return false;
    for (int i = 0; i < simulationManager->simulationSettings->genomeSize; i++)if (genomeWords[i] != c.genomeWords[i]) return false;
    if (speciesID != c.speciesID) return false;
    return true;
}
