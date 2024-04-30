/**
 * @file
 * Analyser
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

#include "analyser.h"
#include "mainwindow.h"
#include "simmanager.h"
#include "globals.h"
#include "analysistools.h"
#include "genomehashtable.h"
#include "analyser.h"
#include "custommemorymanager.h"
#include "groupdata.h"

#include <QDebug>
#include <QHash>
#include <QHashIterator>
#include <QMessageBox>
#include <QSet>
#include <QTextStream>
#include <QTime>
#include <QFile>
#include <QTextStream>
#include <QMap>
#include <QApplication>
#include <QSet>
#include <cstring> //for memcpy
/**
 * @brief Species::Species
 *
 * This is the species class, which is used in the species identifier system
 */
Species::Species()
{
    //type = 0;  /not set in constructor anymore, would need a loop - set elsewhere :-)
    ID = 0; //default, not assigned
    internalID = -1;
    parent = 0;
    size = -1;
    genomeDiversity = -1;
    originTime = -1;
    fitness = -1;
    envFitness = -2;
    logSpeciesStructure = static_cast<LogSpecies *>(nullptr);
    totalEnergy = -1;
    totalStolenEnergy = -1;
    trophicLevel = -1;
    Ca = 0;
    Cr = 0;
    NCa = 0;
    NCr = 0;

    for (int i = 0; i < MAX_GENOME_WORDS; i++) for (int j = 0; j < 32; j++) frequenciesAtOrigination[i][j] = frequenciesLastIteration[i][j] = 0;
    for (int i = 0; i < MAX_GENOME_WORDS; i++) diversityPerWord[i]=0;

    //no need to initialise complexlogdata. I hope!
}

/*!
 * \brief Analyser::Analyser
 *
 * Does the species analysis. This is where the magic happens!
 */

//constructed every iteration
Analyser::Analyser()
{
    genomesTotalCount = 0;
    speciationLogging = false;
}


/*!
 * \brief Analyser::groupsGenealogicalTracker
 *
 * MDS - This is latest code that uses the genealogical tracking of species since last analysis.
 *
 * Algorithm is:
 *
 * 1. Go through all critters, making sets of genomes for each species, and recording
 * positions of all occurrences of each genome (for writing back changes). This is
 * termed 'gathering'. Uses custom genomehashtables for storage
 *
 * 2. For each species (parallelised by species)
 * 2a - Loop through all pairwise comparisons of genomes, looping from 0 to n-2 for
 * 'first', first+1 to n-1 for second. Each time round loop, if first and second are
 * close enough to group, place second into group of first, move on...; if second was
 * already in a group, update ALL of that group to group of first, and move on...
 * 2b - If only one group - do nothing except copy new species with new size if multiple
 * groups - create new species from those with fewer unique genomes.
 * 2c - Go through and write back new species IDs into cells.
 *
 * RJG - Regarding getting the data out of here, it worth noting:
 * -- groupsGenealogicalTracker_v3 includes the decleration of newSpeciesList which overwrites oldSpeciesList after the parallelised per species analysis code (which populated is) returns to the tracker
 * -- In perSpeciesAnalysis, we create some of the required tracking data, and then save that as part of the species data to the newSpeciesList
 * -- Whilst much of the per species data is populated iun this perSpeciesAnalysis function, we do not populate the species data complexLogData within this function
 * -- Rather, perSpeciesAnalysis calls doRunningLogs, which calculates the stuff for the complexLogData, and then this is written to the relevant species within the newSpeciesList
 * -- Note that complexLogData is an instance of structure LogSpeciesDataItem - in this case, it is part of the species class and is recorded here for the on the fly log
 * -- However, this structure is also used in the end run log - a list of these is stored in a LogSpecies data structure - one for each iteration the species is alive, also part of the species class.
 */

void Analyser::groupsGenealogicalTracker_v3()
{

    QElapsedTimer t;
    t.start(); //for debug/user warning timing purposes

    //debug string for reporting timeings
    QString outstring = "_V3 SpeciesID timings: ";
    qint64 last = 0;

    //build new species list here - eventually replaces old species list
    newSpeciesList = new QList<Species>();

    //set up futures list first time for parallel processing
    if (FuturesList.length() == 0)
        for (int i = 0; i < simulationManager->ProcessorCount; i++) FuturesList.append(new QFuture<void>);

    //set up custom hash tables for all species
    allTables.clear();
    foreach(Species s, *oldSpeciesList) allTables.insert(s.ID, new GenomeHashTable(0)); //0 is level

    //stage 1- gather all genomes into custom hash table structure
    gatherGenomesParallel(0, simulationManager->simulationSettings->gridX - 1);
    outstring += QString("Gathering: %1ms  ").arg(t.elapsed());
    last = t.elapsed();


    //MDS - Next. Go through each species and do all the pairwise comparisons. This is stage 2 above.
    //RJG - this is the really time consuming bit of the process especially when many species. Add progress bar if it's slow.

    //set up the queue for parallel per species algorithm
    speciesQueue.clear();
    foreach (Species s, *oldSpeciesList)
    {
        Species *sp = new Species();
        *sp = s;
        speciesQueue.enqueue(sp);
    }
    int processorCount = simulationManager->ProcessorCount;

    //set them going. Each one pulls one from the queue and does it - until queue is done
    for (int i = 0; i < processorCount; i++)
        *(FuturesList[i]) = QtConcurrent::run(this, &Analyser::speciesAnalysisWorker, i);

    for (int i = 0; i < processorCount; i++)
        FuturesList[i]->waitForFinished();

    outstring += QString("PerSpecies: %1ms  ").arg(t.elapsed() - last);
    last = t.elapsed();

    delete oldSpeciesList;
    oldSpeciesList = newSpeciesList; //copy new list over old one

    //Existing code from ENF - not modified except trivially. Runs pretty fast
    QMap<int, int> IDFitnessDict;
    QMap<int, int> IDEnvfitDict;
    QMap<int, int> IDRunningEnergyDict;
    QMap<int, int> IDRunningStolenEnergyDict;
    QMap<int, float> IDTrophicLevelDict;
    QMap<int, int> IDPopulationDict;
    IDFitnessDict.clear();
    IDEnvfitDict.clear();
    IDRunningEnergyDict.clear();
    IDRunningStolenEnergyDict.clear();
    IDTrophicLevelDict.clear();
    IDPopulationDict.clear();

    for (int f = 0; f < oldSpeciesList->count(); f++) // Populate IDFitnessDict and IDEnvfitDict
    {
        IDFitnessDict[oldSpeciesList->at(f).ID] = 0;
        IDEnvfitDict[oldSpeciesList->at(f).ID] = 0;
        IDRunningEnergyDict[oldSpeciesList->at(f).ID] = 0;
        IDRunningStolenEnergyDict[oldSpeciesList->at(f).ID] = 0;
        IDTrophicLevelDict[oldSpeciesList->at(f).ID] = 0;
        IDPopulationDict[oldSpeciesList->at(f).ID] = 0;
    }

    for (int n = 0; n < simulationManager->simulationSettings->gridX; n++) // Loop over cells - update sum species fitnesses in my silly array
    {
        for (int m = 0; m < simulationManager->simulationSettings->gridY; m++)
        {
            for (int c = 0; c < simulationManager->cellSettingsMaster->slotsPerSquare; c++)
            {
                if (critters[n][m][c].age > 0)   //if critter is alive
                {
                    IDFitnessDict.insert(critters[n][m][c].speciesID, (IDFitnessDict.value(critters[n][m][c].speciesID) + critters[n][m][c].fitness));
                    IDEnvfitDict.insert(critters[n][m][c].speciesID, (IDEnvfitDict.value(critters[n][m][c].speciesID) + critters[n][m][c].environmentalFitness));
                    IDRunningEnergyDict.insert(critters[n][m][c].speciesID, (IDRunningEnergyDict.value(critters[n][m][c].speciesID) + critters[n][m][c].lifetimeEnergy));
                    IDRunningStolenEnergyDict.insert(critters[n][m][c].speciesID, (IDRunningStolenEnergyDict.value(critters[n][m][c].speciesID) + critters[n][m][c].stolenEnergy));
                    IDTrophicLevelDict.insert(critters[n][m][c].speciesID, (IDTrophicLevelDict.value(critters[n][m][c].speciesID) + critters[n][m][c].trophicLevel));
                    IDPopulationDict.insert(critters[n][m][c].speciesID, (IDPopulationDict.value(critters[n][m][c].speciesID) + 1));
                }
            }
        }
    }

    for (int f = 0; f < IDFitnessDict.size(); f++) // Lets see if we can make fitness identification work in its own bit instead
    {
        (*oldSpeciesList)[f].fitness = (IDFitnessDict.value(oldSpeciesList->at(f).ID)) / (IDPopulationDict.value(oldSpeciesList->at(f).ID));
        (*oldSpeciesList)[f].envFitness = (IDEnvfitDict.value(oldSpeciesList->at(f).ID)) / (IDPopulationDict.value(oldSpeciesList->at(f).ID));
        (*oldSpeciesList)[f].totalEnergy = (IDRunningEnergyDict.value(oldSpeciesList->at(f).ID)) / (IDPopulationDict.value(oldSpeciesList->at(f).ID));
        (*oldSpeciesList)[f].totalStolenEnergy = (IDRunningStolenEnergyDict.value(oldSpeciesList->at(f).ID)) / (IDPopulationDict.value(oldSpeciesList->at(f).ID));
        (*oldSpeciesList)[f].trophicLevel = (IDTrophicLevelDict.value(oldSpeciesList->at(f).ID)) / (IDPopulationDict.value(oldSpeciesList->at(f).ID));
    }

    outstring += QString("EuanFitness: %1ms  ").arg(t.elapsed() - last);
    last = t.elapsed();

    //clean up - recursively delete the custom tables, and reset custom memory manager
    qDeleteAll(allTables);
    customMemoryManager->reset();

    outstring += QString("Cleanup: %1ms    TOTAL: %2ms").arg(t.elapsed() - last).arg(t.elapsed());

    //Uncomment to report speeds
    qDebug() << outstring;
}

