/**
 * @file
 * Simulation Manager
 *
 * All REvoSim code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All REvoSim code is Copyright 2008-2018 by Mark D. Sutton, Russell J. Garwood,
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

#include <cstdlib>
#include <cmath>
#include <QDebug>
#include <QImage>
#include <QMessageBox>
#include <QThread>

// Simulation variables
quint32 tweakers[32]; // the 32 single bit XOR values (many uses!)
quint64 tweakers64[64]; // the 64 bit version
quint32 bitCounts[65536]; // the bytes representing bit count of each number 0-635535
quint32 xorMasks[256][3]; // determine fitness
int dispersalX[256][256];
int dispersalY[256][256];
quint64 geneX[65536];
int nextGeneX;

// Settable ints
int gridX = 100;
int gridY = 100;
int slotsPerSquare = 100;
int startAge = 15;
int target = 66;
int settleTolerance = 15;
int dispersal = 15;
int food = 3000;
int breedThreshold = 500;
int breedCost = 500;
int maxDifference = 2;
int mutate = 10;
int environmentChangeRate = 100;
int speciesSamples = 1;
int speciesSensitivity = 2;
int timeSliceConnect = 5;
int currentEnvironmentFile;
int environmentChangeCounter;
quint64 lastReport = 0;
quint8 environmentMode;
quint64 minSpeciesSize;
quint64 reseedGenome = 0; // Genome for reseed with known genome

// Settable bools
bool recalculateFitness = false;
bool asexual = false;
bool sexual = true;
bool logging = false;
bool fitnessLoggingToFile = false;
bool nonspatial = false;
bool environmentInterpolate = true;
bool toroidal = false;
bool reseedKnown = false;
bool breedSpecies = false;
bool breedDifference = true;
bool gui = false;
bool allowExcludeWithDescendants;
bool environmentChangeForward;

// File handling
QStringList environmentFiles;
QString speciesLoggingFile = "";
QString fitnessLoggingFile = "";

// Global data
Critter critters[GRID_X][GRID_Y][SLOTS_PER_GRID_SQUARE]; //main array - static for speed
quint8 environment[GRID_X][GRID_Y][3];  //0 = red, 1 = green, 2 = blue
quint8 environmentLast[GRID_X][GRID_Y][3];  //Used for interpolation
quint8 environmentNext[GRID_X][GRID_Y][3];  //Used for interpolation
quint32 totalFittness[GRID_X][GRID_Y];
quint64 iteration;

// These next to hold the babies... old style arrays for max speed
quint64 newGenomes[GRID_X * GRID_Y * SLOTS_PER_GRID_SQUARE * 2];
quint32 newGenomeX[GRID_X * GRID_Y * SLOTS_PER_GRID_SQUARE * 2];
quint32 newGenomeY[GRID_X * GRID_Y * SLOTS_PER_GRID_SQUARE * 2];
int newGenomeDispersal[GRID_X * GRID_Y * SLOTS_PER_GRID_SQUARE * 2];
quint64 newGenomeSpecies[GRID_X * GRID_Y * SLOTS_PER_GRID_SQUARE * 2];
int newGenomeCount;

// Randoms
quint8 randoms[65536];
quint16 nextRandom = 0;

// Analysis
int breedAttempts[GRID_X][GRID_Y]; //for analysis purposes
int breedFails[GRID_X][GRID_Y]; //for analysis purposes
int settles[GRID_X][GRID_Y]; //for analysis purposes
int settleFails[GRID_X][GRID_Y]; //for analysis purposes
int maxUsed[GRID_X][GRID_Y];
int aliveCount;
int totalRecombination;

// Species stuff
QList<Species> oldSpeciesList;
QList< QList<Species> > archivedSpeciesLists; //no longer used?
LogSpecies *rootSpecies;
QHash<quint64, LogSpecies *> logSpeciesByID;
quint64 lastSpeciesCalculated = 0;
quint64 nextSpeciesID;
QList<uint> speciesColours;
quint8 speciesMode;
quint64 ids; //used in tree export -


QMutex *mutexes[GRID_X][GRID_Y]; //set up array of mutexes

/**
 * @brief SimManager::SimManager
 */
