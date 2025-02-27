/**
 * @file
 * Simulation Manager
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

#include "mainwindow.h"
#include "simmanager.h"

#include <QDebug>
#include <QImage>
#include <QMessageBox>
#include <QThread>
#include <QRandomGenerator>
#include <cstdlib>
#include <cmath>
#include <iostream>

/****** The below is all hard coded static arrays for speed, and is thus global ******/
//Globabl data
Critter critters[GRID_X][GRID_Y][SLOTS_PER_GRID_SQUARE]; //main array - static for speed
quint8 randoms8[65536]; //RJG - This remains global so new randoms can be loaded from file
quint32 totalFitness[GRID_X][GRID_Y];
quint32 newgenomeX[GRID_X * GRID_Y * SLOTS_PER_GRID_SQUARE * 2];
quint32 newgenomeY[GRID_X * GRID_Y * SLOTS_PER_GRID_SQUARE * 2];
quint64 iteration;
quint64 pathogens[GRID_X][GRID_Y][PATH_DEPTH]; //RJG - Pathogen overlay
//quint32 *newpathogens[GRID_X * GRID_Y * PATH_DEPTH * 2]; //RJG - Pathogen overlay for variable size genomes
quint64 newgenomespecies[GRID_X * GRID_Y * SLOTS_PER_GRID_SQUARE * 2];
quint64 newgenometrophic[GRID_X * GRID_Y * SLOTS_PER_GRID_SQUARE * 2];
quint32 *newgenomes[GRID_X * GRID_Y * SLOTS_PER_GRID_SQUARE * 2];
QMutex *mutexes[GRID_X][GRID_Y]; //set up array of mutexes
int newgenomeDisp[GRID_X * GRID_Y * SLOTS_PER_GRID_SQUARE * 2]; //These next to hold the babies... old style arrays for max speed

//Simulation lookups
int nextgenex;
int xdisp[256][256];
int ydisp[256][256];
quint32 bitCounts[65536]; // the bytes representing bit count of each number 0-635535
quint32 tweakers[32]; // the 32 single bit XOR values (many uses!)
quint64 tweakers64[64]; // the 64 bit version
quint32 genex[65536];
qint16 interaction[256][256]; //Lookup for ecological interactions

//Analysis and record keeping
int breedAttempts[GRID_X][GRID_Y]; //for analysis purposes
int breedSuccess[GRID_X][GRID_Y]; //for analysis purposes
int breedGeneration[GRID_X][GRID_Y]; //for analysis purposes
int breedFails[GRID_X][GRID_Y]; //for analysis purposes
int settles[GRID_X][GRID_Y]; //for analysis purposes
int settlefails[GRID_X][GRID_Y]; //for analysis purposes
int maxUsed[GRID_X][GRID_Y];
int Randcelllist1[GRID_X];
int Randcelllist2[GRID_Y];

//Species stuff
LogSpecies *rootSpecies;
QHash<quint64, LogSpecies *> logSpeciesByID;
QList<Species> *oldSpeciesList, *newSpeciesList;
QList< QList<Species> > archivedSpeciesLists;
QList<uint> speciesColours;
quint8 speciesMode;
quint64 nextSpeciesID;
quint64 ids; //used in tree export
bool allowExcludeWithDescendants = false;

/*!
 * \brief SimManager::SimManager
 */
SimManager::SimManager()
{
    //Constructor - set up all the data!
    speciesMode = SPECIES_MODE_BASIC;

    //RJG note that environment is created from MainWindow - uses class ImageSequence which in turn uses simmanager, and thus can't be called in this constructor.
    simulationRandoms = new randoms; // RJG - We need randoms for this simulation
    simulationSettings = new SimSettings; //RJG - These are the settings which can't be modified per cell, but apply to whole simulation
    cellSettingsMaster = new CellSettings; //RJG - These are the overall settings for cells which can be modified by images

    makeLookups();

    aliveCount = 0;

    for (auto &mutexe : mutexes)
        for (int j = 0; j < GRID_X; j++)
            mutexe[j] = new QMutex();

    env = nullptr;

    nextSpeciesID = 1;
    rootSpecies = (LogSpecies *)nullptr;

    warningCount = 0;

    //RJG Set up linkages
    for (auto &l : linkedVariables)linkages.insert(l, linkageClass(l));

    //Set newgenomes pointers to all be null. They get created as required later on
    for (int i = 0; i < GRID_X * GRID_Y * SLOTS_PER_GRID_SQUARE * 2; i++) newgenomes[i] = nullptr;

    //Make systems
    environmentalFitnessSytem = new EnvironmentFitnessSystem();
    //MDS note - have to pass maxsize as simulationManager is set on return from this constructor
    environmentalFitnessSytem->setGenomeWordsFromString("0", MAX_GENOME_WORDS);
    systemsList.append(environmentalFitnessSytem);

    bitCountSystem = new BitCountSystem();
    bitCountSystem->setGenomeWordsFromString("01", MAX_GENOME_WORDS);
    //systemsList.append(bitCountSystem);

    breedSystem = new BreedSystem();
    breedSystem->setGenomeWordsFromString("01", MAX_GENOME_WORDS);
    systemsList.append(breedSystem);

    variableMutateSystem = new VariableMutateSystem();
    variableMutateSystem->setGenomeWordsFromString("01", MAX_GENOME_WORDS);
    systemsList.append(variableMutateSystem);

    mutationSystem = new MutationSystem();
    mutationSystem->setGenomeWordsFromString("01", MAX_GENOME_WORDS);
    systemsList.append(mutationSystem);

    variableBreedSystem =  new VariableBreedSystem();
    variableBreedSystem->setGenomeWordsFromString("1", MAX_GENOME_WORDS);
    systemsList.append(variableBreedSystem);

    pathogensSystem = new PathogensSystem();
    pathogensSystem->setGenomeWordsFromString("01", MAX_GENOME_WORDS);
    systemsList.append(pathogensSystem);

    speciesIDSytem = new SpeciesIDSystem();
    speciesIDSytem->setGenomeWordsFromString("01", MAX_GENOME_WORDS);
    systemsList.append(speciesIDSytem);

    interactionSystem = new InteractionSystem();
    interactionSystem->setGenomeWordsFromString("0", MAX_GENOME_WORDS);
    systemsList.append(interactionSystem);

    visualisationSystem = new VisualisationSystem();
    visualisationSystem->setName("Visualisation System 1");
    visualisationSystem->setGenomeWordsFromString("0", MAX_GENOME_WORDS);
    systemsList.append(visualisationSystem);

    visualisationSystem2 = new VisualisationSystem();
    visualisationSystem->setName("Visualisation System 2");
    visualisationSystem2->setGenomeWordsFromString("1", MAX_GENOME_WORDS);
    systemsList.append(visualisationSystem2);

    genomeComparisonSystem = new GenomeComparisonSystem();
    genomeComparisonSystem->setGenomeWordsFromString("01", MAX_GENOME_WORDS);
    systemsList.append(genomeComparisonSystem);

    hgtSystem = new HgtSystem();
    hgtSystem->setGenomeWordsFromString("01", MAX_GENOME_WORDS);
    systemsList.append(hgtSystem);

    variableHgtProbSystem = new VariableHgtProbSystem();
    variableHgtProbSystem->setGenomeWordsFromString("1", MAX_GENOME_WORDS);
    systemsList.append(variableHgtProbSystem);

    variableHgtLenSystem = new VariableHgtLenSystem();
    variableHgtLenSystem->transformableGenomeSize = hgtSystem->returnUseGenomeWordsCount();
    variableHgtLenSystem->setGenomeWordsFromString("0", MAX_GENOME_WORDS);
    systemsList.append(variableHgtLenSystem);

    variableHgtIdSystem = new VariableHgtIdSystem();
    variableHgtIdSystem->setGenomeWordsFromString("0", MAX_GENOME_WORDS);
    systemsList.append(variableHgtIdSystem);

    simulationLog = new LogSimulation(simulationSettings);
}