Analyser::~Analyser()
{
    qDeleteAll(FuturesList);
}

//Worker method for each thread for perSpecies analysis
//threadID can be useful for debugging, but not used otherwise
void Analyser::speciesAnalysisWorker(int threadID)
{
    Q_UNUSED(threadID);
    while (true)
    {
        Species *theSpecies = getNextSpeciesToWorkOn();
        if (theSpecies == nullptr) //no more to do
            return;

        perSpeciesAnalyis(theSpecies);
        delete theSpecies;
    };
}

//Extract a species from the queue - mutex locked
Species *Analyser::getNextSpeciesToWorkOn()
{
    QMutexLocker locker(&speciesQueueMutex);

    if (speciesQueue.empty())
        return nullptr; //all done
    else
    {
        Species *theSpecies = speciesQueue.dequeue();
        return theSpecies;
    }
}

//used in logging system
void Analyser::recordFrequencies(Species *sp, GenomeHashTable *spTable, int group, bool firstFind)
{
    float temporaryFrequencies[MAX_GENOME_WORDS][32];
    //initialise
    for (int i = 0; i < simulationManager->simulationSettings->genomeSize; i++)
        for (int j = 0; j < 32; j++)
            temporaryFrequencies[i][j] = 0;

    //this enters  the frequencies and also calculates a total
    float total = spTable->sumFrequencies(temporaryFrequencies, group);

    //divide by total!
    for (int i = 0; i < simulationManager->simulationSettings->genomeSize; i++)
        for (int j = 0; j < 32; j++)
            temporaryFrequencies[i][j] /= total;

    if (firstFind) //first time - so also copy to origination
        for (int i = 0; i < simulationManager->simulationSettings->genomeSize; i++)
            for (int j = 0; j < 32; j++)
                sp->frequenciesAtOrigination[i][j]  = temporaryFrequencies[i][j];


    //Now - calculate Ca, NCa, Cr, NCr.

    //Ca, Cr
    double Ca = 0;
    double Cr = 0;

    if (!firstFind) for (int i = 0; i < simulationManager->simulationSettings->genomeSize; i++)
        {
            if (simulationManager->environmentalFitnessSytem->returnGenomeWordInUse(i))
            {
                for (int j = 0; j < 32; j++)
                {
                    Ca += qAbs(temporaryFrequencies[i][j] - sp->frequenciesAtOrigination[i][j]);
                    Cr += qAbs(temporaryFrequencies[i][j] - sp->frequenciesLastIteration[i][j]);
                }
            }
        }

    //NCa, NCr
    //NC in new system means 'in breeding but not in environmental fitness'
    double NCa = 0;
    double NCr = 0;
    if (!firstFind) for (int i = 0; i < simulationManager->simulationSettings->genomeSize; i++)
        {
            if (!simulationManager->environmentalFitnessSytem->returnGenomeWordInUse(i) && simulationManager->breedSystem->returnGenomeWordInUse(i))
            {
                for (int j = 0; j < 32; j++)
                {
                    NCa += qAbs(temporaryFrequencies[i][j] - sp->frequenciesAtOrigination[i][j]);
                    NCr += qAbs(temporaryFrequencies[i][j] - sp->frequenciesLastIteration[i][j]);
                }
            }
        }

    sp->Ca = Ca;
    sp->Cr = Cr;
    sp->NCa = NCa;
    sp->NCr  = NCr;

    //copy into lastIteration in species
    for (int i = 0; i < simulationManager->simulationSettings->genomeSize; i++)
        for (int j = 0; j < 32; j++)
            sp->frequenciesLastIteration[i][j]  = temporaryFrequencies[i][j];

    //And finally, work out diversity by word
    QSet<quint32> *diversitySet = new QSet<quint32>();
    for (int i = 0; i < simulationManager->simulationSettings->genomeSize; i++)
    {
        spTable->getSetForOneWordFromAllGenomes(diversitySet, group, i);
        sp->diversityPerWord[i]=diversitySet->count();
        qDebug()<<"Species "<<sp->ID<<" diversity for word "<<i<< " is "<<sp->diversityPerWord[i];
        diversitySet->clear();
    }
    delete diversitySet;
}

//mutex locked operation from parallel processing - in theory two processes might
//try to add a new species at the same time
void Analyser::addSpeciesToList(Species s)
{
    QMutexLocker locker(&addSpeciesToListMutex);
    newSpeciesList->append(s);
}