SimManager::SimManager()
{
    //Constructor - set up all the data!
    speciesMode = SPECIES_MODE_BASIC;
    environmentMode = ENV_MODE_LOOP;
    environmentInterpolate = true;
    makeLookups();
    aliveCount = 0;
    processorCount = QThread::idealThreadCount();

    if (processorCount == -1)
        processorCount = 1;

    //a sanity check
    if (processorCount > 256)
        processorCount = 256;

    for (auto &mutexe : mutexes)
        for (int j = 0; j < GRID_X; j++)
            mutexe[j] = new QMutex();

    for (int i = 0; i < processorCount; i++)
        futuresList.append(new QFuture<int>);


    environmentFiles.clear();
    currentEnvironmentFile = -1;
    environmentChangeCounter = 0;
    environmentChangeForward = true;
    nextSpeciesID = 1;
    rootSpecies = static_cast<LogSpecies *>(nullptr);

    warningCount = 0;
}

/**
 * @brief SimManager::portableRandom
 * @return
 */
int SimManager::portableRandom()
{
    //replacement for qrand to come with RAND_MAX !=32767

    if (RAND_MAX < 32767) {
        qDebug() << "RAND_MAX too low - it's " << RAND_MAX;
        exit(0);
    }
    if (RAND_MAX > 32767) {
        // assume it's (2^n)-1
        int r = qrand();
        return r & 32767; //mask off bottom 16 bits, return those
    }
    return qrand();
}

/**
 * @brief SimManager::makeLookups
 */
void SimManager::makeLookups()
{
    // These are 00000001, 000000010, 0000000100 etc
    tweakers[0] = 1;
    for (int n = 1; n < 32; n++)
        tweakers[n] = tweakers[n - 1] * 2;

    tweakers64[0] = 1;
    for (int n = 1; n < 64; n++)
        tweakers64[n] = tweakers64[n - 1] * 2;

    // and now the bitcounting...
    // set up lookup 0 to 65535 to enable bits to be counted for each
    for (quint32 n = 0; n < 65536; n++) {
        quint32 count = 0;
        for (int m = 0; m < 16; m++)
            if ((n & tweakers[m]) != 0)
                ++count; // count the bits
        bitCounts[n] = count;
    }

    //RJG - seed random from time qsrand(RAND_SEED);
    qsrand(static_cast<uint>(QTime::currentTime().msec()));

    //now set up xor masks for 3 variables - these are used for each of R G and B to work out fitness
    //Start - random bit pattern for each
    xorMasks[0][0] = static_cast<quint32>(portableRandom() * portableRandom() * 2);
    xorMasks[0][1] = static_cast<quint32>(portableRandom() * portableRandom() * 2);
    xorMasks[0][2] = static_cast<quint32>(portableRandom() * portableRandom() * 2);

    for (int n = 1; n < 256; n++) {
        //for all the others - flip a random bit each time (^ is xor) - will slowly modify from 0 to 255
        xorMasks[n][0] = xorMasks[n - 1][0] ^ tweakers[portableRandom() / (PORTABLE_RAND_MAX / 32)];
        xorMasks[n][1] = xorMasks[n - 1][1] ^ tweakers[portableRandom() / (PORTABLE_RAND_MAX / 32)];
        xorMasks[n][2] = xorMasks[n - 1][2] ^ tweakers[portableRandom() / (PORTABLE_RAND_MAX / 32)];
    }

    // Now the randoms - pre_rolled random numbers 0-255
    for (quint8 &random : randoms)
        random = static_cast<quint8>(((portableRandom() & 255)));
    nextRandom = 0;

    // Gene exchange lookup
    for (quint64 &n : geneX) {  //random bit combs, averaging every other bit on
        quint64 value = 0;
        for (quint64 m : tweakers64)
            if (portableRandom() > (PORTABLE_RAND_MAX / 2))
                value += m;
        n = value;
    }
    nextGeneX = 0;

    // Dispersal table - lookups for dispersal amount
    for (int distance = 0; distance < 256; distance++) {
        double newDistance = sqrt(65536 / static_cast<double>((distance + 1))) - 16;
        if (newDistance < 0)
            newDistance = 0;
        for (int angle = 0; angle < 256; angle++) {
            dispersalX[distance][angle] = static_cast<int>(newDistance * sin(static_cast<double>(angle) / 40.5845));
            dispersalY[distance][angle] = static_cast<int>(newDistance * cos(static_cast<double>(angle) / 40.5845));
        }
    }

    // Colours
    for (int i = 0; i < 65536; i++) {
        speciesColours.append(qRgb(random8(), random8(), random8()));
    }
}