//MDS gets called from command line parser - whether or not there is a command line
//pass -1 for 'use QT's idea of what thread count should be' (the default)
void SimManager::SetProcessorCount(int count)
{
    if (count == -1)
        ProcessorCount = QThread::idealThreadCount();
    else
        ProcessorCount = count;


    //some sanity checking
    if (ProcessorCount < 1) ProcessorCount = 1;
    if (ProcessorCount > 256) ProcessorCount = 256; //a sanity check

    for (int i = 0; i < ProcessorCount; i++) FuturesList.append(new QFuture<int>);
}

/*!
 * \brief SimManager::MakeLookups
 */
void SimManager::makeLookups()
{

    //These are 00000001, 000000010, 0000000100 etc
    tweakers[0] = 1;
    for (int n = 1; n < 32; n++) tweakers[n] = tweakers[n - 1] * 2;

    tweakers64[0] = 1;
    for (int n = 1; n < 64; n++) tweakers64[n] = tweakers64[n - 1] * 2;

    //and now the bitcounting...
    // set up lookup 0 to 65535 to enable bits to be counted for each
    for (qint32 n = 0; n < 65536; n++)
    {
        qint32 count = 0;
        for (int m = 0; m < 16; m++) if ((n & tweakers[m]) != 0) ++count; // count the bits
        bitCounts[n] = count;
    }

    makeInteractionLookups();

    // gene exchange lookup
    for (auto &n : genex)    //random bit combs, averaging every other bit on
    {
        quint32 value = 0;
        for (auto m : tweakers) if (QRandomGenerator::global()->bounded(2)) value += m;
        n = value;
    }
    nextgenex = 0;

    //Dispersal table - lookups for dispersal amount
    //n is the distance to be dispersed - biased locally (the sqrt)
    //m is angle
    for (int n = 0; n < 256; n++)
    {
        double d = sqrt(65536 / (double)(n + 1)) - 16;
        if (d < 0) d = 0;
        for (int m = 0; m < 256; m++)
        {
            xdisp[n][m] = (int)(d * sin((double)(m) / 40.5845));
            ydisp[n][m] = (int)(d * cos((double)(m) / 40.5845));
        }
    }

    //colours
    for (int i = 0; i < 65536; i++) speciesColours.append(qRgb(simulationRandoms->rand8(), simulationRandoms->rand8(), simulationRandoms->rand8()));

}

/*!
 * \brief SimManager::makeInteractionLookups
 * \param
 */
void SimManager::makeInteractionLookups()
{
    //EF - ecology lookup construction: I'm sure that there's a better way to do this, but I think that this works...
    for (int org1 = 0; org1 < 256; org1++)  // First, set all values to zero.
    {
        for (int org2 = 0; org2 < 256; org2++)
        {
            interaction[org1][org2] = 0;
        }
    }
    for (int org1 = 0; org1 < 256; org1++)  // Then, determine values.
    {
        for (int org2 = 0; org2 < 256; org2++)
        {
            int org1_runner = org1;
            int org2_runner = org2;
            for (int n = 0; n < 4; n++)
            {
                interaction[org1][org2] = interaction[org1][org2] + simulationSettings->a_priori_interaction[org1_runner % 4][org2_runner % 4]; // % 4 effectively gets the last two digits of a binary number
                org1_runner = org1_runner / 4; // Division always rounds down, so this removes the last two digits, which have now been examined.
                org2_runner = org2_runner / 4;
            }
        }
    }
}

/*!
 * \brief SimManager::populateCellSettings
 *
 *  //RJG - Update cell-based settings arrays for this iteration
 */
void SimManager::populateCellSettings()
{
    for (int n = 0; n < simulationSettings->gridX; n++)
        for (int m = 0; m < simulationSettings->gridY; m++)
            //RJG - cellSettingsMaster is a pointer, cellSettings not, hence dereference pointer
            cellSettings[n][m] = *cellSettingsMaster;
}

/*!
 * \brief SimManager::setupRun
 *
 * RJG - Find middle square, try creatures till something lives, duplicate it [slots] times
 * RJG - called on initial program load and reseed, but also when run/run for are hit
 * RJG - with modification for dual seed if selected
 */
