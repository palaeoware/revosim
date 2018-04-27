/**
 * @file
 * Critter
 *
 * All REvoSim code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All REvoSim code is Copyright 2018 by Mark Sutton, Russell Garwood,
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

//undefine to use old genetic distance methods

//#define SLOTS
#define MONEYBACK

Critter::Critter()
{
    genome=0;
    age=0;  //this is the tested flag for dead
    fitness=0;
    energy=0;
    speciesid=-1; //=not assigned

    //Temporary variable breed tag. Probably better way of doing this but short on time
    int variableBreedAsex = 0;
    Q_UNUSED(variableBreedAsex);
}

void Critter::initialise(quint64 gen, quint8 *env, int x, int y, int z, quint64 species)
{
    //Restart a slot - set up properly
    genome=gen;

    age=startAge;
    //RJG - start with 0 energy
    energy=0;
    //RJG - Work out fitness
    recalc_fitness(env);

    xpos=x; ypos=y; zpos=z;
    speciesid=species;

    quint32 gen2 = genome>>32;
    ugenecombo = (gen2>>16) ^ (gen2 & 65535); //for breed testing - work out in advance for speed
}

int Critter::return_recomb()
{
    return variableBreedAsex;
}

int Critter::recalc_fitness(quint8 *env)
{
    quint32 lowergenome=(quint32)(genome & ((quint64)65536*(quint64)65536-(quint64)1));

    quint32 answer=lowergenome ^ xormasks[env[0]][0]; //apply redmask
    quint32 a2=answer/65536;
    answer &= (quint32) 65535;

    //RJG - add a counter for final bitcount
    int finalanswer=bitcounts[answer];
    finalanswer+=bitcounts[a2];

    answer=lowergenome ^ xormasks[env[1]][1]; //apply greenmask
    a2=answer/65536;
    answer &= (unsigned int) 65535;
    finalanswer+=bitcounts[answer];
    finalanswer+=bitcounts[a2];

    answer=lowergenome ^ xormasks[env[2]][2]; //apply bluemask
    a2=answer/65536;
    answer &= (unsigned int) 65535;
    finalanswer+=bitcounts[answer];
    finalanswer+=bitcounts[a2];

    if (finalanswer>=target+settleTolerance) {fitness=0; age=0; return 0;} // no use
    if (finalanswer<=target-settleTolerance) {fitness=0; age=0; return 0;} // no use

    //These next two SHOULD do reverse of the abs of finalanswer (i.e 0=20, 20=0)
    // RJG - so if settle tolerance is, say, 15, peak fitness will be 15, min will be one, zero will be dead.
    if (finalanswer<target) fitness = settleTolerance - (target - finalanswer);
    else fitness = settleTolerance + target - finalanswer;

    return fitness;
}


bool Critter::iterate_parallel(int *KillCount_local, int addfood)
{
    if (age)
    {
        //RJG - Here is where an individual dies.
        if ((--age)==0)
        {
            (*KillCount_local)++;
            totalfit[xpos][ypos]-=fitness;
            fitness=0;
            if (maxused[xpos][ypos]==zpos)
            {
                for (int n=zpos-1; n>=0; n--)
                if (critters[xpos][ypos][n].age>0)
                {
                     maxused[xpos][ypos]=n;
                     goto past;
                }
                maxused[xpos][ypos]=-1;
    past:   ;
            }
            return false;
        }
        energy +=  fitness * addfood;
        //energy+= (fitness * food) / totalfit[xpos][ypos];]

//non-slot version - try breeding if our energy is high enough
            if (energy>(breedThreshold+breedCost)) {energy-=breedCost; return true;}
            else return false;

    }
    return false;
}

int Critter::breed_with_parallel(int xpos, int ypos, Critter *partner, int *newgenomecount_local)
//newgenomecount_local is position in genome array - arranged so can never overlap
//returns 1 if it fails (so I can count fails)
{
    //do some breeding!

    bool breedsuccess1=true; //for species restricted breeding
    bool breedsuccess2=true; //for difference breeding

    if (breedspecies)
    {
        if (partner->speciesid!=speciesid) breedsuccess1=false;
    }

    if (breeddiff)
    {
        int t1=0;
        // - determine success.. use genetic similarity
        quint64 cg1x = genome ^ partner->genome; //XOR the two to compare

       //Coding half
        quint32 g1xl = quint32(cg1x & ((quint64)65536*(quint64)65536-(quint64)1)); //lower 32 bits
        t1 = bitcounts[g1xl/(quint32)65536] +  bitcounts[g1xl & (quint32)65535];

        //non-Coding half
        quint32 g1xu = quint32(cg1x / ((quint64)65536*(quint64)65536)); //upper 32 bits
        t1 += bitcounts[g1xu/(quint32)65536] +  bitcounts[g1xu & (quint32)65535];
        if (t1>maxDiff)
        {
            breedsuccess2=false;
        }
     }

    if (breedsuccess1 && breedsuccess2)
    {
         //work out new genome

         quint64 g1x = genex[nextgenex++]; if (nextgenex >= 65536) nextgenex = 0;
         quint64 g2x = ~g1x; // inverse

         g2x &= genome;
         g1x &= partner->genome;  // cross bred genome
         g2x |= g1x;

         bool local_mutate=false;

         //this is technically not threadsafe, but it doesn't matter - any value for nextrand is fine
         if(!variableMutate)
             if ((TheSimManager->Rand8())<mutate)
                 local_mutate=true;

         //RJG - Variable mutate implemented here.
         if(variableMutate)
         {
             quint32 g1xu = quint32(g2x / ((quint64)4294967296)); //upper 32 bits
             int t1 = bitcounts[g1xu/(quint32)65536] +  bitcounts[g1xu & (quint32)65535];
            //RJG - probability of mutation currently standard normal distribution from -3 to +3 scaled to randmax
            //More 1's in non coding genome == higher probability of mutation - see documentation.
            if(TheSimManager->Rand32()>=cumulative_normal_distribution[t1])local_mutate=true;
         }

         if(local_mutate)
         {
             int w=TheSimManager->Rand8();
             w &=63;
             g2x ^= tweakers64[w];
         }

         //store it all

         newgenomes[*newgenomecount_local]=g2x;
         newgenomeX[*newgenomecount_local]=xpos;
         newgenomeY[*newgenomecount_local]=ypos;
         newgenomespecies[*newgenomecount_local]=speciesid;
         newgenomeDisp[(*newgenomecount_local)++]=dispersal; //how far to disperse - low is actually far (it's a divider - max is 240, <10% are >30
         return 0;
    }
    else
    {
        //breeders get their energy back - this is an 'abort'
        //---- RJG: Note that this refund is different to and exclusive from that in Simmanager, which refunds if no partner found.
        energy+=breedCost;
        //---- RJG: Presumably removed to prevent critters getting multiple refunds
        //partner->energy+=breedCost;
        return 1;
    }
}