/**
 * @brief SimManager::loadEnvironmentFromFile
 * @param emode
 */
void SimManager::loadEnvironmentFromFile(int emode)
// Load current envirnonment from file
{
    //Use make qimage from file method
    //Load the image
    if (currentEnvironmentFile >= environmentFiles.count()) {
        return;
    }
    QImage LoadImage(environmentFiles[currentEnvironmentFile]);

    if (LoadImage.isNull()) {
        QMessageBox::critical(nullptr, "Error", "Fatal - can't open image " + environmentFiles[currentEnvironmentFile]);
        exit(1);
    }
    //check size works
    int xsize = LoadImage.width();
    int ysize = LoadImage.height();

    if (xsize < gridX || ysize < gridY) //rescale if necessary - only if too small
        LoadImage = LoadImage.scaled(QSize(gridX, gridY), Qt::IgnoreAspectRatio);

    //turn into environment array
    for (int i = 0; i < gridX; i++)
        for (int j = 0; j < gridY; j++) {
            QRgb colour = LoadImage.pixel(i, j);
            environment[i][j][0] = static_cast<quint8>(qRed(colour));
            environment[i][j][1] = static_cast<quint8>(qGreen(colour));
            environment[i][j][2] = static_cast<quint8>(qBlue(colour));
        }

    //set up environmentLast - same as environment
    for (int i = 0; i < gridX; i++)
        for (int j = 0; j < gridY; j++) {
            QRgb colour = LoadImage.pixel(i, j);
            environmentLast[i][j][0] = static_cast<quint8>(qRed(colour));
            environmentLast[i][j][1] = static_cast<quint8>(qGreen(colour));
            environmentLast[i][j][2] = static_cast<quint8>(qBlue(colour));
        }
    //set up environment next - depends on emode

    if (emode == 0 || environmentFiles.count() == 1) { //static environment
        for (int i = 0; i < gridX; i++)
            for (int j = 0; j < gridY; j++) {
                QRgb colour = LoadImage.pixel(i, j);
                environmentNext[i][j][0] = static_cast<quint8>(qRed(colour));
                environmentNext[i][j][1] = static_cast<quint8>(qGreen(colour));
                environmentNext[i][j][2] = static_cast<quint8>(qBlue(colour));
            }
    } else {
        //work out next file
        int nextFile = 0;
        if (environmentChangeForward) {
            if ((currentEnvironmentFile + 1) < environmentFiles.count()) //not yet at end
                nextFile = currentEnvironmentFile + 1;
            else {
                //depends on emode
                if (emode == 1) nextFile = currentEnvironmentFile; //won't matter
                if (emode == 2) nextFile = 0; //loop mode
                if (emode == 3) nextFile = currentEnvironmentFile - 1; //bounce mode
            }
        } else { //backwards - simpler, must be emode 3
            if (currentEnvironmentFile > 0) //not yet at end
                nextFile = currentEnvironmentFile - 1;
            else
                nextFile = 1; //bounce mode
        }

        QImage LoadImage2(environmentFiles[nextFile]);
        if (xsize < gridX || ysize < gridY) //rescale if necessary - only if too small
            LoadImage2 = LoadImage2.scaled(QSize(gridX, gridY), Qt::IgnoreAspectRatio);
        //get it
        for (int i = 0; i < gridX; i++)
            for (int j = 0; j < gridY; j++) {
                QRgb colour = LoadImage2.pixel(i, j);
                environmentNext[i][j][0] = static_cast<quint8>(qRed(colour));
                environmentNext[i][j][1] = static_cast<quint8>(qGreen(colour));
                environmentNext[i][j][2] = static_cast<quint8>(qBlue(colour));
            }
    }
}