//mutex locked operation from parallel processing - in theory two processes might
//try to get a new speciesID at the same time
int Analyser::getNextSpeciesID()
{
    QMutexLocker locker(&getNextSpeciesIDMutex);
    nextSpeciesID++;
    return nextSpeciesID - 1;
}

//Another mutex-locked bit of parallel algorithm
void Analyser::addLogSpecies(quint64 speciesID, LogSpecies *logSpecies)
{
    QMutexLocker locker(&addLogSpeciesMutex);
    logSpeciesByID.insert(speciesID, logSpecies);
}


//used internally in the next two methods


//Do the per-species work - called from parallel worker method
int Analyser::perSpeciesAnalyis(Species *s)
{
    //pull some data
    quint64 thisSpeciesID = s->ID;
    GenomeHashTable *thisSpecies = allTables.value(thisSpeciesID);

    //find log species entry
    LogSpecies *thislogspecies = nullptr;
    if (speciesMode >= SPECIES_MODE_PHYLOGENY)
    {
        //lock the mutex - or in theory a species might be being inserted while we are trying to read this one
        addLogSpeciesMutex.lock();
        thislogspecies = logSpeciesByID.value(thisSpeciesID, static_cast<LogSpecies *>(nullptr));
        addLogSpeciesMutex.unlock();
        if (!thislogspecies)
        {
            QMessageBox::warning(mainWindow, "Oops", "Internal error - species not found in log hash. Please email " + QString(EMAIL) + " with this message or go to " + QString(GITURL) + QString(
                                     GITREPOSITORY) + QString(GITISSUE));
            return 0;
        }
    }

    //This is the bottleneck :-(
    int maxcode = generateGroupsFor(thisSpecies); //do the pairwise comparison to place all genomes into groups

    //maxcode is the highest group number made
    if (maxcode == -1) //species not found - it's extinct
    {
        //we don't need to do anything. It won't get added to new list - we can just stop
        return 1;
    }

    if (maxcode < -1)
    {
        qDebug() << "Error from generate groups - this is likely fatal - but continuing anyway for shits and giggles";
        return 0;
    }

    //everything should be in groups - if there is more than one we need to split the species
    //one group will be old species, others will become new species

    //first - gather some per group data, in an inline class
    int maxcountkey = -1;   //will be group ID for most diverse group
    int maxcount = -1;

    QHash<qint32, GroupData *> groups; //key is group code
    for (int i = 0; i < thisSpecies->bins; i++)
        for (int j = 0; j < thisSpecies->binData.at(i).genomeEntries; j++)
        {
            SpeciesBinEntry *entry = thisSpecies->entryByIndex(i, j);
            GroupData *gd = groups.value(entry->group, nullptr); //get number of genomes, or 0 if no entry
            if (gd == nullptr) gd = new GroupData(entry->group);
            int thiscount = gd->genomeCount++;
            if (thiscount > maxcount)
            {
                maxcount = thiscount;
                maxcountkey = entry->group;
            }
            gd->occurrenceCount += entry->occurrenceCount;
            if (gd->modalGenome == nullptr) gd->modalGenome = entry;
            else
            {
                if (gd->modalGenome->occurrenceCount < entry->occurrenceCount) gd->modalGenome = entry;
            }
            groups[entry->group] = gd;
        }

    //groups hash now complete
    //We now go through these groups and sort out species data, also writing back new species IDs to
    //critters cells for any new species

    foreach (GroupData *g, groups)
    {
        if (g->groupID != maxcountkey)   //if this ISN'T the one we picked to keep the old id
        {
            //WE ARE MAKING A NEW SPECIES HERE!!
            Species newsp;          //new species object
            newsp.size = g->occurrenceCount;
            newsp.genomeDiversity = g->genomeCount;

            int newspID = getNextSpeciesID();

            //copy in modal genome
            std::memcpy(&newsp.type, g->modalGenome->genome, 4 * simulationManager->simulationSettings->genomeSize);

            //do frequencies if needed
            if (speciesMode == SPECIES_MODE_PHYLOGENY_AND_METRICS)
            {
                recordFrequencies(&newsp, thisSpecies, g->groupID, true);
            }
            //go through all genome entries...
            //find all genome entries for this group
            //and fix data in critters for them
            for (int i = 0; i < thisSpecies->bins; i++)
                for (int j = 0; j < thisSpecies->binData.at(i).genomeEntries; j++)
                {
                    //If doing detailed logging - need to add pointers to all genomes to groups
                    SpeciesBinEntry *entry = thisSpecies->entryByIndex(i, j);
                    //retrieve the list of positions for this genome
                    if (entry->group == g->groupID)
                    {
                        //retrieve the list of positions for this genome
                        foreach (quint32 v, entry->positions)   //go through list and set critters data to new species
                        {
                            int x = v / 65536;
                            int ls = v % 65536;
                            int y = ls / 256;
                            int z = ls % 256;
                            critters[x][y][z].speciesID = newspID;
                        }
                        if (speciesMode == SPECIES_MODE_PHYLOGENY_AND_METRICS) g->allGenomes.append(entry);
                    }
                }

            newsp.parent = thisSpeciesID; //parent is the species we are splitting from
            newsp.originTime = static_cast<int>(simulationManager->iteration); //i.e. now (iteration is a global)
            newsp.ID = newspID;
            g->speciesID = newspID;

            //Logging code - NOT MODIFIED EXCEPT TRIVIALLY
            if (speciesMode >= SPECIES_MODE_PHYLOGENY)
            {
                //sort out the logspecies object
                auto *newlogspecies = new LogSpecies;
                auto *newdata = new LogSpeciesDataItem;
                newdata->iteration = simulationManager->iteration;

                newlogspecies->ID = nextSpeciesID;
                newlogspecies->timeOfFirstAppearance = simulationManager->iteration;
                newlogspecies->timeOfLastAppearance = simulationManager->iteration;
                newlogspecies->parent = thislogspecies;
                newlogspecies->maxSize = static_cast<quint32>(newsp.size);
                if (thislogspecies != nullptr) thislogspecies->children.append(newlogspecies);

                newlogspecies->dataItems.append(newdata);
                addLogSpecies(newspID, newlogspecies);
                newsp.logSpeciesStructure = newlogspecies;

                g->logSpecies = newlogspecies;

            }
            addSpeciesToList(newsp);
        }
        else //this is the continuing species
        {
            //copy s from the old list
            Species newsp = *s;
            g->speciesID = s->ID;
            //Logging code - not changed
            if (speciesMode >= SPECIES_MODE_PHYLOGENY)
            {
                g->logSpecies = newsp.logSpeciesStructure;
                newsp.logSpeciesStructure->timeOfLastAppearance = simulationManager->iteration;
                auto *newdata = new LogSpeciesDataItem;
                newdata->iteration = simulationManager->iteration;
                if (static_cast<quint32>(g->occurrenceCount) > newsp.logSpeciesStructure->maxSize) newsp.logSpeciesStructure->maxSize = g->occurrenceCount; //fix max size if needbe
                newsp.logSpeciesStructure->dataItems.append(newdata);
            }

            if (speciesMode == SPECIES_MODE_PHYLOGENY_AND_METRICS)
            {
                for (int i = 0; i < thisSpecies->bins; i++)
                    for (int j = 0; j < thisSpecies->binData.at(i).genomeEntries; j++)
                    {
                        SpeciesBinEntry *entry = thisSpecies->entryByIndex(i, j);
                        if (entry->group == g->groupID) g->allGenomes.append(entry);
                    }
            }

            //do frequencies if needed
            if (speciesMode == SPECIES_MODE_PHYLOGENY_AND_METRICS)
                recordFrequencies(&newsp, thisSpecies, g->groupID, false);

            //copy the modal genome into species
            std::memcpy(&newsp.type, g->modalGenome->genome, 4 * simulationManager->simulationSettings->genomeSize);

            newsp.size = g->occurrenceCount;
            newsp.genomeDiversity = g->genomeCount;


            //and put copied species (with new type) into the new species list
            addSpeciesToList(newsp);
        }
    }

    doRunningLogs(&groups, s);
    qDeleteAll(groups);
    return 1;
}

