#include "simsettings.h"
#include <QDebug>

SimSettings::SimSettings()
{
    //Settable ints
    gridX = 100; //Can't be used to declare arrays - hence there is a define to do this, and then this dictates how much of the array is actually filled (current max 256)
    gridY = 100;
    speciesSamples = 1;
    speciesSensitivity = 2;
    timeSliceConnect = 5;
    environmentChangeRate = 100;
    for (int i = 0; i < MAX_GENOME_WORDS; i++) reseedGenome[i] = 0; //RJG - Genome for reseed with known genome
    lastSpeciesCalculated = 0;
    minSpeciesSize = 0;
    maxDifference = 3;
    genomeSize = 2;
    environmentMode = ENV_MODE_LOOP;
    pathogenMode = PATH_MODE_DRIFT;
    trophicSeedMode = TROPHIC_SEED_DEFAULT;
    reseedMode = RESEED_MODE_SINGLE_RANDOM;

    //EF - A priori ecological interactions table, from which more computationally efficient tables are constructed. Table arrangement is {row 1: 00_vs{00,01,10,11}, row 2: 01_vs{00,01,10,11}...}
    qint16 default_a_priori_interaction[4][4] = {{0, -2, -2, -4}, {2, 0, 0, -2}, {2, 0, 0, -2}, {4, 2, 2, 0}}; // Predator Grid
    //RJG - this is a shortcut to assign the values without having to do for loops, whilst not assigning the values on initialisation in the header
    std::copy(&default_a_priori_interaction[0][0], &default_a_priori_interaction[0][0] + 16, &a_priori_interaction[0][0]);

    //Settable bools
    predationRestriction = false;
    logging = false;
    fitnessLoggingToFile = false;
    nonspatial = false;
    environmentInterpolate = true;
    toroidal = false;
    gui = false;
    logging = false;
    recalculateFitness = false;
    environmentInterpolate = true;
    linkagesOn = false;
    randomReseedBeforeGenetic = true;
}