/**
 * @brief SimManager::regenerateEnvironment
 * @param emode
 * @param interpolate
 * @return returns true if finished simulation
 */
bool SimManager::regenerateEnvironment(int emode, bool interpolate)
{
    if (environmentChangeRate == 0 || emode == 0
            || environmentFiles.count() == 1) return
                    false; //constant environment - either static in menu, or 0 environmentChangeRate, or only one file

    --environmentChangeCounter;

    if (environmentChangeCounter <= 0)
        //is it time to do a full change?
    {
        if (emode != 3 && !environmentChangeForward) //should not be going backwards!
            environmentChangeForward = true;
        if (environmentChangeForward) {
            currentEnvironmentFile++; //next image
            if (currentEnvironmentFile >= environmentFiles.count()) {
                if (emode == 1) return true; //no more files and we are in 'once' mode - stop the sim
                if (emode == 2) currentEnvironmentFile = 0; //loop mode
                if (emode == 3) {
                    currentEnvironmentFile -= 2; //bounce mode - back two to undo the extra ++
                    environmentChangeForward = false;
                }
            }
        } else { //going backwards - must be in emode 3 (bounce)
            currentEnvironmentFile--; //next image
            if (currentEnvironmentFile < 0) {
                currentEnvironmentFile = 1; //bounce mode - one to one again, must have just been 0
                environmentChangeForward = true;
            }
        }
        environmentChangeCounter = environmentChangeRate; //reset counter
        loadEnvironmentFromFile(emode); //and load it from the file

    } else {
        if (interpolate) {
            float progress;
            float invprogress;
            invprogress = (static_cast<float>(environmentChangeCounter + 1) / (static_cast<float>(environmentChangeRate)));
            progress = 1 - invprogress;
            //not getting new, doing an interpolate
            for (int i = 0; i < gridX; i++)
                for (int j = 0; j < gridY; j++) {
                    environment[i][j][0] = static_cast<quint8>(static_cast<float>(0.5) + (static_cast<float>(environmentLast[i][j][0])) * invprogress + (static_cast<float>(environmentNext[i][j][0])) * progress);
                    environment[i][j][1] = static_cast<quint8>(static_cast<float>(0.5) + (static_cast<float>(environmentLast[i][j][1])) * invprogress + (static_cast<float>(environmentNext[i][j][1])) * progress);
                    environment[i][j][2] = static_cast<quint8>(static_cast<float>(0.5) + (static_cast<float>(environmentLast[i][j][2])) * invprogress + (static_cast<float>(environmentNext[i][j][2])) * progress);
                }
        }

    }
    return false;
}


/**
 * @brief SimManager::random64
 *
 * 64 bit random useful for initialising critters
 *
 * @return
 */
quint64 SimManager::random64()
{
    return static_cast<quint64>(random32()) + static_cast<quint64>(65536) * static_cast<quint64>(65536) * static_cast<quint64>(random32());
}

/**
 * @brief SimManager::random32
 * @return
 */
quint32 SimManager::random32()
{
    // 4 lots of random8
    quint32 rand1 = portableRandom() & 255;
    quint32 rand2 = (portableRandom() & 255) * 256;
    quint32 rand3 = (portableRandom() & 255) * 256 * 256;
    quint32 rand4 = (portableRandom() & 255) * 256 * 256 * 256;

    return rand1 + rand2 + rand3 + rand4;
}

/**
 * @brief SimManager::random8
 * @return
 */
quint8 SimManager::random8()
{
    return randoms[nextRandom++];
}

/**
 * @brief SimManager::setupRun
 */