void Analyser::doRunningLogs(QHash<qint32, GroupData *> *groups, Species *thisSpecies)
{
    Q_UNUSED(thisSpecies);

    QVector<QVector<QVector<int> > > speciationLogDataStructure;

    if (speciesMode == SPECIES_MODE_PHYLOGENY_AND_METRICS)
    {
        int speciationLogArrayLocal[GRID_X][GRID_Y] = {{0}};

        int listpos = -1; //position in newSpeciesList of species (for genefrequency version). Just increment each time!
        //record a load of stuff

        //RJG Speciation - create a vector to record species sizes for writing.
        QVector<quint64> speciesSizes;
        //And species ID
        QVector<int> speciesIDs;
        //And our data structure
        for (int i = 0; i < simulationManager->simulationSettings->gridX; i++)
        {
            speciationLogDataStructure.append(QVector <QVector <int> >());
            for (int j = 0; j < simulationManager->simulationSettings->gridY; j++)speciationLogDataStructure[i].append(QVector <int> ());
        }

        //RJG - assuming that this represents the groups within one species - is this the case? - MDS yes!
        foreach (GroupData *g, *groups)
        {
            listpos++;
            quint64 speciesSize = g->occurrenceCount;
            quint32 *sampleGenome = g->modalGenome->genome;
            QSet<quint16> cellsoc;
            quint64 sumfit = 0;

            int mincol[3];
            mincol[0] = 256;
            mincol[1] = 256;
            mincol[2] = 256;

            int maxcol[3];
            maxcol[0] = -1;
            maxcol[1] = -1;
            maxcol[2] = -1;

            quint64 sumcol[3];
            sumcol[0] = 0;
            sumcol[1] = 0;
            sumcol[2] = 0;

            quint64 sumxpos = 0, sumypos = 0;
            int minx = 256;
            int maxx = -1;
            int maxy = -1;
            int miny = 256;

            quint32 genomicDiversity = g->genomeCount;

            //RJG - Speciation logging - here we have a local 2D array here to record where  *this* species is at speciation
            for (int n = 0; n < simulationManager->simulationSettings->gridX; n++)
                for (int m = 0; m < simulationManager->simulationSettings->gridY; m++)
                    speciationLogArrayLocal[n][m] = 0;

            //RJG - set species ID - this is updated within groups before this function is called, so represents new species number
            int speciesIDForLog = g->speciesID;

            //RJG - here we do local counts of where members of each group (now a new species) are - this seems to work get decent numbers for individuals
            //for all occupied cells - using list of positions from bin entries pointed to by group
            foreach (SpeciesBinEntry *sbe, g->allGenomes)
                foreach (quint32 v, sbe->positions)
                {
                    int x = v / 65536;
                    int ls = v % 65536;
                    int y = ls / 256;
                    int z = ls % 256;

                    sumxpos += static_cast<quint64>(x);
                    sumypos += static_cast<quint64>(y);
                    if (x < minx) minx = static_cast<int>(x);
                    if (y < miny) miny = static_cast<int>(y);
                    if (x > maxx) maxx = static_cast<int>(x);
                    if (y < maxy) maxy = static_cast<int>(y);

                    //RJG - Speciation logging - add one to count in *LOCAL* array at X,Y
                    if (speciationLogging)
                        speciationLogArrayLocal[x][y]++;

                    sumfit += static_cast<quint64>(critters[x][y][z].fitness);
                    cellsoc.insert(static_cast<quint16>(x) * static_cast<quint16>(256) + static_cast<quint16>(y));

                    quint8 *environment = simulationManager->env->returnRGB(x, y);
                    quint8 r = environment[0];
                    quint8 g = environment[1];
                    quint8 b = environment[2];

                    if (r < mincol[0]) mincol[0] = r;
                    if (g < mincol[1]) mincol[1] = g;
                    if (b < mincol[2]) mincol[2] = b;

                    if (r > maxcol[0]) maxcol[0] = r;
                    if (g > maxcol[1]) maxcol[1] = g;
                    if (b > maxcol[2]) maxcol[2] = b;

                    sumcol[0] += static_cast<quint64>(r);
                    sumcol[1] += static_cast<quint64>(g);
                    sumcol[2] += static_cast<quint64>(b);

                    //TODO - allow for toroidal!
                    //to do - put correct data in this item

                    //newdata->centroidRangeX=n; // mean of all x's - but allow for toroidal somehow
                    //newdata->centroidRangeY=m;
                    //newdata->geographicalRange=0;  //max (max-min x, max-min y), but allow for toroidal

                }

            //RJG - if this group is present in n,m, append it to the species log structure
            //RJG - Speciation logging - if count >0 in local array this species is in that cell, so record that ID in our data structure
            if (speciationLogging)
            {
                for (int n = 0; n < simulationManager->simulationSettings->gridX; n++)
                    for (int m = 0; m < simulationManager->simulationSettings->gridY; m++)
                        if (speciationLogArrayLocal[n][m] > 0)
                            speciationLogDataStructure[n][m].append(speciesIDForLog);

                speciesSizes.append(speciesSize);
                speciesIDs.append(speciesIDForLog);
            }

            //add data to phylogeny log (accruing in memory)
            LogSpecies *thislogspecies = g->logSpecies;
            LogSpeciesDataItem *thisdataitem = thislogspecies->dataItems.last();

            thisdataitem->genomicDiversity = genomicDiversity;
            thisdataitem->meanFitness = static_cast<quint16>((sumfit * 1000) / speciesSize);
            //copy in the multiword genome
            for (int iii = 0; iii < simulationManager->simulationSettings->genomeSize; iii++)
                thisdataitem->sampleMultiWordGenome[iii] = sampleGenome[iii];

            thisdataitem->size = static_cast<quint32>(speciesSize);
            thisdataitem->cellsOccupied = static_cast<quint16>(cellsoc.count());
            thisdataitem->maxEnvironment[0] = static_cast<quint8>(maxcol[0]);
            thisdataitem->maxEnvironment[1] = static_cast<quint8>(maxcol[1]);
            thisdataitem->maxEnvironment[2] = static_cast<quint8>(maxcol[2]);
            thisdataitem->minEnvironment[0] = static_cast<quint8>(mincol[0]);
            thisdataitem->minEnvironment[1] = static_cast<quint8>(mincol[1]);
            thisdataitem->minEnvironment[2] = static_cast<quint8>(mincol[2]);
            thisdataitem->meanEnvironment[0] = static_cast<quint8>(sumcol[0] / speciesSize);
            thisdataitem->meanEnvironment[1] = static_cast<quint8>(sumcol[1] / speciesSize);
            thisdataitem->meanEnvironment[2] = static_cast<quint8>(sumcol[2] / speciesSize);
            thisdataitem->centroidRangeX = static_cast<quint8>(sumxpos / speciesSize);
            thisdataitem->centroidRangeY = static_cast<quint8>(sumypos / speciesSize);
            thisdataitem->geographicalRange = static_cast<quint8>(qMax(maxx - minx, maxy - miny));

            //RJG 2023 - copy over new species data to newSpeciesList
            addSpeciesToListMutex.lock(); //MDS - added mutex locker for list access
            for (auto &s : *newSpeciesList)
                if (s.ID == static_cast<quint64>(g->speciesID))
                    s.complexLogData = *thisdataitem;
            addSpeciesToListMutex.unlock();  //Look, look, I remembered to unlock it!!
        }

        //RJG - Speciation logging - write counts from  non local array to file here if length of species size is more than 1 (i.e. there has been speciation)
        if (speciationLogging && speciesSizes.length() > 1)
        {
            int count = 0;
            for (int i = 0; i < speciesSizes.length(); i++)
                if (speciesSizes[i] > simulationManager->simulationSettings->minSpeciesSize)
                    count++;
            if (count > 1)
            {
                QImage populationImage(simulationManager->simulationSettings->gridX, simulationManager->simulationSettings->gridY, QImage::Format_RGB32);

                //MUTEX LOCK THIS! speciationLogText is a global
                speciationLogTextMutex.lock();

                QTextStream out (&speciationLogText);
                QHash<int, int> counts;
                QHash<int, int> soleCounts;

                for (int m = 0; m < simulationManager->simulationSettings->gridY; m++)
                {
                    for (int n = 0; n < simulationManager->simulationSettings->gridX; n++)
                    {
                        //Code to output a text grid, if required
                        //out << speciationLogDataStructure[n][m].length();
                        //if (n < simulationManager->simulationSettings->gridY - 1) out << ",";
                        int speciesPresent = 0;
                        for (int i = 0; i < speciationLogDataStructure[n][m].length(); i++)
                        {
                            speciesPresent += speciationLogDataStructure[n][m][i];
                            if (counts.find(speciationLogDataStructure[n][m][i]) == counts.end())
                                counts.insert(speciationLogDataStructure[n][m][i], 1);
                            else
                                counts.insert(speciationLogDataStructure[n][m][i], ++counts[speciationLogDataStructure[n][m][i]]);
                        }

                        if (speciesPresent == 0)
                            populationImage.setPixel(n, m, 0);
                        else
                        {
                            if (speciationLogDataStructure[n][m].length() > 1)
                            {
                                populationImage.setPixel(n, m, qRgb(255, 255, 255));
                                if (counts.find(-1) == counts.end())
                                    counts.insert(-1, 1);
                                else
                                    counts.insert(-1, ++counts[-1]);
                            }
                            else
                            {
                                populationImage.setPixel(n, m, speciesColours[speciesPresent % 65536]);
                                if (soleCounts.find(speciesPresent) == soleCounts.end())
                                    soleCounts.insert(speciesPresent, 1);
                                else
                                    soleCounts.insert(speciesPresent, ++soleCounts[speciesPresent]);
                            }
                        }
                    }
                    //out << "\n";
                }

                out << simulationManager->iteration << "," << speciesSizes.length() << "," << counts[-1] << ",";
                for (int i = 0; i < speciesSizes.length(); i++)
                {
                    out << speciesIDs[i] << "," << speciesSizes[i] << "," << counts[speciesIDs[i]] << "," << soleCounts[speciesIDs[i]];
                    if (i != (speciesSizes.length() - 1)) out << ",";
                }
                out << "\n";
                out.flush();
                speciationLogTextMutex.unlock();

                // Save needs to be here in order to save multiple images per iteration - for each speciation
                QString globalSavePathStr(mainWindow->getSavePath());
                if (!globalSavePathStr.endsWith(QDir::separator()))globalSavePathStr.append(QDir::separator());
                if (mainWindow->batchRunning)
                {
                    globalSavePathStr.append(QString("Images_run_%1").arg(mainWindow->getBatchRuns(), 4, 10, QChar('0')));
                    globalSavePathStr.append(QDir::separator());
                }
                QDir save_dir(globalSavePathStr);
                QString speciesIDsString;
                for (int i = 0; i < speciesIDs.length(); i++) speciesIDsString.append(QString("sp_%1_").arg(speciesIDs.at(i), 4, 10, QChar('0')));
                if (save_dir.mkpath("speciation/"))
                {
                    QString fname = QString(save_dir.path() + "/speciation/REvoSim_it_%1_" + speciesIDsString + ".png").arg(simulationManager->iteration, 7, 10, QChar('0'));

                    populationImage.save(fname);
                    //qDebug()<<"Saved to "<<fname;
                }
                else qDebug() << "Error making folder for speciation log.";
            }
        }

    }
}

