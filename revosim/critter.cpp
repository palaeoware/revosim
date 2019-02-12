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

/**
 * @brief Critter::Critter
 */
Critter::Critter()
{
    genome = 0;
    age = 0; //this is the tested flag for dead
    fitness = 0;
    energy = 0;
    speciesID = -1; //=not assigned
    //speciesID = std::numeric_limits<quint64>::max(); //=not assigned
}

/**
 * @brief Critter::initialise
 * @param iteration
 * @param environment
 * @param x
 * @param y
 * @param z
 * @param species
 */
void Critter::initialise(quint64 iteration, quint8 *environment, int x, int y, int z, quint64 species)
{
    //Restart a slot - set up properly
    genome = iteration;

    age = startAge;
    //RJG - start with 0 energy
    energy = 0;
    //RJG - Work out fitness
    recalculateFitness(environment);

    xPosition = x;
    yPosition = y;
    zPosition = z;
    speciesID = species;
}

/**
 * @brief Critter::recalculateFitness
 * @param environment
 * @return
 */
int Critter::recalculateFitness(const quint8 *environment)
{
    auto lowergenome = static_cast<quint32>(genome & (static_cast<quint64>(65536) * static_cast<quint64>(65536) - static_cast<quint64>(1)));

    quint32 answer = lowergenome ^ xorMasks[environment[0]][0]; //apply redmask
    quint32 a2 = answer / 65536;
    answer &= static_cast<quint32>(65535);

    //RJG - add a counter for final bitcount
    int finalanswer = static_cast<int>(bitCounts[answer]);
    finalanswer += bitCounts[a2];

    answer = lowergenome ^ xorMasks[environment[1]][1]; //apply greenmask
    a2 = answer / 65536;
    answer &= static_cast<quint32>(65535);
    finalanswer += bitCounts[answer];
    finalanswer += bitCounts[a2];

    answer = lowergenome ^ xorMasks[environment[2]][2]; //apply bluemask
    a2 = answer / 65536;
    answer &= static_cast<quint32>(65535);
    finalanswer += bitCounts[answer];
    finalanswer += bitCounts[a2];

    if (finalanswer >= target + settleTolerance) {
        fitness = 0;    // no use
        age = 0;
        return 0;
    }
    if (finalanswer <= target - settleTolerance) {
        fitness = 0;    // no use
        age = 0;
        return 0;
    }

    //These next two SHOULD do reverse of the abs of finalanswer (i.e 0=20, 20=0)
    // RJG - so if settle tolerance is, say, 15, peak fitness will be 15, min will be one, zero will be dead.
    if (finalanswer < target) fitness = settleTolerance - (target - finalanswer);
    else fitness = settleTolerance + target - finalanswer;

    return fitness;
}

/**
 * @brief Critter::iterateParallel
 * @param killCountLocal
 * @param addFood
 * @return
 */
bool Critter::iterateParallel(int *killCountLocal, int addFood)
{
    if (age) {
        //RJG - Here is where an individual dies.
        if ((--age) == 0) {
            (*killCountLocal)++;
            totalFitness[xPosition][yPosition] -= static_cast<quint32>(fitness);
            fitness = 0;
            if (maxUsed[xPosition][yPosition] == zPosition) {
                for (int n = zPosition - 1; n >= 0; n--)
                    if (critters[xPosition][yPosition][n].age > 0) {
                        maxUsed[xPosition][yPosition] = n;
                        goto past;
                    }
                maxUsed[xPosition][yPosition] = -1;
past:
                ;
            }
            return false;
        }
        energy +=  fitness * addFood;
        //energy+= (fitness * food) / totalFitness[xPosition][yPosition];]

        //non-slot version - try breeding if our energy is high enough
        if (energy > (breedThreshold + breedCost)) {
            energy -= breedCost;
            return true;
        }
        return false;

    }
    return false;
}

/**
 * @brief Critter::breedWithParallel
 *
 * newGenomeCountLocal is position in genome array - arranged so can never overlap
 * returns 1 if it fails (so I can count fails)
 *
 * @param xPosition
 * @param yPosition
 * @param partner
 * @param newGenomeCountLocal
 * @return
 */
int Critter::breedWithParallel(int xPosition, int yPosition, Critter *partner, int *newGenomeCountLocal)
{
    bool breedsuccess1 = true; //for species restricted breeding
    bool breedsuccess2 = true; //for difference breeding

    if (breedSpecies) {
        if (partner->speciesID != speciesID)
            breedsuccess1 = false;
    }

    if (breedDifference) {
        int t1 = 0;
        // - determine success.. use genetic similarity
        quint64 cg1x = genome ^ partner->genome; //XOR the two to compare

        //Coding half
        auto g1xl = static_cast<quint32>(cg1x & (static_cast<quint64>(65536) * static_cast<quint64>(65536) - static_cast<quint64>(1))); //lower 32 bits
        t1 = static_cast<int>(bitCounts[g1xl / static_cast<quint32>(65536)] +  bitCounts[g1xl & static_cast<quint32>(65535)]);

        //non-Coding half
        auto g1xu = static_cast<quint32>(cg1x / (static_cast<quint64>(65536) * static_cast<quint64>(65536))); //upper 32 bits
        t1 += bitCounts[g1xu / static_cast<quint32>(65536)] +  bitCounts[g1xu & static_cast<quint32>(65535)];
        if (t1 > maxDifference) {
            breedsuccess2 = false;
        }
    }

    if (breedsuccess1 && breedsuccess2) {
        //work out new genome

        quint64 g1x = geneX[nextGeneX++];
        if (nextGeneX >= 65536) nextGeneX = 0;
        quint64 g2x = ~g1x; // inverse

        g2x &= genome;
        g1x &= partner->genome;  // cross bred genome
        g2x |= g1x;

        bool local_mutate = false;

        //this is technically not threadsafe, but it doesn't matter - any value for nextrand is fine
        if ((simulationManager->random8()) < mutate)
            local_mutate = true;

        if (local_mutate) {
            int w = simulationManager->random8();
            w &= 63;
            g2x ^= tweakers64[w];
        }

        //store it all
        newGenomes[*newGenomeCountLocal] = g2x;
        newGenomeX[*newGenomeCountLocal] = static_cast<quint32>(xPosition);
        newGenomeY[*newGenomeCountLocal] = static_cast<quint32>(yPosition);
        newGenomeSpecies[*newGenomeCountLocal] = speciesID;
        newGenomeDispersal[(*newGenomeCountLocal)++] = dispersal; //how far to disperse - low is actually far (it's a divider - max is 240, <10% are >30
        return 0;
    }
    //breeders get their energy back - this is an 'abort'
    //---- RJG: Note that this refund is different to and exclusive from that in Simmanager, which refunds if no partner found.
    energy += breedCost;

    return 1;
}