void SimManager::setupRun()
{

    //Find middle square, try creatures till something lives, duplicate it [slots] times
    //RJG - called on initial program load and reseed, but also when run/run for are hit
    //RJG - with modification for dual seed if selected

    //Kill em all
    for (int n = 0; n < gridX; n++)
        for (int m = 0; m < gridY; m++) {
            for (int c = 0; c < slotsPerSquare; c++) {
                critters[n][m][c].age = 0;
                critters[n][m][c].fitness = 0;
            }
            totalFittness[n][m] = 0;
            maxUsed[n][m] = -1;
            breedAttempts[n][m] = 0;
            breedFails[n][m] = 0;
            settles[n][m] = 0;
            settleFails[n][m] = 0;
        }

    nextSpeciesID = 1; //reset id counter

    int n = gridX / 2;
    int m = gridY / 2;

    //RJG - Either reseed with known genome if set
    if (reseedKnown) {
        critters[n][m][0].initialise(reseedGenome, environment[n][m], n, m, 0, nextSpeciesID);
        if (critters[n][m][0].fitness == 0) {
            // RJG - But sort out if it can't survive...
            QMessageBox::warning(
                nullptr,
                "Oops",
                "The genome you're trying to reseed with can't survive in this environment. There could be a number of reasons why this is. Please contact RJG or MDS to discuss."
            );
            reseedKnown = false;
            setupRun();
            return;
        }

        //RJG - I think this is a good thing to flag in an obvious fashion.
        QString reseedGenomeString("Started simulation with known genome: ");
        for (quint64 i : tweakers64)
            if (i & reseedGenome)
                reseedGenomeString.append("1");
            else
                reseedGenomeString.append("0");

        mainWindow->setStatusBarText(reseedGenomeString);
    } else {
        while (critters[n][m][0].fitness < 1)
            critters[n][m][0].initialise(random64(), environment[n][m], n, m, 0, nextSpeciesID);
        mainWindow->setStatusBarText("");
    }

    totalFittness[n][m] = static_cast<quint32>(critters[n][m][0].fitness); //may have gone wrong from above

    aliveCount = 1;
    iteration = 0;

    quint64 iteration = critters[n][m][0].genome;

    //RJG - Fill square with successful critter
    for (int c = 1; c < slotsPerSquare; c++) {
        critters[n][m][c].initialise(iteration, environment[n][m], n, m, c, nextSpeciesID);

        if (critters[n][m][c].age > 0) {
            critters[n][m][c].age /= ((random8() / 10) + 1);
            critters[n][m][c].age += 10;
            aliveCount++;
            maxUsed[n][m] = c;
            totalFittness[n][m] += static_cast<quint32>(critters[n][m][c].fitness);
        }
    }

    environmentChangeCounter = environmentChangeRate;
    environmentChangeForward = true;

    //remove old species log if one exists
    delete rootSpecies;

    //create a new logspecies with appropriate first data entry
    rootSpecies = new LogSpecies;

    rootSpecies->maxSize = static_cast<quint32>(aliveCount);
    rootSpecies->id = nextSpeciesID;
    rootSpecies->timeOfFirstAppearance = 0;
    rootSpecies->timeOfLastAppearance = 0;
    rootSpecies->parent = static_cast<LogSpecies *>(nullptr);
    auto *newdata = new LogSpeciesDataItem;
    newdata->centroidRangeX = static_cast<quint8>(n);
    newdata->centroidRangeY = static_cast<quint8>(m);
    newdata->iteration = 0;
    newdata->cellsOccupied = 1;
    newdata->genomicDiversity = 1;
    newdata->size = static_cast<quint32>(aliveCount);
    newdata->geographicalRange = 0;
    newdata->cellsOccupied = 0; //=1, this is stored as -1
    newdata->sampleGenome = iteration;
    newdata->maxEnvironment[0] = environment[n][m][0];
    newdata->maxEnvironment[1] = environment[n][m][1];
    newdata->maxEnvironment[2] = environment[n][m][2];
    newdata->minEnvironment[0] = environment[n][m][0];
    newdata->minEnvironment[1] = environment[n][m][1];
    newdata->minEnvironment[2] = environment[n][m][2];
    newdata->meanEnvironment[0] = environment[n][m][0];
    newdata->meanEnvironment[1] = environment[n][m][1];
    newdata->meanEnvironment[2] = environment[n][m][2];
    newdata->meanFitness = static_cast<quint16>((totalFittness[n][m] * 1000) / static_cast<quint32>(aliveCount));

    rootSpecies->dataItems.append(newdata);
    logSpeciesByID.clear();
    logSpeciesByID.insert(nextSpeciesID, rootSpecies);

    //RJG - Depreciated, but clear here just in case
    archivedSpeciesLists.clear();

    oldSpeciesList.clear();
    Species newsp;
    newsp.id = nextSpeciesID;
    newsp.originTime = 0;
    newsp.parent = 0;
    newsp.size = slotsPerSquare;
    newsp.type = iteration;
    newsp.logSpeciesStructure = rootSpecies;
    oldSpeciesList.append(newsp);

    nextSpeciesID++; //ready for first species after this

    //RJG - reset warning system
    warningCount = 0;
}