//returns max group code used. If negative - error
//This is the core pairwise-comparison algorithm
int Analyser::generateGroupsFor(GenomeHashTable *thisSpecies)
{
    QVarLengthArray<qint32> grouplookupVLA;  //lookups are translation of groups into parent group codes
    QVarLengthArray<qint32> groupcodesVLA;   //group codes themselves - in a VLA so can be laid out sequentially
    grouplookupVLA.reserve(5000);  //guesses at good starting size for arrays
    groupcodesVLA.reserve(20000);

    int count = 0;
    int nextGroupCode = 0;
    int firstrealbin = -1;
    int lastrealbin = 100000;
    int md = simulationManager->simulationSettings->maxDifference;   //for speed
    int stride = thisSpecies->stride;                                //ditto - avoiding indirection in loop core
    SpeciesIDSystem *sis = simulationManager->speciesIDSytem;
    //initialise the VLAs, and find first and last real bins actually used
    for (int i = 0; i < thisSpecies->bins; i++)
    {
        if (thisSpecies->binData.at(i).genomeEntries > 0)
        {
            if (firstrealbin < 0) firstrealbin = i;
            lastrealbin = i;
        }
        for (int j = 0; j < thisSpecies->binData.at(i).genomeEntries; j++)
        {
            grouplookupVLA.append(count++);
            groupcodesVLA.append(-1);
        }
    }

    //lookups can no longer grow - so just get their pointers so use further down
    qint32 *lookuparray = grouplookupVLA.data();
    qint32 *groupcodearray = groupcodesVLA.data();

    if (count == 0) //species no longer present - get out now!
    {
        return -1;
    }

    if (count == 1) //single genome species - set simple group and skip pairwise comparisons
    {
        groupcodearray[0] = 0; //set single group
    }
    else
    {
        int indexForFirst = 0;  //Index (for groupcodes) for first (base) genome in pairwise comp.
        for (int i = firstrealbin; i <= lastrealbin; i++) //loop over all genomes for base
        {
            for (int j = 0; j < thisSpecies->binData.at(i).genomeEntries; j++)
            {
                //get some details
                SpeciesBinEntry *entry = thisSpecies->entryByIndex(i, j);
                quint32 *firstGenome = entry->genome;

                qint32 firstgroupcode;
                if (groupcodearray[indexForFirst] == -1)
                    firstgroupcode = nextGroupCode++;
                else
                    firstgroupcode = groupcodearray[indexForFirst];

                //recurse up parents of groups to find correct code
                while (lookuparray[firstgroupcode] != firstgroupcode)
                    firstgroupcode = lookuparray[firstgroupcode];

                //and reset it into the data array
                groupcodearray[indexForFirst] = firstgroupcode;

                //how far ahead to look? No point checking bins over maxDiff away - CAN'T
                //be compatible
                int maxBinForSecond = i + md;

                bool allInFlag = false;
                if (maxBinForSecond >= lastrealbin)
                {
                    maxBinForSecond = lastrealbin;
                    allInFlag = true;
                    //will be set to false if we find an entry that didn't get grouped with me
                    //allows skipping of some checks - we can stop if we check to the end and
                    //find that everything can be grouped with me. No further splits possible
                }

                int indexForSecond = indexForFirst + 1; //array index for second genome

                //complex loop structure to iterate over all subsequent genomes efficiently
                //directly indexing into the raw data in the genomehashtable
                for (int sbin = i; sbin <= maxBinForSecond; sbin++)
                {
                    if (thisSpecies->binData.at(sbin).genomeEntries == 0) continue;

                    int minx = 0;  //minimum x for this bin
                    if (sbin == i)
                    {
                        minx = j + 1;
                        //catch case where minx is actually past end (j was last entry)
                        //should go to next bin - so just contine to go to next iteration
                        if (minx >= thisSpecies->binData.at(sbin).genomeEntries) continue;
                    }

                    int thisx = minx; //this x

                    int l = thisSpecies->binData.at(sbin).genomeEntries;
                    int maxchunk = (l - 1) / thisSpecies->entries_per_block; //because max number is l-1

                    //get base chunk address and offset
                    for (int chunk = minx / thisSpecies->entries_per_block; chunk <= maxchunk; chunk++)
                    {
                        int startminx = thisx % thisSpecies->entries_per_block;
                        //if (chunk>=thisSpecies->binData.at(sbin).binChunks.length())
                        //{
                        //    qDebug()<<chunk<<thisSpecies->binData.at(sbin).binChunks.length()<<sbin<<"minx, thisx"<<minx<<thisx<<"startminx, ent/block"<<startminx<<thisSpecies->entries_per_block<<"i,j"<<i<<j<<thisSpecies->binData.at(sbin).genomeEntries;
                        //}
                        quint32 *datapointer = thisSpecies->binData.at(sbin).binChunks.at(chunk);
                        datapointer += startminx * stride;
                        int last = thisSpecies->entries_per_block - 1;
                        if (chunk == maxchunk)
                            last = (l - 1) % thisSpecies->entries_per_block;
                        for (int sx = startminx; sx <= last; sx++) //inner loop - loop over all entries in this
                            //chunk. This is the speed critical bit
                        {
                            qint32 gcs = groupcodearray[indexForSecond];   //get group code from array
                            if (gcs != -1) //already has a group
                            {
                                //recurse up arrays to find parent group
                                //typically goes round this while only once (determined experimentally)
                                while (lookuparray[gcs] != gcs) gcs = lookuparray[gcs];
                                lookuparray[groupcodearray[indexForSecond]] = gcs; //shortcut lookups for next time!

                                if (gcs == firstgroupcode) //it's in my group
                                {
                                    //gets here ~95% of the time
                                    //increment pointers
                                    thisx++;
                                    datapointer += stride;
                                    indexForSecond++;
                                    continue;  //Already in same group - so no work to do, onto next iteration
                                }
                                else
                                    allInFlag = false; //was in a different group that is NOT mine - can't get out after this base
                            }

                            //do species compatibility comparison with speciesID system
                            if (sis->isCompatible(firstGenome, datapointer, md))
                            {
                                //Pair IS within tolerances - so second should be in the same group as first
                                //if second not in a group - place it in group of first
                                if (gcs == -1)
                                    groupcodearray[indexForSecond] = firstgroupcode;
                                else
                                    //It was in a group - but not same group as first or
                                    //would have been caught by first line of loop
                                    //so merge this group into group of first
                                    lookuparray[gcs] = firstgroupcode;
                            }
                            else
                                //not mergable - leave it along - but can't stop after this base
                                allInFlag = false;

                            //increment pointers and round again
                            thisx++;
                            datapointer += stride;
                            indexForSecond++;
                        }
                    }
                }
                if (allInFlag) goto out;  //if everything ahead of the base grouped with it - we can stop!
                //otherwise...
                //increment index - and round again to next base
                indexForFirst++;
            }

        }
out:  ; //Finished loop (multi-level break needed, hence goto)
    }


    //Finalise groups and write back into the SpeciesBinEntry structures
    int maxcode = -1;
    int ecount = 0;
    for (int i = firstrealbin; i <= lastrealbin; i++)
    {
        for (int j = 0; j < thisSpecies->binData.at(i).genomeEntries; j++)
        {
            SpeciesBinEntry *entry = thisSpecies->entryByIndex(i, j);
            qint32 gci = groupcodearray[ecount];
            while (lookuparray[gci] != gci) gci = lookuparray[gci];
            entry->group = gci;
            if (gci > maxcode) maxcode = gci;
            ecount++;
        }
    }

    //sanity check
    if (maxcode < 0)
    {
        qDebug() << "Oops" << maxcode << ecount;
        exit(0);
    }

    return maxcode;
}