void SimManager::setupRun()
{
    //Kill em all
    for (int n = 0; n < simulationSettings->gridX; n++)
        for (int m = 0; m < simulationSettings->gridY; m++)
        {
            //Clear contents of cell - go up to global max, as slots is controlled locally
            for (int c = 0; c < SLOTS_PER_GRID_SQUARE; c++)
            {
                critters[n][m][c].age = 0;
                critters[n][m][c].fitness = 0;
            }
            totalFitness[n][m] = 0;
            maxUsed[n][m] = -1;
            breedAttempts[n][m] = 0;
            breedSuccess[n][m] = 0;
            breedFails[n][m] = 0;
            settles[n][m] = 0;
            settlefails[n][m] = 0;
        }

    aliveCount = 0;
    nextSpeciesID = 1; //reset ID counter

    //RJG - Reset environmental fitness system
    environmentalFitnessSytem->reset();
    //RJG - Reset pathogens
    pathogensSystem->reset();

    makeLookups();
    populateCellSettings();

    int n = simulationSettings->gridX / 2, m = simulationSettings->gridY / 2;
    int n2 = 0;

    //Dual seed if required
    if (simulationSettings->reseedMode > 1)
    {
        n = 2;
        n2 = simulationSettings->gridX - 2;
    }

    quint8 *environment = simulationManager->env->returnRGB(n, m);
    quint8 *environment2 = simulationManager->env->returnRGB(n2, m);

    //RJG - Either reseed with known genome if set, or then random ones, dual and single. Default at end is single reseed random genome
    if (simulationSettings->reseedMode == RESEED_MODE_SINGLE_KNOWN)
    {
        critters[n][m][0].initialise((quint32 *)(simulationSettings->reseedGenome), environment, n, m, 0, nextSpeciesID, 0);
        if (critters[n][m][0].fitness == 0)
        {
            // RJG - But sort out if it can't survive...
            QString message("The genome you're trying to reseed with can't survive in this environment. There could be a number of reasons why this is. Please contact RJG or MDS to discuss. Reverting to reseed random.");

            if (!mainWindow->autoFromCommand)QMessageBox::warning(nullptr, "Oops", message);
            else qInfo() << message;

            simulationSettings->reseedMode = RESEED_MODE_SINGLE_RANDOM;

            setupRun();
            return;
        }

        //RJG - I think this is a good thing to flag in an obvious fashion.
        QString reseedGenomeString("Started simulation with known genome: ");
        for (int i = 0; i < simulationSettings->genomeSize; i++) reseedGenomeString.append(printGenome(simulationSettings->reseedGenome[i]));
        mainWindow->setStatusBarText(reseedGenomeString);
    }

    else if (simulationSettings->reseedMode == RESEED_MODE_DUAL_KNOWN)
    {
        critters[n][m][0].initialise((quint32 *)(&simulationSettings->reseedGenome), environment, n, m, 0, nextSpeciesID, 0);
        critters[n2][m][0].initialise((quint32 *)(&simulationSettings->reseedGenome), environment2, n2, m, 0, nextSpeciesID, 0);
        if (critters[n][m][0].fitness == 0 || critters[n2][m][0].fitness == 0)
        {
            // RJG - But sort out if it can't survive...
            QString message("The genome you're trying to reseed with can't survive in one of the two chosen environmental pixels. Please either try different settings, or contact the Palaeoware team to discuss. Reverting to reseed random.");

            if (!mainWindow->autoFromCommand)QMessageBox::warning(nullptr, "Oops", message);
            else qInfo() << message;

            simulationSettings->reseedMode = RESEED_MODE_DUAL_RANDOM;
            setupRun();
            return;
        }
        //RJG - I think this is a good thing to flag in an obvious fashion.
        QString reseedGenomeString("Started simulation with dual known genome starting: ");
        int size = simulationSettings->genomeSize;
        if (size > 3) size = 3;
        for (int i = 0; i < size; i++) reseedGenomeString.append(printGenome(simulationSettings->reseedGenome[i]));
        mainWindow->setStatusBarText(reseedGenomeString);
    }

    else if (simulationSettings->reseedMode == RESEED_MODE_DUAL_RANDOM)
    {
        quint32 genome[MAX_GENOME_WORDS];
        bool OK = environmentalFitnessSytem->findRandomViableGenome(genome, environment, cellSettings[n][m].target, cellSettings[n][m].settleTolerance);

        quint32 genome2[MAX_GENOME_WORDS];
        if (OK) OK = environmentalFitnessSytem->findRandomViableGenome(genome2, environment2, cellSettings[n2][m].target, cellSettings[n2][m].settleTolerance);

        if (!OK)
        {
            QString message("It looks like no digital organisms are capable of surviving using the current settings. There could be a number of reasons why this is: either try different settings, or contact the Palaeoware team to discuss. Reverting to reseed random.");
            if (!mainWindow->autoFromCommand)QMessageBox::warning(nullptr, "Problem", message);
            else qInfo() << message;
            aliveCount = 0;
            simulationSettings->reseedMode = RESEED_MODE_SINGLE_RANDOM;
            return;
        }

        critters[n][m][0].initialise(genome, environment, n, m, 0, nextSpeciesID, 0);
        critters[n2][m][0].initialise(genome2, environment2, n2, m, 0, nextSpeciesID, 0);
        mainWindow->setStatusBarText("");
    }
    else if (simulationSettings->reseedMode == RESEED_MODE_DUAL_IDENTICAL)
    {
        quint32 genome[MAX_GENOME_WORDS];

        bool OK = environmentalFitnessSytem->findDualViableGenome(genome, environment, environment2, cellSettings[n][m].target, cellSettings[n][m].settleTolerance);

        if (!OK)
        {
            QString message("It looks like no digital organisms are capable of surviving using the current settings. There could be a number of reasons why this is: either try different settings, or contact the Palaeoware team to discuss. Reverting to dual random reseed.");
            if (!mainWindow->autoFromCommand)QMessageBox::warning(nullptr, "Problem", message);
            else qInfo() << message;
            aliveCount = 0;
            simulationSettings->reseedMode = RESEED_MODE_DUAL_RANDOM;
            return;
        }

        critters[n][m][0].initialise(genome, environment, n, m, 0, nextSpeciesID, 0);
        critters[n2][m][0].initialise(genome, environment2, n, m, 0, nextSpeciesID, 0);

        mainWindow->setStatusBarText("");
    }
    else
    {
        quint32 genome[MAX_GENOME_WORDS];

        if (!environmentalFitnessSytem->findRandomViableGenome(genome, environment, cellSettings[n][m].target, cellSettings[n][m].settleTolerance))
        {
            QString message("It looks like no digital organisms are capable of surviving using the current settings. There could be a number of reasons why this is: either try different settings, or contact the Palaeoware team to discuss.");
            if (!mainWindow->autoFromCommand)QMessageBox::warning(nullptr, "Problem", message);
            else qInfo() << message;
            aliveCount = 0;
            return;
        }

        critters[n][m][0].initialise(genome, environment, n, m, 0, nextSpeciesID, 0);
        mainWindow->setStatusBarText("");
    }

    totalFitness[n][m] = critters[n][m][0].fitness; //may have gone wrong from above
    if (simulationSettings->reseedMode > 1)totalFitness[n2][m] = critters[n2][m][0].fitness;
    aliveCount = 1;
    iteration = 0;
    quint32 artificalGenome1[MAX_GENOME_WORDS];
    quint32 artificalGenome2[MAX_GENOME_WORDS];
    quint32 artificalGenome3[MAX_GENOME_WORDS];
    quint32 artificalGenome4[MAX_GENOME_WORDS];
    quint32 artificalGenome5[MAX_GENOME_WORDS];

    if (simulationSettings->trophicSeedMode == TROPHIC_SEED_5TIER)
    {
        for (int word = 0; word < MAX_GENOME_WORDS; word++)
        {
            artificalGenome1[word] = 3221290992;
            artificalGenome2[word] = 1610612736;
            artificalGenome3[word] = 805306368;
            artificalGenome4[word] = 402653184;
            artificalGenome5[word] = 201326592;
        }
    }
    else if (simulationSettings->trophicSeedMode == TROPHIC_SEED_3TIER)
    {
        for (int word = 0; word < MAX_GENOME_WORDS; word++)
        {
            artificalGenome1[word] = 858783744;
            artificalGenome2[word] = 1717567488;
            artificalGenome3[word] = 3435659264;
        }
    }
    quint32 *aG1p = &artificalGenome1[0];
    quint32 *aG2p = &artificalGenome2[0];
    quint32 *aG3p = &artificalGenome3[0];
    quint32 *aG4p = &artificalGenome4[0];
    quint32 *aG5p = &artificalGenome5[0];

    //RJG - Fill square with successful critter

    if ((simulationSettings->trophicSeedMode == TROPHIC_SEED_5TIER) || (simulationSettings->trophicSeedMode == TROPHIC_SEED_3TIER))
    {
        for (int n = 0; n < simulationSettings->gridX; n++)
            for (int m = 0; m < simulationSettings->gridY; m++)
                for (int c = 0; c < cellSettingsMaster->slotsPerSquare; c++)
                {
                    if (simulationSettings->trophicSeedMode == TROPHIC_SEED_5TIER)
                    {
                        if (c % 5 == 1) critters[n][m][c].initialise(aG1p, environment, n, m, c, nextSpeciesID, 0);
                        else if (c % 5 == 2) critters[n][m][c].initialise(aG2p, environment, n, m, c, nextSpeciesID, 0);
                        else if (c % 5 == 3) critters[n][m][c].initialise(aG3p, environment, n, m, c, nextSpeciesID, 0);
                        else if (c % 5 == 4) critters[n][m][c].initialise(aG4p, environment, n, m, c, nextSpeciesID, 0);
                        else critters[n][m][c].initialise(aG5p, environment, n, m, c, nextSpeciesID, 0);
                    }
                    else // 3-Tiered seeding
                    {
                        if (c % 3 == 1) critters[n][m][c].initialise(aG1p, environment, n, m, c, nextSpeciesID, 0);
                        else if (c % 3 == 2) critters[n][m][c].initialise(aG2p, environment, n, m, c, nextSpeciesID, 0);
                        else critters[n][m][c].initialise(aG3p, environment, n, m, c, nextSpeciesID, 0);
                    }
                    critters[n][m][c].calculateBitCountWithJitter();

                    if (simulationSettings->reseedMode > 1)
                    {
                        critters[n2][m][c].initialise(critters[n2][m][0].genomeWords, environment, n2, m, c, nextSpeciesID, 0);
                        critters[n2][m][c].calculateBitCountWithJitter();
                    }

                    if (critters[n][m][c].age > 0)
                    {
                        critters[n][m][c].age /= ((simulationRandoms->rand8() / 10) + 1);
                        critters[n][m][c].age += 10;
                        aliveCount++;
                        maxUsed[n][m] = c;
                        totalFitness[n][m] += critters[n][m][c].fitness;
                    }

                    if (simulationSettings->reseedMode > 1 && critters[n2][m][c].age > 0)
                    {
                        critters[n2][m][c].age /= ((simulationRandoms->rand8() / 10) + 1);
                        critters[n2][m][c].age += 10;
                        aliveCount++;
                        maxUsed[n2][m] = c;
                        totalFitness[n2][m] += critters[n2][m][c].fitness;
                    }
                }
    }
    else // Default seeding - central pixel only or dual
    {
        for (int c = 0; c < cellSettingsMaster->slotsPerSquare; c++)
        {
            if (c > 0) critters[n][m][c].initialise(critters[n][m][0].genomeWords, environment, n, m, c, nextSpeciesID, 0);
            critters[n][m][c].calculateBitCountWithJitter();

            if (simulationSettings->reseedMode > 1 && c > 0)
            {
                critters[n2][m][c].initialise(critters[n2][m][0].genomeWords, environment, n2, m, c, nextSpeciesID, 0);
                critters[n2][m][c].calculateBitCountWithJitter();
            }

            if (critters[n][m][c].age > 0)
            {
                critters[n][m][c].age /= ((simulationRandoms->rand8() / 10) + 1);
                critters[n][m][c].age += 10;
                aliveCount++;
                maxUsed[n][m] = c;
                totalFitness[n][m] += critters[n][m][c].fitness;
            }

            if (simulationSettings->reseedMode > 1 && critters[n2][m][c].age > 0)
            {
                critters[n2][m][c].age /= ((simulationRandoms->rand8() / 10) + 1);
                critters[n2][m][c].age += 10;
                aliveCount++;
                maxUsed[n2][m] = c;
                totalFitness[n2][m] += critters[n2][m][c].fitness;
            }
        }
    }

    //RJG - Env is null when constructed, otherwise we need to reset it here
    if (env != nullptr) env->reset(simulationSettings->environmentChangeRate);

    //RJG - remove old species log if one exists
    delete rootSpecies;

    //create a new logspecies with appropriate first data entry
    rootSpecies = new LogSpecies;

    rootSpecies->maxSize = aliveCount;
    rootSpecies->ID = nextSpeciesID;
    rootSpecies->timeOfFirstAppearance = 0;
    rootSpecies->timeOfLastAppearance = 0;
    rootSpecies->parent = (LogSpecies *)nullptr;
    auto *newdata = new LogSpeciesDataItem;
    newdata->centroidRangeX = n;
    newdata->centroidRangeY = m;
    newdata->iteration = 0;
    newdata->cellsOccupied = 1;
    newdata->genomicDiversity = 1;
    newdata->size = aliveCount;
    newdata->geographicalRange = 0;
    newdata->cellsOccupied = 0; //=1, this is stored as -1
    for (int i = 0; i < MAX_GENOME_WORDS; i++)newdata->sampleMultiWordGenome[i] = critters[n][m][0].genomeWords[i];
    newdata->maxEnvironment[0] = environment[0];
    newdata->maxEnvironment[1] = environment[1];
    newdata->maxEnvironment[2] = environment[2];
    newdata->minEnvironment[0] = environment[0];
    newdata->minEnvironment[1] = environment[1];
    newdata->minEnvironment[2] = environment[2];
    newdata->meanEnvironment[0] = environment[0];
    newdata->meanEnvironment[1] = environment[1];
    newdata->meanEnvironment[2] = environment[2];
    newdata->meanFitness = (quint16)((totalFitness[n][m] * 1000) / aliveCount);

    rootSpecies->dataItems.append(newdata);
    logSpeciesByID.clear();
    logSpeciesByID.insert(nextSpeciesID, rootSpecies);

    //RJG - Deprecated, but clear here just in case
    archivedSpeciesLists.clear();

    oldSpeciesList = new QList<Species>();

    Species newsp;
    newsp.ID = nextSpeciesID;
    newsp.originTime = 0;
    newsp.parent = 0;
    newsp.size = cellSettingsMaster->slotsPerSquare;
    newsp.fitness = 0;
    newsp.envFitness = 0;
    for (int ii = 0; ii < MAX_GENOME_WORDS; ii++) newsp.type[ii] = critters[n][m][0].genomeWords[ii];
    newsp.logSpeciesStructure = rootSpecies;
    for (int ii = 0; ii < MAX_GENOME_WORDS; ii++)
        for (int i = 0; i < 32; i++)
            // qDebug() << ii << tweakers[31 - i] << critters[n][m][0].genomeWords[ii];
            if (tweakers[31 - i] & critters[n][m][0].genomeWords[ii])
                newsp.frequenciesAtOrigination[ii][i] = newsp.frequenciesLastIteration[ii][i] = 1;
            else
                newsp.frequenciesAtOrigination[ii][i] = newsp.frequenciesLastIteration[ii][i] = 0; //set up initial gene frequencies. All individuals alike, so it's just the genome*/

    oldSpeciesList->append(newsp);

    nextSpeciesID++; //ready for first species after this

    //RJG - now set up pathogens. Chose to do here rather than with lookups as pathogens mutate, and thus it's pretty much impossible to repeat them anyway - so start afresh each run
    for (int n = 0; n < GRID_X; n++)
        for (int m = 0; m < GRID_Y; m++)
            for (int o = 0; o < PATH_DEPTH; o++)
                //RJG - Seed pathogen layer with 64 bit randoms too
                pathogens[n][m][o] = simulationRandoms->rand64();

    //RJG - reset warning system
    warningCount = 0;
}