/**
 * @brief SimManager::iterateParallel
 *
 * Parallel version - takes newGenomesLocal as the start point it can write to in main genomes array
 *
 * @param firstX
 * @param lastX
 * @param newGenomeCountLocal
 * @param killCountLocal
 * @return returns number of new genomes
 */
int SimManager::iterateParallel(int firstX, int lastX, int newGenomeCountLocal, int *killCountLocal)
{
    int breedlist[SLOTS_PER_GRID_SQUARE];
    int maxalive;
    int deathcount;

    for (int n = firstX; n <= lastX; n++)
        for (int m = 0; m < gridY; m++) {
            int maxv = maxUsed[n][m];

            Critter *crit = critters[n][m];

            if (recalculateFitness) {
                totalFittness[n][m] = 0;
                maxalive = 0;
                deathcount = 0;
                for (int c = 0; c <= maxv; c++) {
                    if (crit[c].age) {
                        quint32 f = static_cast<quint32>(crit[c].recalculateFitness(environment[n][m]));
                        totalFittness[n][m] += f;
                        if (f > 0) maxalive = c;
                        else deathcount++;
                    }
                }
                maxUsed[n][m] = maxalive;
                maxv = maxalive;
                (*killCountLocal) += deathcount;
            }

            // RJG - reset counters for fitness logging to file
            if (fitnessLoggingToFile || logging)breedAttempts[n][m] = 0;

            if (totalFittness[n][m]) { //skip whole square if needbe
                int addFood = 1 + static_cast<int>(static_cast<quint32>(food) / totalFittness[n][m]);

                int breedListEntries = 0;

                for (int c = 0; c <= maxv; c++)
                    if (crit[c].iterateParallel(killCountLocal, addFood))
                        breedlist[breedListEntries++] = c;

                // ----RJG: breedAttempts was no longer used - co-opting for fitness report.
                if (fitnessLoggingToFile || logging)breedAttempts[n][m] = breedListEntries;

                //----RJG Do breeding
                if (breedListEntries > 0) {
                    quint8 divider = static_cast<quint8>(255 / breedListEntries);
                    for (int c = 0; c < breedListEntries; c++) {
                        int partner;
                        bool temp_asexual = asexual;

                        if (temp_asexual)partner = c;
                        else partner = random8() / divider;

                        if (partner < breedListEntries) {
                            if (crit[breedlist[c]].breedWithParallel(n, m, &(crit[breedlist[partner]]), &newGenomeCountLocal))
                                breedFails[n][m]++; //for analysis purposes
                        } else //didn't find a partner, refund breed cost
                            crit[breedlist[c]].energy += breedCost;
                    }
                }

            }
        }

    return newGenomeCountLocal;
}

/**
 * @brief SimManager::settleParallel
 * @param newGenomeCountsStart
 * @param newGenomeCountsEnd
 * @param tryCountLocal
 * @param settleCountLocal
 * @param birthCountsLocal
 * @return
 */