//The heuristic version
//This doesn't do all pairwise comparison, but selects _comparisons_ genomes to compare for each
//genome
//It then runs a second stage where it looks at all isolates - common to around 10% ungrouped -
//and does pairwise comparisons for all those, with code similar to exhaustive
//Sound faster? Well .. it isn't! About same speed. And... not debugged
int Analyser::generateGroupsForHeuristic(GenomeHashTable *thisSpecies, int comparisons)
{
    QVarLengthArray<qint32> grouplookupVLA;  //lookups are translation of groups into parent group codes
    QVarLengthArray<qint32> groupcodesVLA;   //group codes themselves - in a VLA so can be laid out sequentially
    grouplookupVLA.reserve(5000);  //guesses at good starting size for arrays
    groupcodesVLA.reserve(20000);

    QVarLengthArray<int> countsPerBinVLA;
    QVarLengthArray<int> cumulativeCountsPerBinVLA;

    cumulativeCountsPerBinVLA.resize(thisSpecies->bins);
    countsPerBinVLA.resize(thisSpecies->bins);
    int *countsPerBin = countsPerBinVLA.data();
    int *cumulativeCountsPerBin = cumulativeCountsPerBinVLA.data();

    int count = 0;
    int nextGroupCode = 0;
    int firstrealbin = -1;
    int lastrealbin = 100000;
    int md = simulationManager->simulationSettings->maxDifference;   //for speed
    SpeciesIDSystem *sis = simulationManager->speciesIDSytem;
    //initialise the VLAs, and find first and last real bins actually used
    int cumulative = 0;
    for (int i = 0; i < thisSpecies->bins; i++)
    {
        countsPerBin[i] = thisSpecies->binData.at(i).genomeEntries;
        cumulativeCountsPerBin[i] = cumulative;
        //qDebug()<<i<<countsPerBin[i]<<cumulativeCountsPerBin[i]<<thisSpecies->binData.at(i).genomeEntries;
        cumulative += countsPerBin[i];
        if (countsPerBin[i] > 0)
        {
            if (firstrealbin < 0) firstrealbin = i;
            lastrealbin = i;
        }

        for (int j = 0; j < thisSpecies->binData.at(i).genomeEntries; j++)
        {
            grouplookupVLA.append(count++);
            groupcodesVLA.append(-1);

        }
    }

    qDebug() << "In heuristic for species of size " << count;

    //lookups can no longer grow - so just get their pointers so use further down
    qint32 *lookuparray = grouplookupVLA.data();
    qint32 *groupcodearray = groupcodesVLA.data();

    if (count == 0) //species no longer present - get out now!
    {
        return -1;
    }

    if (count == 1) //single genome species - set simple group and skip pairwise comparisons
    {
        groupcodearray[0] = 0; //set single group
    }
    else
    {
        int ccount = 0;
        QElapsedTimer t;
        t.start();
        int indexForFirst = 0;  //Index (for groupcodes) for first (base) genome in pairwise comp.
        for (int i = firstrealbin; i <= lastrealbin; i++) //loop over all genomes for base
        {
            int totalInTheseBins = 0;
            int actualMinBinForRandom = -1;
            int actualMaxBinForRandom = 0;

            //work out bins to select from randomly
            for (int j = i - simulationManager->simulationSettings->maxDifference; j <= i + simulationManager->simulationSettings->maxDifference; j++)
            {
                //qDebug()<<"Checking bin "<<j;
                if (j >= firstrealbin && i <= lastrealbin)
                {
                    totalInTheseBins += countsPerBin[j];
                    actualMaxBinForRandom = j;
                    if (actualMinBinForRandom == -1) actualMinBinForRandom = j;
                }
            }

            for (int j = 0; j < thisSpecies->binData.at(i).genomeEntries; j++)
            {
                //get some details
                SpeciesBinEntry *entry = thisSpecies->entryByIndex(i, j);
                entry->index = indexForFirst;
                quint32 *firstGenome = entry->genome;

                qint32 firstgroupcode;
                if (groupcodearray[indexForFirst] == -1)
                    firstgroupcode = nextGroupCode++;
                else
                    firstgroupcode = groupcodearray[indexForFirst];

                //qDebug()<<"TEST "<<firstgroupcode<<grouplookupVLA.length()<<indexForFirst<<groupcodearray[indexForFirst];
                //recurse up parents of groups to find correct code
                while (lookuparray[firstgroupcode] != firstgroupcode)
                    firstgroupcode = lookuparray[firstgroupcode];

                //and reset it into the data array
                groupcodearray[indexForFirst] = firstgroupcode;


                for (int k = 0; k < comparisons; k++)
                {
                    //RJG - Prevent division by zero compiler warning
                    if (totalInTheseBins == 0) continue;
                    qint32 r = (qint32) (simulationManager->simulationRandoms->rand32() % (quint32)totalInTheseBins);
                    int indexOfRandomBin;
                    for (indexOfRandomBin = actualMinBinForRandom; indexOfRandomBin < actualMaxBinForRandom; indexOfRandomBin++)
                    {
                        r -= countsPerBin[indexOfRandomBin];
                        //qDebug()<<"looking at bin "<<indexOfRandomBin<<" r is "<<r;
                        if (r < 0)
                        {
                            //qDebug()<<"selected bin "<<indexOfRandomBin<<" with count "<<countsPerBin[indexOfRandomBin];
                            break;
                        }
                    }
                    //qDebug()<<indexOfRandomBin<<countsPerBin[indexOfRandomBin];
                    //index should be correct - loop doesn't check last one, should get autoset to this if needbe
                    quint32 r2 = simulationManager->simulationRandoms->rand32() % (quint32) countsPerBin[indexOfRandomBin];

                    int indexForSecond = cumulativeCountsPerBin[indexOfRandomBin] + r2;
                    if (indexForFirst == indexForSecond) break; //avoid self

                    //qDebug()<<i<<j<<indexOfRandomBin<<r2<<indexForFirst<<indexForSecond;
                    ccount++;
                    qint32 gcs = groupcodearray[indexForSecond];   //get group code from array
                    if (gcs != -1) //already has a group
                    {
                        //recurse up arrays to find parent group
                        //typically goes round this while only once (determined experimentally)
                        while (lookuparray[gcs] != gcs) gcs = lookuparray[gcs];
                        lookuparray[groupcodearray[indexForSecond]] = gcs; //shortcut lookups for next time!

                        if (gcs == firstgroupcode) //it's in my group
                        {
                            //qDebug()<<"Was in my group";
                            //indexForFirst++;
                            continue;  //Already in same group - so no work to do, onto next iteration
                        }
                    }

                    //do species compatibility comparison with speciesID system
                    if (sis->isCompatible(firstGenome, thisSpecies->entryByIndex(indexOfRandomBin, r2)->genome, md))
                    {

                        //Pair IS within tolerances - so second should be in the same group as first
                        //if second not in a group - place it in group of first
                        if (gcs == -1)
                        {
                            groupcodearray[indexForSecond] = firstgroupcode;
                            //qDebug()<<"Compatible - not in a group - placing";
                        }
                        else
                            //It was in a group - but not same group as first or
                            //would have been caught by first line of loop
                            //so merge this group into group of first
                        {
                            lookuparray[gcs] = firstgroupcode;
                            //qDebug()<<"Compatible - group merge";
                        }
                    }
                }
                //increment index - and round again to next base
                indexForFirst++;
            }
        }
        qDebug() << "Heuristic stage 1 (random pairings) time taken: " << t.elapsed();

        //finalise groups
        int fcount = 0;
        for (int i = firstrealbin; i <= lastrealbin; i++)
        {
            for (int j = 0; j < thisSpecies->binData.at(i).genomeEntries; j++)
            {
                int gci = groupcodearray[fcount];
                while (lookuparray[gci] != gci) gci = lookuparray[gci];
                groupcodearray[fcount++] = gci;
            }
        }

        indexForFirst = 0;
        QHash<qint32, GroupData *> groups; //key is group code
        for (int i = firstrealbin; i <= lastrealbin; i++)
            for (int j = 0; j < thisSpecies->binData.at(i).genomeEntries; j++)
            {
                int thisgroupcode = groupcodearray[indexForFirst++];
                GroupData *gd = groups.value(thisgroupcode, nullptr); //get number of genomes, or 0 if no entry
                if (gd == nullptr)
                {
                    gd = new GroupData(thisgroupcode);
                    groups.insert(thisgroupcode, gd);
                }
                gd->genomeCount += 1;
                gd->allGenomes.append(thisSpecies->entryByIndex(i, j));
            }

        reportGroups(&groups);

        int maxGenomeCount = -1;
        int groupIDforMaxGenomeCount = -1;
        foreach (GroupData *g, groups)
        {
            if (g->genomeCount > maxGenomeCount)
            {
                groupIDforMaxGenomeCount = g->groupID;
                maxGenomeCount = g->genomeCount;
            }
        }


        qDebug() << "Heuristic stage 2 (grouping) time taken: " << t.elapsed() << " (groups=" << groups.count();

        //onto stage 3 - largely copied from generateGroupsFor

        int stride = thisSpecies->stride;                                //ditto - avoiding indirection in loop core


        //Index (for groupcodes) for first (base) genome in pairwise comp.

        int redoCount = 0;
        foreach (GroupData *g, groups)
        {
            if (g->groupID == groupIDforMaxGenomeCount)
            {
//                qDebug()<<"Skipping group "<<g->groupID<<" with count of "<<g->genomeCount;
                continue; //skip the big group!

            }
            for (int j = 0; j < g->genomeCount; j++)
            {
//                qDebug()<<"Doing group "<<g->groupID<<" size "<<g->genomeCount;

                redoCount++;
                //get some details
                SpeciesBinEntry *entry = g->allGenomes.at(j);
                quint32 *firstGenome = entry->genome;
                int bin = simulationManager->speciesIDSytem->bitcountAllMasked(entry->genome, 0);
                indexForFirst = entry->index;

                qint32 firstgroupcode;
                if (groupcodearray[indexForFirst] == -1)
                    firstgroupcode = nextGroupCode++;
                else
                    firstgroupcode = groupcodearray[indexForFirst];

                //recurse up parents of groups to find correct code
                while (lookuparray[firstgroupcode] != firstgroupcode)
                    firstgroupcode = lookuparray[firstgroupcode];

                //and reset it into the data array
                groupcodearray[indexForFirst] = firstgroupcode;

                //how far ahead to look? No point checking bins over maxDiff away - CAN'T
                //be compatible
                int minBinForSecond = bin - md;
                int maxBinForSecond = bin + md;

                int indexForSecond = cumulativeCountsPerBin[minBinForSecond]; //array index for second genome

                //complex loop structure to iterate over all subsequent genomes efficiently
                //directly indexing into the raw data in the genomehashtable
                for (int sbin = bin; sbin <= maxBinForSecond; sbin++)
                {
                    if (thisSpecies->binData.at(sbin).genomeEntries == 0) continue;

                    int minx = 0;  //minimum x for this bin

                    int thisx = minx; //this x

                    int l = thisSpecies->binData.at(sbin).genomeEntries;
                    int maxchunk = (l - 1) / thisSpecies->entries_per_block; //because max number is l-1

                    //get base chunk address and offset
                    for (int chunk = minx / thisSpecies->entries_per_block; chunk <= maxchunk; chunk++)
                    {
                        int startminx = thisx % thisSpecies->entries_per_block;
                        quint32 *datapointer = thisSpecies->binData.at(sbin).binChunks.at(chunk);
                        datapointer += startminx * stride;
                        int last = thisSpecies->entries_per_block - 1;
                        if (chunk == maxchunk)
                            last = (l - 1) % thisSpecies->entries_per_block;
                        for (int sx = startminx; sx <= last; sx++) //inner loop - loop over all entries in this
                            //chunk. This is the speed critical bit
                        {
                            if (indexForSecond == indexForFirst) continue; //skip self comparison
                            qint32 gcs = groupcodearray[indexForSecond];   //get group code from array
                            if (gcs != -1) //already has a group
                            {
                                //recurse up arrays to find parent group
                                //typically goes round this while only once (determined experimentally)
                                while (lookuparray[gcs] != gcs) gcs = lookuparray[gcs];
                                lookuparray[groupcodearray[indexForSecond]] = gcs; //shortcut lookups for next time!

                                if (gcs == firstgroupcode) //it's in my group
                                {
                                    //gets here ~95% of the time
                                    //increment pointers
                                    thisx++;
                                    datapointer += stride;
                                    indexForSecond++;
                                    continue;  //Already in same group - so no work to do, onto next iteration
                                }
                            }

                            //do species compatibility comparison with speciesID system
                            if (sis->isCompatible(firstGenome, datapointer, md))
                            {
                                //Pair IS within tolerances - so second should be in the same group as first
                                //if second not in a group - place it in group of first
                                if (gcs == -1)
                                    groupcodearray[indexForSecond] = firstgroupcode;
                                else
                                    //It was in a group - but not same group as first or
                                    //would have been caught by first line of loop
                                    //so merge this group into group of first
                                    lookuparray[gcs] = firstgroupcode;
                            }

                            //increment pointers and round again
                            thisx++;
                            datapointer += stride;
                            indexForSecond++;
                        }
                    }
                }
                //increment index - and round again to next base
                indexForFirst++;
            }
        }


        //Finalise groups and write back into the SpeciesBinEntry structures
        int maxcode = -1;
        int ecount = 0;
        for (int i = firstrealbin; i <= lastrealbin; i++)
        {
            for (int j = 0; j < thisSpecies->binData.at(i).genomeEntries; j++)
            {
                SpeciesBinEntry *entry = thisSpecies->entryByIndex(i, j);
                int gci = groupcodearray[ecount];
                while (lookuparray[gci] != gci) gci = lookuparray[gci];
                entry->group = gci;
                if (gci > maxcode) maxcode = gci;
                ecount++;
            }
        }

        qDeleteAll(groups);
        //sanity check
        if (maxcode < 0)
        {
            qDebug() << "Oops" << maxcode << ecount;
            exit(0);
        }

        qDebug() << "Heuristic stage 3 (mop-up) for " << redoCount << " time taken: " << t.elapsed();
        //exit(0);
        return maxcode;



    }
    return 1;
}