/*!
 * \brief SimManager::iterateParallel
 * \param firstx
 * \param lastx
 * \param newGenomeCountLocal
 * \param killCountLocal
 * \return
 *
 * Parallel version - takes newgenomes_local as the start point it can write to in main genomes array
 * Returns number of new genomes
 */
int SimManager::iterateParallel(int firstx, int lastx, int newGenomeCountLocal, int *killCountLocal)
{
    int breedlist[67][SLOTS_PER_GRID_SQUARE]; // ENF 67 breed lists, one per possible bitcount value for a two-word genome, plus two to account for the bins added by +1 and -1 shifts in the bitcount system.
    int maxalive;
    int deathcount;

    int StartingIndex = 0;
    int IndexOffset = 0;
    int n;
    int m;

    // For every cell...
    for (int nOld = firstx; nOld <= lastx; nOld++)
        for (int mOld = 0; mOld < simulationSettings->gridY; mOld++)
        {
            n = Randcelllist1[firstx + (StartingIndex % (lastx - firstx + 1))];
            m = Randcelllist2[(StartingIndex + IndexOffset) % simulationSettings->gridY];
            CellSettings *settings = &cellSettings[n][m];
            int maxv = maxUsed[n][m];

            Critter *crit = critters[n][m];
            quint8 *environment = simulationManager->env->returnRGB(n, m);

            // ----RJG: Pathogens have set chance of killing any living critter
            if (temp_path_on)
            {
                //RJG - if pathogens are just drifting, use the first entry in the pathogens array, and ignore the rest
                if (simulationSettings->pathogenMode == PATH_MODE_DRIFT)
                    for (int c = 0; c <= maxv; c++)
                        if (pathogensSystem->willDie(crit[c].genomeWords, n, m, 0))
                        {
                            crit[c].age = 1;
                            crit[c].energy = 0;
                        }


                //RJG - when pathogens are evolving, we want to keep track of how virulent they are
                if (simulationSettings->pathogenMode == PATH_MODE_EVOLVE && maxv > 0)
                {
                    //RJG - Use counts to record how nasty it is
                    int pathogenKills[PATH_DEPTH][3][3] = {{{0}}};

                    //RJG - For every pathogen
                    for (int i = 0; i < PATH_DEPTH; i++)
                        //RJG - assess virulence for this cell, and also those around it allowing pathogens to migrate
                        for (int j = -1; j < 2; j++)
                            for (int k = -1; k < 2; k ++)
                                //RJG - assess a 3 by 3 square, but ignore diagonals - trying to minimise associated slowdown
                                if (j == 0 || k == 0)
                                {
                                    int nLocal = n + j;
                                    int mLocal = m + k;

                                    if (simulationSettings->toroidal)
                                    {
                                        nLocal = (nLocal + simulationSettings->gridX) % simulationSettings->gridX;
                                        mLocal = (mLocal + simulationSettings->gridY) % simulationSettings->gridY;
                                    }
                                    else
                                    {
                                        if (nLocal < 0 || nLocal >= simulationSettings->gridX)  continue;
                                        if (mLocal < 0 || mLocal >= simulationSettings->gridY)  continue;
                                    }

                                    for (int c = 0; c <= maxv; c++)
                                        if (pathogensSystem->willDie(crit[c].genomeWords, nLocal, mLocal, i))
                                            pathogenKills[i][j + 1][k + 1]++;
                                }

                    //RJG - Work out nastiest
                    int max = 0;
                    int pathogen[3] = {-1};
                    for (int i = 0; i < PATH_DEPTH; i++)
                        //RJG - assess virulence for this cell, and also those around it allowing pathogens to migrate
                        for (int j = 0; j < 3; j++)
                            for (int k = 0; k < 3; k ++)
                            {
                                if (pathogenKills[i][j][k] > max)
                                {
                                    max = pathogenKills[i][j][k];
                                    pathogen[0] = i;
                                    pathogen[1] = j - 1;
                                    pathogen[2] = k - 1;
                                }
                            }

                    //RJG - from above we now have the most virulent pathogen - now let's do stuff with it.
                    //RJG - first work out coords - note only need to deal with toroidal as outside bounds won't have been in count
                    int nLocal = n + pathogen[1];
                    int mLocal = m + pathogen[2];
                    if (simulationSettings->toroidal)
                    {
                        nLocal = (nLocal + simulationSettings->gridX) % simulationSettings->gridX;
                        mLocal = (mLocal + simulationSettings->gridY) % simulationSettings->gridY;
                    }

                    //RJG - then do the killing
                    for (int c = 0; c <= maxv; c++)
                        if (pathogensSystem->willDie(crit[c].genomeWords, nLocal, mLocal, pathogen[0]))
                        {
                            crit[c].age = 1;
                            crit[c].energy = 0;
                        }

                    //RJG - now lets do the evolution, select individual for duplication, apply mutation, the throw back into pathogen array randomly
                    pathogensSystem->replicate(nLocal, mLocal, n, m, pathogen[0]);
                }
            }

            // ... recalculate critter fitnesses if requested...
            if (simulationSettings->recalculateFitness)
                // Not certain how this block interacts with the "interact with energy" exception to the settle failure rule.
            {
                totalFitness[n][m] = 0;
                maxalive = 0;
                deathcount = 0;
                for (int c = 0; c <= maxv; c++)
                {
                    if (crit[c].age)
                    {
                        quint32 f = crit[c].calculateFitness(environment);
                        totalFitness[n][m] += f;
                        if (f > 0) maxalive = c;
                        else deathcount++; // ... recording deaths caused by this recalculation.
                    }
                }
                maxUsed[n][m] = maxalive;
                maxv = maxalive;
                (*killCountLocal) += deathcount;
            }

            // RJG - reset counters for fitness logging to file
            if (simulationSettings->fitnessLoggingToFile || simulationSettings->logging)
            {
                breedAttempts[n][m] = 0;
                breedSuccess[n][m] = 0;
                breedGeneration[n][m] = 0;
            }

            //PG- HGT block
            //- if either hgt setting selected enter hgt loop
            if (cellSettingsMaster->hgtTransform || simulationSettings->variableHgtProb)
            {
                quint32* willDie[256]; //maximum slots per grid square
                int willDieCount = 0;

                //- produce a list in the cell of all organism set to die
                for (int c = 0; c < cellSettingsMaster->slotsPerSquare; c++)
                {
                    //- if age is initiated , age is less than 2 and energy is 0 create the list
                    if (crit[c].age && (crit[c].age < 2 || crit[c].energy == 0)) //- think about interactions with energy stealers and energy being set to 0 as indication of death
                    {
                        willDie[willDieCount] = crit[c].genomeWords;
                        willDieCount++;
                    }
                }

                //- If there is at least one organism flagged to die in cell, randomly select one "dead" donor genome
                if (willDieCount !=0)
                {
                    quint32 selectedGenome = simulationRandoms->rand32() % willDieCount;
                    quint32* donorGenome = willDie[selectedGenome];

                    //- Iterate through organisms slots in the cell
                    for (int c = 0; c <= cellSettingsMaster->slotsPerSquare; c++)
                    { 
                        //- if slot is not empty, if GUI settings or variable breed probabilty determine HGT will take place, do the thing
                        if ((crit[c].age) && (((cellSettingsMaster->hgtTransform && hgtSystem->willTransform()) || (simulationSettings->variableHgtProb && variableHgtProbSystem->variableWillTransform(crit[c].genomeWords)))))
                        {
                            quint32 positionMask[hgtSystem->useGenomeWordsCount]; // mask created based on transfer position and length, needed for correct transfer into recipent
                            quint32 donorMask[hgtSystem->useGenomeWordsCount]; // mask including donor transfer segment

                            hgtSystem->generateMask(crit[c].genomeWords, positionMask); // generate mask for transformation
                            hgtSystem->generateTransfer(donorGenome, positionMask, donorMask); // use mask to generate transfer segment
                            hgtSystem->transformRecipient(crit[c].genomeWords, donorGenome, positionMask, donorMask); // tranform recipient genome by inserting transformation segment
                        }
                    }
                }
            }


            // Determine the food to be given to organisms per point of fitness that they have.
            float addFood;
            //totalFitness == 0 is no longer a safe indicator of an empty cell, but it does indicate that a cell does not need fed
            int localFitness = totalFitness[n][m];
            int localFood = cellSettings[n][m].food;
            if (localFitness)
                addFood = (localFood / localFitness);
            else addFood = 0;

            int breedlistentries[67] = {0};

            // ENF If we are using multiple breed lists...
            if (settings->multiBreedList)
            {
                for (int p = 0; p < 67; p++) // ... For each of those lists...
                {
                    // ----RJG: Iterate critters will kill and clean up pathogened critters
                    for (int c = 0; c <= maxv; c++)
                    {
                        if (crit[c].genomeBitCountWithJitter % 67 == p)
                        {
                            if (crit[c].iterateParallel(killCountLocal, addFood, cellSettings[n][m].settleTolerance)) // ... add food and age up the critters.
                            {
                                breedlist[p][breedlistentries[p]++] = c; // Then, if they have enough energy to breed, put them on the relevant list.
                            }
                        }
                    }

                    if (settings->variableBreed) for (int c = 0; c <= maxv; c++)crit[c].variableBreedAsex = 0;

                    //----RJG Do breeding
                    if (breedlistentries[p] > 0)
                    {
                        quint8 divider = 255 / breedlistentries[p];
                        for (int c = 0; c < breedlistentries[p]; c++)
                        {
                            int partner;
                            bool localAsexual = settings->asexual;

                            //Variable breeding allows sexual or asexual reproduction depending on the #1's in the non-coding genome.
                            if (settings->variableBreed) localAsexual = variableBreedSystem->returnAsexual(crit[breedlist[p][c]].genomeWords);

                            if (localAsexual)partner = c;
                            else partner = simulationRandoms->rand8() / divider;

                            if (partner < breedlistentries[p])
                            {
                                // ----RJG: If obligate then fail if you self paired
                                if (settings->obligateSexual && partner == c)
                                {
                                    breedFails[n][m]++;
                                    //----RJG - refund breed cost.
                                    crit[breedlist[p][c]].energy += settings->breedCost;
                                    //----RJG - note if breed fails below, this is refunded in the breedWithParallel function, not so here
                                }
                                // ----RJG: Otherwise, you're good to go, facultative, or asexual (self partnered above)
                                else
                                {
                                    // ----RJG: this returns one (or potentially more) if breeding has failed
                                    if (crit[breedlist[p][c]].breedWithParallel(n, m, &(crit[breedlist[p][partner]]), &newGenomeCountLocal)) breedFails[n][m]++; //for analysis purposes
                                    else  //----RJG: or zero if breeding was a success
                                    {
                                        breedSuccess[n][m]++;
                                        //RJG - to keep a track of generation time, let's total here the age of each critter at breeding success
                                        //Under most settings organisms will breed once per lifetime - this can be checked by doing breed success / gridNumberAlive in the log
                                        breedGeneration[n][m] += (15 - crit[breedlist[p][c]].age);

                                        //----RJG - Keeping track of how bred for recombination log - may want to change down line
                                        if (localAsexual && cellSettingsMaster->variableBreed)crit[breedlist[p][c]].variableBreedAsex = -1;
                                        if (!localAsexual && cellSettingsMaster->variableBreed)crit[breedlist[p][c]].variableBreedAsex = 1;
                                        //----RJG - ultimately this needs to be updated to report if sexual, whether it was self paired.
                                    }
                                }
                            }
                            else   //didn't find a partner, refund breed cost
                                crit[breedlist[p][c]].energy += settings->breedCost;
                        }
                    }
                }
                if (simulationSettings->fitnessLoggingToFile || simulationSettings->logging)
                {
                    breedAttempts[n][m] = 0;
                    for (int p = 0; p < 67; p++)
                    {
                        breedAttempts[n][m] = breedAttempts[n][m] + breedlistentries[p];
                    }
                }
            }
            else // If we are breeding from a single list:
            {
                // ----RJG: Iterate critters will kill and clean up pathogened critters
                for (int c = 0; c <= maxv; c++)
                {
                    if (crit[c].iterateParallel(killCountLocal, addFood, cellSettings[n][m].settleTolerance))
                    {
                        breedlist[0][breedlistentries[0]++] = c;
                    }
                }

                // ----RJG: breedAttempts was no longer used - co-opting for fitness report.
                if (simulationSettings->fitnessLoggingToFile || simulationSettings->logging) breedAttempts[n][m] = breedlistentries[0]; // Needs fixed at some point
                if (settings->variableBreed) for (int c = 0; c <= maxv; c++)crit[c].variableBreedAsex = 0;

                //----RJG Do breeding
                if (breedlistentries[0] > 0)
                {
                    quint8 divider = 255 / breedlistentries[0];
                    for (int c = 0; c < breedlistentries[0]; c++)
                    {
                        int partner;
                        bool temp_asexual = settings->asexual;

                        //Variable breeding allows sexual or asexual reproduction depending on the #1's in the non-coding genome.
                        if (settings->variableBreed)temp_asexual = variableBreedSystem->returnAsexual(crit[breedlist[0][c]].genomeWords);

                        if (temp_asexual)partner = c;
                        else partner = simulationRandoms->rand8() / divider;

                        if (partner < breedlistentries[0])
                        {
                            // ----RJG: If obligate then fail if you self paired
                            if (settings->obligateSexual && partner == c)
                            {
                                breedFails[n][m]++;
                                //----RJG - refund breed cost.
                                crit[breedlist[0][c]].energy += settings->breedCost;
                                //----RJG - note if breed fails below, this is refunded in the breedWithParallel function, not so here
                            }
                            // ----RJG: Otherwise, you're good to go, facultative, or asexual (self partnered above)
                            else
                            {
                                // ----RJG: this returns one (or potentially more) if breeding has failed
                                if (crit[breedlist[0][c]].breedWithParallel(n, m, &(crit[breedlist[0][partner]]), &newGenomeCountLocal)) breedFails[n][m]++; //for analysis purposes
                                else //----RJG: or zero if breeding was a success
                                {
                                    breedSuccess[n][m]++; //RJG - This keeps track of how many successes there are at breeding, obviously
                                    //----RJG - Keeping track of how bred for recombination log - may want to change down line
                                    if (temp_asexual && settings->variableBreed) crit[breedlist[0][c]].variableBreedAsex = -1;
                                    if (!temp_asexual && settings->variableBreed) crit[breedlist[0][c]].variableBreedAsex = 1;
                                    //----RJG - ultimately this needs to be updated to report if sexual, whether it was self paired.

                                    //RJG - to keep a track of generation time, let's total here the age of each critter at breeding success
                                    //Under most settings organisms will breed once per lifetime - this can be checked by doing breed success / gridNumberAlive in the log
                                    breedGeneration[n][m] += (15 - crit[breedlist[0][c]].age);
                                }
                            }
                        }
                        else   //didn't find a partner, refund breed cost
                            crit[breedlist[0][c]].energy += settings->breedCost;
                    }
                }
            }

            if (settings->interactEnergy)
                for (int c = 0; c <= maxv; c++)
                    if (crit[c].age)
                    {
                        for (int interaction_count = 0; interaction_count < settings->interactions; interaction_count++)
                        {
                            quint32 target_slot = simulationRandoms->rand32() % (settings->slotsPerSquare);
                            if ((((crit[target_slot].age) && ((crit[target_slot].speciesID != crit[c].speciesID) || (settings->interactWithinSpecies))) && simulationSettings->predationRestriction == false)
                                    || ((crit[target_slot].age) && ((crit[target_slot].speciesID != crit[c].speciesID) || (settings->interactWithinSpecies)) && ((crit[target_slot].trophicLevel < (crit[c].trophicLevel - 0.5))
                                            || (iteration < 100))))// Interacts only if that slot is occupied by a living critter
                            {
                                int energyTheftAmount = (interactionSystem->performInteractEnergy(crit[c].genomeWords, crit[target_slot].genomeWords, settings->minDeltaPredatorness, crit[target_slot].energy,
                                                                                                  settings->interactBlocks));
                                if (energyTheftAmount >= 1)
                                {
                                    crit[c].energy += (energyTheftAmount * (settings->predationEfficiency) /  10);
                                    crit[c].stolenEnergy += (energyTheftAmount * (settings->predationEfficiency) /  10);
                                    crit[c].trophicLevel = (crit[c].lifetimeEnergy * crit[c].trophicLevel + (energyTheftAmount * (settings->predationEfficiency) /  10) * (crit[target_slot].trophicLevel + 1)) /
                                                           (crit[c].lifetimeEnergy + (energyTheftAmount * (settings->predationEfficiency) /  10));
                                    crit[c].lifetimeEnergy += (energyTheftAmount * (settings->predationEfficiency) /  10);
                                    crit[target_slot].energy -= energyTheftAmount;
                                }
                                //if (crit[target_slot].energy < 1) crit[target_slot].age = 1; // Age set to 1 will be killed by the next iteration
                            }
                        }
                        //if (crit[c].energy < 1) crit[c].age = 1; // It'll die next iteration.
                    }
            if (iteration > 500) // Bodge, which allows .bat running
            {
                for (int c = 0; c <= maxv; c++) //Bodge here to kill some percentage of organisms, at random, every iteration.
                    if (crit[c].age)
                    {
                        quint32 diesForNoReasonNumber = (simulationRandoms->rand32()) % 100;
                        if (diesForNoReasonNumber < static_cast<quint32>(cellSettingsMaster->croppingFrequency))
                        {
                            crit[c].age = 1;
                            crit[c].energy = 0;
                        }
                    }
            }
            StartingIndex++;
            if (StartingIndex >= simulationSettings->gridX)
            {
                StartingIndex = 0;
                IndexOffset++;
            }
        }
    return newGenomeCountLocal;
}