int SimManager::settleParallel(int newGenomeCountsStart, int newGenomeCountsEnd, int *tryCountLocal, int *settleCountLocal, int *birthCountsLocal)
{
    if (nonspatial) {
        //settling with no geography - just randomly pick a cell
        for (int n = newGenomeCountsStart; n < newGenomeCountsEnd; n++) {
            quint64 xPosition = static_cast<quint64>(random32()) * static_cast<quint64>(gridX);
            xPosition /= static_cast<quint64>(65536) * static_cast<quint64>(65536);

            quint64 yPosition = static_cast<quint64>(random32()) * static_cast<quint64>(gridY);
            yPosition /= static_cast<quint64>(65536) * static_cast<quint64>(65536);

            mutexes[static_cast<int>(xPosition)][static_cast<int>(yPosition)]->lock(); //ensure no-one else buggers with this square
            (*tryCountLocal)++;
            Critter *crit = critters[static_cast<int>(xPosition)][static_cast<int>(yPosition)];
            //Now put the baby into any free slot here
            for (int m = 0; m < slotsPerSquare; m++) {
                Critter *crit2 = &(crit[m]);
                if (crit2->age == 0) {
                    //place it

                    crit2->initialise(newGenomes[n], environment[xPosition][yPosition], static_cast<int>(xPosition), static_cast<int>(yPosition), m, newGenomeSpecies[n]);
                    if (crit2->age) {
                        int fit = crit2->fitness;
                        totalFittness[xPosition][yPosition] += static_cast<quint32>(fit);
                        (*birthCountsLocal)++;
                        if (m > maxUsed[xPosition][yPosition]) maxUsed[xPosition][yPosition] = m;
                        settles[xPosition][yPosition]++;
                        (*settleCountLocal)++;
                    } else settleFails[xPosition][yPosition]++;
                    break;
                }
            }
            mutexes[xPosition][yPosition]->unlock();
        }
    } else {
        //old code - normal settling with radiation from original point
        for (int n = newGenomeCountsStart; n < newGenomeCountsEnd; n++) {
            //first handle dispersal

            quint8 t1 = random8();
            quint8 t2 = random8();

            int xPosition = (dispersalX[t1][t2]) / newGenomeDispersal[n];
            int yPosition = (dispersalY[t1][t2]) / newGenomeDispersal[n];
            xPosition += newGenomeX[n];
            yPosition += newGenomeY[n];


            if (toroidal) {
                //NOTE - this assumes max possible settle distance is less than grid size. Otherwise it will go tits up
                if (xPosition < 0) xPosition += gridX;
                if (xPosition >= gridX) xPosition -= gridX;
                if (yPosition < 0) yPosition += gridY;
                if (yPosition >= gridY) yPosition -= gridY;
            } else {
                if (xPosition < 0) continue;
                if (xPosition >= gridX)  continue;
                if (yPosition < 0)  continue;
                if (yPosition >= gridY)  continue;
            }

            mutexes[xPosition][yPosition]->lock(); //ensure no-one else buggers with this square
            (*tryCountLocal)++;
            Critter *crit = critters[xPosition][yPosition];
            //Now put the baby into any free slot here
            for (int m = 0; m < slotsPerSquare; m++) {
                Critter *crit2 = &(crit[m]);
                if (crit2->age == 0) {
                    //place it
                    crit2->initialise(newGenomes[n], environment[xPosition][yPosition], xPosition, yPosition, m, newGenomeSpecies[n]);
                    if (crit2->age) {
                        int fit = crit2->fitness;
                        totalFittness[xPosition][yPosition] += static_cast<quint32>(fit);
                        (*birthCountsLocal)++;
                        if (m > maxUsed[xPosition][yPosition])
                            maxUsed[xPosition][yPosition] = m;
                        settles[xPosition][yPosition]++;
                        (*settleCountLocal)++;
                    } else
                        settleFails[xPosition][yPosition]++;
                    break;
                }
            }
            mutexes[xPosition][yPosition]->unlock();

        }
    }
    return 0;
}