void Analyser::reportGroups(QHash<int, GroupData *> *groups)
{
    int one = 0;
    int under10 = 0;
    int under100 = 0;
    int total = 0;
    int biggest = -1;
    foreach (GroupData *g, *groups)
    {
        if (g->genomeCount == 1) one++;
        else
        {
            if (g->genomeCount < 10) under10++;
            else

            {
                if (g->genomeCount < 100) under100++;
            }
        }
        if (g->genomeCount > biggest) biggest = g->genomeCount;
    }
    qDebug() << "Groups: " << groups->count() << " 1=" << one << " <10=" << under10 << " <100=" << under100 << " biggest=" << biggest << " of " << total;
}


//Potentially parallel code to gather all genomes into frequency based recursive hash structure
//currently done in serial - actually faster
void Analyser::gatherGenomesParallel(int firstx, int lastx)
{
    int ycount = simulationManager->simulationSettings->gridY;
    int zcount = simulationManager->cellSettingsMaster->slotsPerSquare;

    quint64 lastid = 0;
    GenomeHashTable *thisSpecies = allTables.value(lastid, nullptr);
    for (int n = firstx; n <= lastx; n++)
        for (int m = 0; m < ycount; m++)
        {
            for (int c = 0; c < zcount; c++)
            {
                Critter *crit = &(critters[n][m][c]);
                if (crit->age > 0)   //if critter is alive
                {
                    quint64 thisSpeciesID = crit->speciesID;
                    if (lastid != thisSpeciesID) //this is a trick to avoid too many lookups of thisSpecies in the hash
                        //typically this will be same species as last time round!
                    {
                        thisSpecies = allTables.value(thisSpeciesID);
                        lastid = thisSpeciesID;
                    }
                    quint32 pos = static_cast<quint32>(n * 65536 + m * 256 + c);
                    thisSpecies->insert(crit->genomeWords, pos);    //insert into custom hash
                }
            }
        }
}