/*!
 * \brief SimManager::settleParallel
 * \param newGenomeCountsStart
 * \param newGenomeCountsEnd
 * \param tryCountLocal
 * \param settleCountLocal
 * \param birthCountsLocal
 * \return
 */
int SimManager::settleParallel(int newGenomeCountsStart, int newGenomeCountsEnd, int *birthCountsLocal)
{
    if (simulationSettings->nonspatial)
    {
        //settling with no geography - just randomly pick a cell
        for (int n = newGenomeCountsStart; n < newGenomeCountsEnd; n++)
        {
            quint64 xPosition = ((quint64)simulationRandoms->rand32()) * (quint64)simulationSettings->gridX;
            xPosition /= (((quint64)65536) * ((quint64)65536));
            quint64 yPosition = ((quint64)simulationRandoms->rand32()) * (quint64)simulationSettings->gridY;
            yPosition /= (((quint64)65536) * ((quint64)65536));

            mutexes[(int)xPosition][(int)yPosition]->lock(); //ensure no-one else buggers with this square
            Critter *crit = critters[(int)xPosition][(int)yPosition];
            //Now put the baby into any free slot here
            for (int m = 0; m < cellSettings[xPosition][yPosition].slotsPerSquare; m++)
            {
                Critter *crit2 = &(crit[m]);
                if (crit2->age == 0)
                {
                    quint8 *environment = simulationManager->env->returnRGB(xPosition, yPosition);
                    //place it
                    crit2->initialise(newgenomes[n], environment, xPosition, yPosition, m, newgenomespecies[n], newgenometrophic[n]);
                    if (crit2->age)
                    {
                        int fit = crit2->fitness;
                        totalFitness[xPosition][yPosition] += fit;
                        (*birthCountsLocal)++;
                        if (m > maxUsed[xPosition][yPosition]) maxUsed[xPosition][yPosition] = m;
                        settles[xPosition][yPosition]++;
                    }
                    else settlefails[xPosition][yPosition]++;
                    break;
                }
            }
            mutexes[xPosition][yPosition]->unlock();
        }
    }
    else
    {
        //old code - normal settling with radiation from original point
        for (int n = newGenomeCountsStart; n < newGenomeCountsEnd; n++)
        {
            //first handle dispersal

            quint8 t1 = simulationRandoms->rand8();
            quint8 t2 = simulationRandoms->rand8();

            int xPosition = (xdisp[t1][t2]) / newgenomeDisp[n];
            int yPosition = (ydisp[t1][t2]) / newgenomeDisp[n];
            xPosition += newgenomeX[n];
            yPosition += newgenomeY[n];


            if (simulationSettings->toroidal)
            {
                //NOTE - this assumes max possible settle distance is less than grid size. Otherwise it will go tits up
                if (xPosition < 0) xPosition += simulationSettings->gridX;
                if (xPosition >= simulationSettings->gridX) xPosition -= simulationSettings->gridX;
                if (yPosition < 0) yPosition += simulationSettings->gridY;
                if (yPosition >= simulationSettings->gridY) yPosition -= simulationSettings->gridY;
            }
            else
            {
                if (xPosition < 0) continue;
                if (xPosition >= simulationSettings->gridX)  continue;
                if (yPosition < 0)  continue;
                if (yPosition >= simulationSettings->gridY)  continue;
            }

            mutexes[xPosition][yPosition]->lock(); //ensure no-one else buggers with this square
            Critter *crit = critters[xPosition][yPosition];
            //Now put the baby into any free slot here
            for (int m = 0; m < cellSettings[xPosition][yPosition].slotsPerSquare; m++)
            {
                Critter *crit2 = &(crit[m]);
                if (crit2->age == 0)
                {
                    //place it
                    quint8 *environment = simulationManager->env->returnRGB(xPosition, yPosition);
                    crit2->initialise(newgenomes[n], environment, xPosition, yPosition, m, newgenomespecies[n], newgenometrophic[n]);
                    crit2->calculateBitCountWithJitter();
                    if (crit2->age)
                    {
                        int fit = crit2->fitness;
                        totalFitness[xPosition][yPosition] += fit;
                        (*birthCountsLocal)++;
                        if (m > maxUsed[xPosition][yPosition]) maxUsed[xPosition][yPosition] = m;
                        settles[xPosition][yPosition]++;
                    }
                    else settlefails[xPosition][yPosition]++;
                    break;
                }
            }
            mutexes[xPosition][yPosition]->unlock();
        }
    }
    return 0;
}