/**
 * @brief SimManager::iterate
 * @param emode
 * @param interpolate
 * @return
 */
bool SimManager::iterate(int emode, bool interpolate)
{
    iteration++;

    //RJG - Provide user with warning if the system is grinding through so many species it's taking>5 seconds.Option to turn off species mode.
    if (warningCount == 1) {
        if (QMessageBox::question(
                    nullptr,
                    "A choice awaits...",
                    "The last species search took more than five seconds."
                    " This suggests the settings you are using lend themselves towards speciation, and the species system is a bottleneck."
                    " Would you like to switch off the species system? If you select no, a progress bar will appear to give you an idea of how long it is taking."
                    "If you click yes, the system will be disabled. You will only see this warning once per run.",
                    QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) == QMessageBox::Yes
           ) {
            speciesMode = 0;
            mainWindow->updateGUIFromVariables();
        }
        warningCount++;
    }

    if (regenerateEnvironment(emode, interpolate)) return true;

    //New parallelised version

    int newgenomecounts_starts[256]; //allow for up to 256 threads
    int newgenomecounts_ends[256]; //allow for up to 256 threads

    //work out positions in genome array that each thread can write to to guarantee no overlap
    int positionadd = (GRID_X * GRID_Y * SLOTS_PER_GRID_SQUARE * 2) / processorCount;
    for (int i = 0; i < processorCount; i++)
        newgenomecounts_starts[i] = i * positionadd;

    int killCounts[256];
    for (int i = 0; i < processorCount; i++)
        killCounts[i] = 0;

    //do the magic! Set up futures objects, call the functions, wait till done, retrieve values

    for (int i = 0; i < processorCount; i++)
        *(futuresList[i]) = QtConcurrent::run(
                                this,
                                &SimManager::iterateParallel,
                                (i * gridX) / processorCount, (((i + 1) * gridX) / processorCount) - 1, newgenomecounts_starts[i],
                                &(killCounts[i])
                            );

    for (int i = 0; i < processorCount; i++)
        futuresList[i]->waitForFinished();

    for (int i = 0; i < processorCount; i++)
        newgenomecounts_ends[i] = futuresList[i]->result();

    //apply all the kills to the global count
    for (int i = 0; i < processorCount; i++)
        aliveCount -= killCounts[i];

    //Now handle spat settling
    int trycount = 0;
    int settlecount = 0;

    int trycounts[256];
    for (int i = 0; i < processorCount; i++)
        trycounts[i] = 0;
    int settlecounts[256];
    for (int i = 0; i < processorCount; i++)
        settlecounts[i] = 0;
    int birthcounts[256];
    for (int i = 0; i < processorCount; i++)
        birthcounts[i] = 0;

    //Parallel version of settle functions
    for (int i = 0; i < processorCount; i++)
        *(futuresList[i]) = QtConcurrent::run(
                                this,
                                &SimManager::settleParallel,
                                newgenomecounts_starts[i],
                                newgenomecounts_ends[i],
                                &(trycounts[i]),
                                &(settlecounts[i]),
                                &(birthcounts[i])
                            );

    for (int i = 0; i < processorCount; i++)
        futuresList[i]->waitForFinished();

    //sort out all the counts
    for (int i = 0; i < processorCount; i++) {
        aliveCount += birthcounts[i];
        trycount += trycounts[i];
        settlecount += settlecounts[i];
    }

    return false;
}

/**
 * @brief SimManager::testcode
 *
 * Use for any test with debugger, used to trigger from menu item
 *
 */
void SimManager::testcode()
{
    qDebug() << "Test code";
}


/**
 * @brief SimManager::debugGenome
 *
 * This is useful for debugging stuff with critters e.g. print one to screen
 *
 * @param genome
 */
void SimManager::debugGenome(quint64 genome)
{
    QString newGenome;
    for (int i = 0; i < 64; i++) {
        if (tweakers64[63 - i] & genome) newGenome.append("1");
        else newGenome.append("0");
    }
    qDebug() << newGenome;
}