/*!
 * \brief SimManager::iterate
 * \param eMode
 * \param interpolate
 * \return
 */
bool SimManager::iterate(int eMode, bool interpolate)
{
    iteration++;

    //RJG - Set up the settings array, and then modify as required based on linkages
    populateCellSettings();
    if (simulationSettings->linkagesOn)
        //RJG - regenerate image sequence
        for (auto &l : linkages)
            if (l.set)
            {
                //RJG - settings for each pixel
                for (int n = 0; n < simulationSettings->gridX; n++)
                    for (int m = 0; m < simulationSettings->gridY; m++)
                    {
                        quint8 *RGB = l.imageSequence->returnRGB(n, m);
                        //RJG for now do this manually - if we need to add lots in the long run makes sense to change to a QHash or similar
                        if (l.variable == "Energy")cellSettings[n][m].food *= RGB[0];
                        else if (l.variable == "No_selection")
                        {
                            if (RGB[0])cellSettings[n][m].noSelection = true;
                        }
                        else if (l.variable == "Mutation_rate")
                        {
                            cellSettings[n][m].mutate *= RGB[0];
                            if (cellSettings[n][m].mutate > 255) cellSettings[n][m].mutate = 255;
                        }
                        else qInfo() << "Error in applying linkage to cell settings.";
                    }
                l.imageSequence->regenerate(l.mode, l.interpolate);
            }

    if (warningCount == 1 && !mainWindow->autoFromCommand)
    {
        if (!mainWindow->batchRunning)
        {
            if (QMessageBox::question(nullptr, "A choice awaits...",
                                      "The last species search took more than five seconds."
                                      " This suggests the settings you are using lend themselves towards speciation, and the species system is a bottleneck."
                                      " Would you like to switch off the species system? If you select no, a progress bar will appear to give you an idea of how long it is taking."
                                      "If you click yes, the system will be disabled. You will only see this warning once per run.",
                                      QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) == QMessageBox::Yes)
            {
                speciesMode = 0;
                mainWindow->updateGUIFromVariables();
            }
        }
        else
        {
            QString message("The last species search took more than five seconds."
                            " This suggests the settings you are using lend themselves towards speciation, and the species system is a bottleneck."
                            " This will slow stuff down a bit.");
            QMessageBox *warningBox = new QMessageBox;
            //Delete when closed so no memory leak
            warningBox->setAttribute(Qt::WA_DeleteOnClose, true);
            warningBox->setWindowTitle("FYI");
            warningBox->setText(message);
            warningBox->show();
            //Close after three minutes.
            QTimer::singleShot(180000, warningBox, SLOT(close()));
        }

        warningCount++;
    }
    else if (warningCount == 1 && mainWindow->autoFromCommand)
    {
        qInfo() << "FYI the species searches are taking more than five seconds each, so are significantly slowing your simulation down. Do they need to be on?";
    }

    if (env->regenerate(eMode, interpolate)) return true;

    //RJG - pathogens will need to change if they are to be applied on a cell-level basis
    if (iteration % cellSettingsMaster->pathogenFrequency == 0 && cellSettingsMaster->pathOn)temp_path_on = true;
    else temp_path_on = false;

    //New parallelised version
    int newgenomecounts_starts[256]; //allow for up to 256 threads
    int newgenomecounts_ends[256]; //allow for up to 256 threads

    //work out positions in genome array that each thread can write to to guarantee no overlap
    int positionadd = (GRID_X * GRID_Y * SLOTS_PER_GRID_SQUARE * 2) / ProcessorCount;
    for (int i = 0; i < ProcessorCount; i++) newgenomecounts_starts[i] = i * positionadd;

    int KillCounts[256];
    for (int i = 0; i < ProcessorCount; i++) KillCounts[i] = 0;

    //Create randomised list of cell coordinates for splitting among processors - ENF
    int celllist1[simulationSettings->gridX];
    int celllist2[simulationSettings->gridY];

    for (int n = 0; n < simulationSettings->gridX; n++)
    {
        celllist1[n] = n;
    }
    for (int m = 0; m < simulationSettings->gridY; m++)
    {
        celllist2[m] = m;
    }

    int transferredNumbers1 = 0;
    while (transferredNumbers1 < simulationSettings->gridX)
    {
        int randomIndex = simulationRandoms->rand16() % simulationSettings->gridX;
        if (celllist1[randomIndex] != -1)
        {
            celllist1[randomIndex] = -1;
            Randcelllist1[transferredNumbers1] = randomIndex;
            transferredNumbers1++;
        }
    }
    int transferredNumbers2 = 0;
    while (transferredNumbers2 < simulationSettings->gridY)
    {
        int randomIndex = simulationRandoms->rand16() % simulationSettings->gridY;
        if (celllist2[randomIndex] != -1)
        {
            celllist2[randomIndex] = -1;
            Randcelllist2[transferredNumbers2] = randomIndex;
            transferredNumbers2++;
        }
    }
    //do the magic! Set up futures objects, call the functions, wait till done, retrieve values

    for (int i = 0; i < ProcessorCount; i++)
        *(FuturesList[i]) = QtConcurrent::run(this, &SimManager::iterateParallel,
                                              (i * simulationSettings->gridX) / ProcessorCount, (((i + 1) * simulationSettings->gridX) / ProcessorCount) - 1, newgenomecounts_starts[i],
                                              &(KillCounts[i]));

    for (int i = 0; i < ProcessorCount; i++)
        FuturesList[i]->waitForFinished();

    for (int i = 0; i < ProcessorCount; i++)
        newgenomecounts_ends[i] = FuturesList[i]->result();

    //Testbed - call parallel functions, but in series
    /*
      for (int i=0; i<ProcessorCount; i++)
            newgenomecounts_ends[i]=SimManager::iterateParallel((i*simulationSettings->gridX)/ProcessorCount, (((i+1)*simulationSettings->gridX)/ProcessorCount)-1,newgenomecounts_starts[i], &(KillCounts[i]));
    */

    //apply all the kills to the global count
    for (int i = 0; i < ProcessorCount; i++)
        aliveCount -= KillCounts[i];

    //Currently pathogens is messing up aliveCount - localKillCounts seem to be too high, so number goes very negative. Bodge fix for now:
    int tmp_alive_cnt = 0;
    for (int n = 0; n < simulationSettings->gridX; n++)
        for (int m = 0; m < simulationSettings->gridY; m++)
            for (int c = 0; c < cellSettings[n][m].slotsPerSquare; c++)if (critters[n][m][c].age)tmp_alive_cnt++;
    aliveCount = tmp_alive_cnt;

    //Now handle spat settling

    int birthcounts[256];
    for (int i = 0; i < ProcessorCount; i++) birthcounts[i] = 0;

    //call the parallel settling function - in series for now
    /*    for (int i=0; i<ProcessorCount; i++)
            settleParallel(newgenomecounts_starts[i],newgenomecounts_ends[i],&(trycounts[i]), &(settlecounts[i]), &(birthcounts[i]));
    */

    //Parallel version of settle functions
    for (int i = 0; i < ProcessorCount; i++)
        *(FuturesList[i]) = QtConcurrent::run(this, &SimManager::settleParallel, newgenomecounts_starts[i], newgenomecounts_ends[i], &(birthcounts[i]));

    for (int i = 0; i < ProcessorCount; i++)
        FuturesList[i]->waitForFinished();

    //sort out all the counts
    for (int i = 0; i < ProcessorCount; i++)
    {
        aliveCount += birthcounts[i];
    }

    // ----RJG: Mutate pathogens if they are drifting - this needs to be done here to avoid multiple mutates.
    if (temp_path_on && simulationSettings->pathogenMode == PATH_MODE_DRIFT) pathogensSystem->mutate();

    return false;
}

/*!
 * \brief SimManager::debug_genome
 * \param genome
 * RJG - this is useful for debugging stuff with critters, and I'm a little bored of recoding it every time I need to print one to screen
 */
QString SimManager::printGenome(quint64 genome)
{
    QString newGenome;
    for (int i = 0; i < 64; i++)
    {
        if (tweakers64[63 - i] & genome) newGenome.append("1");
        else newGenome.append("0");
    }
    return newGenome;
}

QString SimManager::printGenome(quint32 genome)
{
    QString newGenome;
    for (int i = 0; i < 32; i++)
    {
        if (tweakers[31 - i] & genome) newGenome.append("1");
        else newGenome.append("0");
    }
    return newGenome;
}

