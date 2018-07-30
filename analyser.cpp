/**
 * @file
 * Analyser
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

#include "analyser.h"
#include "mainwindow.h"
#include "simmanager.h"
#include "version.h"

#include <QDebug>
#include <QHash>
#include <QHashIterator>
#include <QMessageBox>
#include <QSet>
#include <QTextStream>
#include <QTime>

/*!
 * \brief Species:sSpecies
 *
 * This is the species class, which is used in the species identifier system
 */
Species::Species()
{
    type = 0;
    ID = 0; //default, not assigned
    internalID = -1;
    parent = 0;
    size = -1;
    originTime = -1;
    logSpeciesStructure = (LogSpecies *)nullptr;
}

/*!
 * \brief Analyser::Analyser
 */
Analyser::Analyser()
{
    genomesTotalCount = 0;
}

/*!
 * \brief Analyser::addGenomeFast
 * \param genome
 */
void Analyser::addGenomeFast(quint64 genome)
{
    //adds genome to sorted list. Use halving algorithm to find insertion point

    if (genomeList.count() == 0) {
        genomeList.append(genome);
        genomeCount.append(1);
        genomesTotalCount++;
        return;
    }

    int minp = 0;
    int maxp = genomeList.count() - 1;
    int lookingat = (maxp + minp) / 2; //start in middle

    int counter = 0;
    while (counter++ < 100) {
        if (genomeList[lookingat] == genome) {
            genomeCount[lookingat]++;
            genomesTotalCount++;
            return;
        } //found it

        //found where it should be?

        if (genomeList[lookingat] < genome) {
            //we are lower in list than we should be - genome we are looking at
            //is lower than us
            int next = lookingat + 1;
            if (next == genomeCount.count()) { //at end, just append then
                genomeList.append(genome);
                genomeCount.append(1);
                genomesTotalCount++;
                goto done;
            }

            if (genomeList[next] > genome) {
                //insert here
                genomeList.insert(next, genome);
                genomeCount.insert(next, 1);
                genomesTotalCount++;
                goto done;
            } else {
                minp = lookingat;
                int oldlookingat = lookingat;
                lookingat = (maxp + minp) / 2;
                if (lookingat == oldlookingat) lookingat++;
            }
        } else {
            //we are higher in list than we should be - genome we are looking at
            //is higher than us


            int next = lookingat - 1;
            if (next == -1) { //at start, just prepend then
                genomeList.prepend(genome);
                genomeCount.prepend(1);
                genomesTotalCount++;
                goto done;
            }

            if (genomeList[next] < genome) {
                //insert here
                genomeList.insert(lookingat, genome);
                genomesTotalCount++;
                genomeCount.insert(lookingat, 1);
                goto done;
            } else {
                maxp = lookingat;
                lookingat -= (maxp + minp) / 2;
                int oldlookingat = lookingat;
                lookingat = (maxp + minp) / 2;
                if (lookingat == oldlookingat) lookingat--;
            }
        }
    };

    //shouldn't get here - old emergency test!
    if (counter >= 99) qDebug() << "Fell off end, counter is " << counter;
done:
    return;
}

/*!
 * \brief Analyser::groupsGenealogicalTracker
 *
 * This is new code that uses the genealogical tracking of species since last analysis.
 *
 * Algorithm is:
 *
 * 1. Go through all critters, making sets of genomes for each species, and recording
 * positions of all occurrences of each genome (for writing back changes).
 *
 * 2. For each species:
 * 2a - Loop through all pairwise comparisons of genomes, looping from 0 to n-2 for
 * 'first', first+1 to n-1 for second. Each time round loop, if first and second are
 * close enough to group, place second into group of first, move on...; if second was
 * already in a group, update ALL of that group to group of first, and move on...
 * 2b - If only one group - do nothing except copy new species with new size if multiple
 * groups - create new species from those with fewer unique genomes.
 * 2c - Go through and write back new species IDs into cells.
 */
void Analyser::groupsGenealogicalTracker()
{
    QTime t;
    t.start(); //for debug/user warning timing purposes

    QHash<quint64, QSet<quint64> *>
    genomedata; //key is speciesID, set is all unique genomes within that species

    //Horrible container structure to store all locations of particular genomes, for rapid write-back of new species
    //first key is speciesID
    //second key is gemome
    //qlist is of quint32s which are packed x,y,z as x*65536+y*256+z
    QHash<quint64, QHash<quint64, QList<quint32> *> *> slotswithgenome;

    QHash<quint64, qint32>
    speciessizes; //number of occurrences of particular species - key is speciesID

    //Loop over all critters and gather data (this is 1. above)
    for (int n = 0; n < gridX; n++)
        for (int m = 0; m < gridY; m++) {
            if (totalFittness[n][m] == 0) continue; //nothing alive in the cell - skip
            for (int c = 0; c < slotsPerSquare; c++) {
                if (critters[n][m][c].age > 0) { //if critter is alive
                    QHash<quint64, QList<quint32>*>
                    *genomeposlist; //will be pointer to the position list by genome for this species
                    QSet<quint64> *speciesset; //will be pointer to the genome set for this species
                    speciesset = genomedata.value(critters[n][m][c].speciesID,
                                                  (QSet<quint64> *)nullptr); //get the latter from hash table if it's there
                    if (!speciesset) { //it wasn't there - so first time we've seen this species this iteration
                        speciesset = new QSet<quint64>; //new set for the genomes for the species
                        genomedata.insert(critters[n][m][c].speciesID, speciesset); //add it to the hash

                        genomeposlist = new
                        QHash<quint64, QList<quint32>*>; //new genome/position list hash table for the species
                        slotswithgenome.insert(critters[n][m][c].speciesID, genomeposlist); //add this to its hash as well
                    } else { //species already encountered - objects exist
                        genomeposlist = slotswithgenome.value(
                                            critters[n][m][c].speciesID); //retrieve postion list/genome hash pointer for this species
                        //speciesset already retrieved
                    }

                    QList<quint32> *poslist; //this will be used or particular position list for this genome
                    int before =
                        speciesset->count(); //to check - will insert actually add genome? If not it's a duplicate
                    speciesset->insert(critters[n][m][c].genome); //add genome to the set
                    if (before != speciesset->count()) { // count changed, so genome is novel for the species
                        //needs a new position list object
                        poslist = new QList<quint32>; //create
                        genomeposlist->insert(critters[n][m][c].genome, poslist); //and add to the hash for the species
                    } else { //genome not novel, so list already exists - retrieve it from the hash list
                        poslist = genomeposlist->value(critters[n][m][c].genome);
                    }
                    poslist->append((quint32)(n * 65536 + m * 256 + c)); //package up x,y,z and add them to the list

                    //add 1 to count of occurrences for this speciesID - by end it will be correct - pre-splitting
                    //later if species are split off, their counts will be removed from this
                    speciessizes[critters[n][m][c].speciesID] = speciessizes.value(critters[n][m][c].speciesID, 0) + 1;
                }
            }
        }
    //Done - all data retrieved and ready to process

    //MDS - Next. Go through each species and do all the pairwise comparisons. This is 2 above.
    //RJG - this is the really time consuming bit of the process especially when many species. Add progress bar if it's slow.

    QHashIterator<quint64, QSet<quint64> *> ii(
        genomedata); //iterator to loop over all species and their genome sets

    QList<Species> newspecieslist; // will eventually replace the global oldSpeciesList
    // convenient to build into  a new list, then copy at the end

    //RJG - Add a progress bar
    QProgressBar prBar;
    //Work out limits
    int count = 0;
    if (simulationManager->warningCount > 0) {
        while (ii.hasNext()) {
            count++;
            ii.next();
        }
        prBar.setRange(0, count);
        prBar.setAlignment(Qt::AlignCenter);
        mainWindow->statusProgressBar(&prBar, true);
        count = 0;
        ii.toFront();
    }

    while (ii.hasNext()) { //for each entry in genomedata hash
        ii.next();

        if (simulationManager->warningCount > 0) {
            count++;
            prBar.setValue(count);
            mainWindow->processAppEvents();
        }

        QSet<quint64> *speciesset = ii.value(); // Get the set of genomes
        quint64 speciesID = ii.key(); //get the speciesID
        LogSpecies *thislogspecies;

        if (speciesMode >= SPECIES_MODE_PHYLOGENY) {
            thislogspecies = logSpeciesByID.value(speciesID, (LogSpecies *)nullptr);
            if (!thislogspecies) {
                QMessageBox::warning(mainWindow, "Oops",
                                     "Internal error - species not found in log hash. Please email MDS / RJG with this message");
                exit(0);
            }
        }

        //for speed when working, we convert the set into a static array of genomes
        //static array of group codes
        quint64 genomes[MAX_GENOME_COUNT];
        qint32 groupcodes[MAX_GENOME_COUNT];

        qint32 grouplookup[MAX_GENOME_COUNT]; //which group is this merged with? We no longer actually change group values - far too slow

        int arraymax = 0; //size used of static array
        int nextgroup = 0; //group numbers don't leave this function. Start at 0 for each species.

        if (speciesset->count() >= MAX_GENOME_COUNT) { //check it actually fits in the static array
            QMessageBox::warning(mainWindow, "Oops",
                                 "Species static array too small - you have more species than " + QString(
                                     PRODUCTNAME) + " was designed to handle. Pleasee email MDS / RJG with this message for a fix. " +
                                 QString(PRODUCTNAME) + " will now close." );
            //Hopefully this won't ever happen - if it does
            //the MAX_GENOME_COUNT can be raised of course
            exit(0);
        }

        foreach (quint64 g,
                 *speciesset) { //copy genomes into static array and set groupcodes to 'not assigned' (-1)
            genomes[arraymax] = g;
            groupcodes[arraymax] = -1; //code for not assigned
            grouplookup[arraymax] = arraymax; //not merged - just itself
            arraymax++;
        }
        //arraymax is not number of items in the static array

        //now do ALL the possible pairwise comparisons
        //THIS is the slow bit, when there are not many species - not really any faster with index group merging
        for (int first = 0; first < (arraymax - 1); first++) {
            //if this isn't in a group - put it in a new one
            if (groupcodes[first] == -1)
                groupcodes[first] = nextgroup++; //so first genome will be in group 0

            quint64 firstgenome = genomes[first]; //get genome of first for speed - many comparisons to come
            qint32 firstgroupcode = groupcodes[first];
            while (grouplookup[firstgroupcode] != firstgroupcode) firstgroupcode = grouplookup[firstgroupcode];
            groupcodes[first] = firstgroupcode;

            for (int second = first + 1; second < arraymax;
                    second++) { //for second (i.e. compare to) loop through all rest of static array
                int gcs = groupcodes[second];
                if (gcs != -1) {
                    while (grouplookup[gcs] != gcs) gcs = grouplookup[gcs];
                    grouplookup[groupcodes[second]] = gcs; //for next time!

                    if (gcs == firstgroupcode) continue;
                    //Already in same group - so no work to do, onto next iteration
                }
                //do comparison using standard (for REvoSim) xor/bitcount code. By nd, t1 is bit-distance.
                //maxDifference is set by user in the settings dialog
                quint64 g1x = firstgenome ^ genomes[second]; //XOR the two to compare
                auto g1xl = quint32(g1x & ((quint64)65536 * (quint64)65536 - (quint64)1)); //lower 32 bits
                int t1 = bitCounts[g1xl / (quint32)65536] +  bitCounts[g1xl & (quint32)65535];
                if (t1 <= maxDifference) {
                    auto g1xu = quint32(g1x / ((quint64)65536 * (quint64)65536)); //upper 32 bits
                    t1 += bitCounts[g1xu / (quint32)65536] +  bitCounts[g1xu & (quint32)65535];
                    if (t1 <= maxDifference) {
                        //Pair IS within tolerances - so second should be in the same group as first
                        //if second not in a group - place it in group of first
                        if (gcs == -1)
                            groupcodes[second] = firstgroupcode;
                        else {
                            //It was in a group - but not same group as first or
                            //would have been caught by first line of loop
                            //so merge this group into group of first
                            grouplookup[gcs] = firstgroupcode;
                        }
                    }
                }
            }
        }

        int maxcode = -1;
        for (int i = 0; i < arraymax; i++) { //fix all groups
            int gci = groupcodes[i];
            while (grouplookup[gci] != gci) gci = grouplookup[gci];
            groupcodes[i] = gci;
            if (gci > maxcode) maxcode = gci;
        }

        //if (groupcodes[i]==groupcodetomerge) groupcodes[i]=firstgroupcode;
        //after this loop - everything should be in groups - if there is more than one we need to split the species
        //one group will be old species, others will become new species

        //first - find out how many groups we have and how many genomes each has
        QHash<qint32, qint32> groups; //key is group code, value is number of genomes in that group
        for (int i = 0; i < arraymax; i++) { //for all entries in our static arrays
            qint32 v = groups.value(groupcodes[i], 0); //get number of genomes, or 0 if no entry
            groups[groupcodes[i]] = ++v; //add 1 and put it into hash table under correct groupcode
        }
        //groups hash now complete

        //Next job - go through groups hash and find the most diverse one - this will be the species that keeps the
        //old ID. This all works fine if there is only one group (most common case)
        int maxcount = -1;
        int maxcountkey = -1; //will hold group number of the most diverse group (biggest genome count)
        QHashIterator<qint32, qint32> jj(groups);
        while (jj.hasNext()) { //standard 'find biggest' loop
            jj.next();
            if (jj.value() > maxcount) {
                maxcount = jj.value();
                maxcountkey = jj.key();
            }
        }

        //We now go through these groups and sort out species data, also writing back new species IDs to
        //critters cells for any new species
        QVector<LogSpecies *>logspeciespointers;
        logspeciespointers.resize(maxcode + 1);

        jj.toFront(); //reuse same iterator for groups
        while (jj.hasNext()) {
            jj.next();
            if (jj.key() != maxcountkey) { //if this ISN'T the one we picked to keep the old ID
                qint32 groupcode = jj.key(); //get its code
                quint64 speciessize = 0; //zero its size
                quint64 samplegenome;  //will have to pick a genome for 'type' - it goes here

                for (int iii = 0; iii < arraymax; iii++) //go through static arrays -
                    //find all genome entries for this group
                    //and fix data in critters for them
                    if (groupcodes[iii] == groupcode) {
                        QList<quint32> *updatelist = slotswithgenome.value(speciesID)->value(genomes[iii]);
                        //retrieve the list of positions for this genome
                        speciessize += updatelist->count(); //add its count to size
                        foreach (quint32 v, *updatelist) { //go through list and set critters data to new species
                            int x = v / 65536;
                            int ls = v % 65536;
                            int y = ls / 256;
                            int z = ls % 256;
                            critters[x][y][z].speciesID = nextSpeciesID;

                        }

                        samplegenome = genomes[iii]; //samplegenome ends up being the last one on the list -
                        //probably actually the most efficient way to do this
                    }

                speciessizes[nextSpeciesID] = speciessize; //can set species size now in the hash
                speciessizes[speciesID] = speciessizes[speciesID] - speciessize; //remove this number from parent

                Species newsp;          //new species object
                newsp.parent = speciesID; //parent is the species we are splitting from
                newsp.originTime = itteration; //i.e. now (itteration is a global)
                newsp.ID = nextSpeciesID;   //set the ID - last use so increment
                newsp.type = samplegenome;    //put in our selected type genome

                if (speciesMode >= SPECIES_MODE_PHYLOGENY) {
                    //sort out the logspecies object
                    auto *newlogspecies = new LogSpecies;
                    auto *newdata = new LogSpeciesDataItem;
                    newdata->itteration = itteration;

                    newlogspecies->ID = nextSpeciesID;
                    newlogspecies->timeOfFirstAppearance = itteration;
                    newlogspecies->timeOfLastAppearance = itteration;
                    newlogspecies->parent = thislogspecies;
                    newlogspecies->maxSize = speciessize;
                    thislogspecies->children.append(newlogspecies);

                    newlogspecies->dataItems.append(newdata);
                    logSpeciesByID.insert(nextSpeciesID, newlogspecies);
                    newsp.logSpeciesStructure = newlogspecies;
                    logspeciespointers[groupcode] = newlogspecies;
                }

                newspecieslist.append(newsp);

                nextSpeciesID++;
            } else { //this is the continuing species
                //find it in the old list and copy
                Species newsp;
                for (int j = 0; j < oldSpeciesList.count(); j++) {
                    if (oldSpeciesList[j].ID == speciesID) {
                        newsp = oldSpeciesList[j];
                        if (speciesMode >= SPECIES_MODE_PHYLOGENY) {
                            logspeciespointers[jj.key()] = newsp.logSpeciesStructure;
                            newsp.logSpeciesStructure->timeOfLastAppearance = itteration;
                            auto *newdata = new LogSpeciesDataItem;
                            newdata->itteration = itteration;
                            newsp.logSpeciesStructure->dataItems.append(newdata);
                        }
                    }
                }
                //go through and find first occurrence of this group in static arrays
                //pick the genome as our sample
                for (int iii = 0; iii < arraymax; iii++)
                    if (groupcodes[iii] == maxcountkey) {
                        newsp.type = genomes[iii]; // a sample genome
                        break;
                    }

                //and put copied species (with new type) into the new species list
                newspecieslist.append(newsp);
            }
        }

        if (speciesMode == SPECIES_MODE_PHYLOGENY_AND_METRICS) {
            //record a load of stuff
            jj.toFront(); //reuse same iterator for groups
            while (jj.hasNext()) {
                jj.next();
                qint32 groupcode = jj.key(); //get its code
                LogSpecies *thislogspecies = logspeciespointers[groupcode];
                LogSpeciesDataItem *thisdataitem = thislogspecies->dataItems.last();

                quint64 speciessize = 0; //zero its size

                quint64 samplegenome;
                QSet<quint16> cellsoc;
                thisdataitem->genomicDiversity = 0;
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

                for (int iii = 0; iii < arraymax; iii++) //go through static arrays -
                    //find all genome entries for this group
                    //and fix data in critters for them
                {
                    if (groupcodes[iii] == groupcode) {
                        thisdataitem->genomicDiversity++;
                        QList<quint32> *updatelist = slotswithgenome.value(speciesID)->value(genomes[iii]);
                        //retrieve the list of positions for this genome
                        speciessize += updatelist->count(); //add its count to size

                        foreach (quint32 v, *updatelist) { //go through list and set critters data to new species
                            int x = v / 65536;
                            int ls = v % 65536;
                            int y = ls / 256;
                            int z = ls % 256;

                            sumxpos += (quint64)x;
                            sumypos += (quint64)y;
                            if (x < minx) minx = (int)x;
                            if (y < miny) miny = (int)y;
                            if (x > maxx) maxx = (int)x;
                            if (y < maxy) maxy = (int)y;

                            sumfit += (quint64)(critters[x][y][z].fitness);
                            cellsoc.insert((quint16)x * (quint16)256 + (quint16)y);

                            quint8 r = environment[x][y][0];
                            quint8 g = environment[x][y][1];
                            quint8 b = environment[x][y][2];

                            if (r < mincol[0]) mincol[0] = r;
                            if (g < mincol[1]) mincol[1] = g;
                            if (b < mincol[2]) mincol[2] = b;

                            if (r > maxcol[0]) maxcol[0] = r;
                            if (g > maxcol[1]) maxcol[1] = g;
                            if (b > maxcol[2]) maxcol[2] = b;

                            sumcol[0] += (quint64)r;
                            sumcol[1] += (quint64)g;
                            sumcol[2] += (quint64)b;

                            //TODO - allow for toroidal!
                            //to do - put correct data in this item

                            //newdata->centroidRangeX=n; // mean of all x's - but allow for toroidal somehow
                            //newdata->centroidRangeY=m;
                            //newdata->geographicalRange=0;  //max (max-min x, max-min y), but allow for toroidal
                        }
                        samplegenome = genomes[iii];
                    }
                }
                thisdataitem->meanFitness = (quint16)((sumfit * 1000) / speciessize);
                thisdataitem->sampleGenome = samplegenome;
                thisdataitem->size = speciessize;
                thisdataitem->cellsOccupied = cellsoc.count();
                thisdataitem->maxEnvironment[0] = maxcol[0];
                thisdataitem->maxEnvironment[1] = maxcol[1];
                thisdataitem->maxEnvironment[2] = maxcol[2];
                thisdataitem->minEnvironment[0] = mincol[0];
                thisdataitem->minEnvironment[1] = mincol[1];
                thisdataitem->minEnvironment[2] = mincol[2];
                thisdataitem->meanEnvironment[0] = (quint8)(sumcol[0] / speciessize);
                thisdataitem->meanEnvironment[1] = (quint8)(sumcol[1] / speciessize);
                thisdataitem->meanEnvironment[2] = (quint8)(sumcol[2] / speciessize);
                thisdataitem->centroidRangeX = (quint8)(sumxpos / speciessize);
                thisdataitem->centroidRangeY = (quint8)(sumypos / speciessize);
                thisdataitem->geographicalRange = (quint8)(qMax(maxx - minx, maxy - miny));
            }
        }

    }

    if (simulationManager->warningCount > 0) mainWindow->statusProgressBar(&prBar, false);

    //Nearly there! Just need to put size data into correct species
    for (int f = 0; f < newspecieslist.count(); f++) { //go through new species list
        auto newsize = (quint32)speciessizes[newspecieslist[f].ID];
        newspecieslist[f].size = newsize;
        //find size in my hash, put it in

        if (speciesMode >= SPECIES_MODE_PHYLOGENY) {
            LogSpecies *ls = newspecieslist[f].logSpeciesStructure;
            if (newsize > ls->maxSize) ls->maxSize = newsize;
        }
        //also in maxSize for species fluff culling
    }


    oldSpeciesList = newspecieslist; //copy new list over old one

    //delete all data - not simple for the slotswithgenome hash of hashes, but this works!
    qDeleteAll(genomedata);
    QHashIterator<quint64, QHash<quint64, QList<quint32>* > *> iter(slotswithgenome);
    while (iter.hasNext()) {
        iter.next();
        qDeleteAll(iter.value()->begin(), iter.value()->end());
        delete (iter.value());
    }
    //Done!

    //RJG - need to give user heads up if species ID is taking > 5 seconds, and allow them to turn it off.
    if (t.elapsed() > 5000)simulationManager->warningCount++;
}


//
/*!
 * \brief Analyser::groupsWithHistoryModal
 *
 * This was last functional version pre-2017. Now superceded by groupsGenealogicalTracker().
 *
 * Implementation of new search mechanism based around using modal genome as core of species.
 *
 * 1. Take ordered genomes from addgroups_fast
 * 2. Set up an array for each which has a species number - default is 0 (not assigned).
 * Set N (next species number) to 1
 * 3. Find largest count not yet assigned to a species (perhaps do by sorting, but probably fine
 * to just go through and find biggest). If everything is assigned, go to DONE
 * 4. Compare this to ALL other genomes, whether assigned to a species or not. If they are close
 * enough, mark them down as species N (will include self), or if they are already assigned to a
 * species, note somewhere that those two species are equivalent
 * 5. go through and fix up equivalent species
 * 6. Go to 3
 *
 * Species still merge a little too easily.
 * How about - only implement a species merge if we find a decent number of connections
 * Now does this, but looks at not absolute size but connection size wrt overall size of the
 * to-be-merged species
 * Setting in dialog for sensitivity is percentage ratio between link size and my size. Normally around
 * 100 - so small will tend to link to big, but
 * big won't link to small. Seems to work pretty well.
 *
 * Next modification - matching up with species from different time-slices
 *
 * Then - do comparison with last time - not yet implemented
 *
 * \see Analyser::groupsGenealogicalTracker()
 */
void Analyser::groupsWithHistoryModal()
{
    //QTime t;
    //t.start();

    //We start with two QLlists
    // genomeList - list of quint64 genomes
    // genomeCount - list of ints, the number of occurences of each
    // genomesTotalCount is the sum of values in genomeCount

    //2. Set up and  blank the speciesID array
    int genome_list_count = genomeList.count();
    QList<int> species_sizes;
    QList<int> species_type; //type genome in genomeList, genomeCount

    species_sizes.append(0); //want an item 0 so we can start count at 1#
    species_type.append(0);

    //Calculate mergethreshold - how many cross links to merge a species?
    speciesID.clear();
    for (int i = 0; i < genome_list_count; i++) speciesID.append(0);

    //This is N above
    int next_id = 1;

    //List to hold the species translations
    QHash<int, int> merge_species; //second int is count of finds

    //3. Find largest count
    do {
        int largest = -1;
        int largest_index = -1;
        for (int i = 0; i < genome_list_count; i++) {
            if (speciesID[i] == 0)
                if (genomeCount[i] > largest) {
                    largest = genomeCount[i];
                    largest_index = i;
                }
        }

        if (largest == -1) break; // if all assigned - break out of do-while

        //4. Compare this to ALL other genomes, whether assigned to a species or not.
        //If they are close enough, mark them down as species N (will include self),
        //or if they are already assigned to a species, note somewhere that those two species are equivalent
        int this_species_size = 0;
        quint64 mygenome = genomeList[largest_index];
        for (int i = 0; i < genome_list_count; i++) {
            quint64 g1x = mygenome ^ genomeList[i]; //XOR the two to compare
            auto g1xl = quint32(g1x & ((quint64)65536 * (quint64)65536 - (quint64)1)); //lower 32 bits
            int t1 = bitCounts[g1xl / (quint32)65536] +  bitCounts[g1xl & (quint32)65535];
            if (t1 <= maxDifference) {
                auto g1xu = quint32(g1x / ((quint64)65536 * (quint64)65536)); //upper 32 bits
                t1 += bitCounts[g1xu / (quint32)65536] +  bitCounts[g1xu & (quint32)65535];
                if (t1 <= maxDifference) {
                    if (speciesID[i] >
                            0) { //already in a species, mark to merge- summing the number of genome occurences creating the link
                        int key = speciesID[i];
                        int sum = genomeCount[i];
                        if (merge_species.contains(key)) sum += merge_species[key];
                        merge_species.insert(speciesID[i], sum);
                    } else {
                        this_species_size += genomeCount[i]; // keep track of occurences
                        speciesID[i] = next_id;
                    }
                }
            }
        }

        //5. go through and fix up equivalent species
        //merge any species that need merging
        //iterate over set, convert all examples to this species
        //Also check for the correct type specimen for this species - the one with highest count basically
        QHashIterator<int, int> j(merge_species);
        int highestcount = largest;
        int highestcountindex = largest_index;
        while (j.hasNext()) {
            j.next();
            int tomerge = j.key();
            int usesize = qMin(this_species_size,
                               species_sizes[tomerge]); //use ratio of links to SMALLEST of the two populations
            int senscalc = ((j.value()) * 100) / usesize;
            if (senscalc >= speciesSensitivity) {

                if (genomeCount[species_type[tomerge]] > highestcount) {
                    highestcount = genomeCount[species_type[tomerge]];
                    highestcountindex = species_type[tomerge];
                }

                this_species_size += species_sizes[tomerge];
                species_sizes[tomerge] = 0; //merged species size to 0
                for (int i = 0; i < genome_list_count; i++)
                    if (speciesID[i] == tomerge) speciesID[i] = next_id;
            }
        }

        merge_species.clear();
        species_type.append(highestcountindex);
        species_sizes.append(this_species_size); //store size
        next_id++;
    } while (true);


    //OK, now to match up with last time

    //Actual results from all this are
    //  species_type - array of indexes to type specimens
    //  species_sizes - total size for each species (if 0, not a species)
    //  Want to turn this into a Qlist of species structures - each an ID and a modal genome

    QList<Species> newspecieslist;

    for (int i = 1; i < species_sizes.count(); i++) {
        if (species_sizes[i] > 0) {
            Species s;
            s.type = genomeList[species_type[i]];
            s.internalID = i;
            s.size = species_sizes[i];
            newspecieslist.append(s);
        }
    }


    //That's created the new list. Now need to go through and match up with the old one. Use code from before... reproduced
    //below

    //set up parent/child arrays - indices will be group keys
    QHash <int, int>
    parents; //link new to old. Key is specieslistnew indices, value is specieslistold index.
    QHash <int, int>
    childdists; //just record distance to new - if find better, replace it. Key is specieslistold indices
    QHash <int, int> childcounts; //Number of children
    QHash <int, int> primarychild; //link old to new
    QHash <int, int>
    primarychildsizediff; //used for new tiebreaking code. Key is specieslistold indices, value is size difference to new

    QList<Species> oldspecieslist_combined = oldSpeciesList;

    //Add in all past lists to oldSpeciesList  - might be slow, but simplest solution. Need to do an add that avoids duplicates though
    QSet<quint64> IDs;

    //put all IDs in the set from oldSpeciesList
    for (int i = 0; i < oldSpeciesList.count(); i++) IDs.insert(oldSpeciesList[i].ID);

    //now append all previous list items that are not already in list with a more recent ID!
    for (int l = 0; l < (timeSliceConnect - 1) && l < archivedSpeciesLists.count(); l++)
        for (int m = 0; m < archivedSpeciesLists[l].count(); m++) {
            if (!(IDs.contains(archivedSpeciesLists[l][m].ID))) {
                IDs.insert(archivedSpeciesLists[l][m].ID);
                oldspecieslist_combined.append(archivedSpeciesLists[l][m]);
            }
        }


    if (oldSpeciesList.count() > 0) {

        for (int i = 0; i < newspecieslist.count(); i++) {
            //for every new species

            int bestdist = 999;
            int closestold = -1;
            int bestsize = -1;

            //look at each old species, find closest
            for (int j = 0; j < oldspecieslist_combined.count(); j++) {
                quint64 g1x = oldspecieslist_combined[j].type ^ newspecieslist[i].type; //XOR the two to compare
                auto g1xl = quint32(g1x & ((quint64)65536 * (quint64)65536 - (quint64)1)); //lower 32 bits
                int t1 = bitCounts[g1xl / (quint32)65536] +  bitCounts[g1xl & (quint32)65535];
                auto g1xu = quint32(g1x / ((quint64)65536 * (quint64)65536)); //upper 32 bits
                t1 += bitCounts[g1xu / (quint32)65536] +  bitCounts[g1xu & (quint32)65535];

                if (t1 == bestdist) {
                    //found two same distance. Parent most likely to be the one with the bigger population, so tiebreak on this!
                    if (oldspecieslist_combined[j].size > bestsize) {
                        bestdist = t1;
                        closestold = j;
                        bestsize = oldspecieslist_combined[j].size;
                    }
                }
                if (t1 < bestdist) {
                    bestdist = t1;
                    closestold = j;
                    bestsize = oldspecieslist_combined[j].size;
                }
            }

            parents[i] = closestold; //record parent
            int thissizediff = qAbs(bestsize - newspecieslist[i].size);

            if (childdists.contains(closestold)) { //already has a child

                if (thissizediff <
                        primarychildsizediff[closestold]) //one closest in size is to be treated as primary

                {
                    childdists[closestold] = bestdist;
                    primarychild[closestold] = i;
                    primarychildsizediff[closestold] = thissizediff;
                }
                /*
                                //if (childdists[closestold]>bestdist) {childdists[closestold]=bestdist; primarychild[closestold]=i;}
                                if (childdists[closestold]==bestdist)
                                {
                                    //tie-break on size again - primary child should be the child closest in size to original I think

                                    if (thissizediff<primarychildsizediff[closestold])
                                      {childdists[closestold]=bestdist; primarychild[closestold]=i; primarychildsizediff[closestold]=thissizediff;}
                                }
                */
                childcounts[closestold] = childcounts[closestold] + 1;
            } else { //new

                childdists[closestold] = bestdist;
                childcounts[closestold] = 1;
                primarychild[closestold] = i;
                primarychildsizediff[closestold] = thissizediff;
            }
        }

        //now handle ID numbers. Loop over old
        for (int j = 0; j < oldspecieslist_combined.count(); j++) {
            //for every old species
            if (childcounts.contains(j)) {
                newspecieslist[primarychild[j]].ID = oldspecieslist_combined[j].ID;
                newspecieslist[primarychild[j]].parent = oldspecieslist_combined[j].parent;
                newspecieslist[primarychild[j]].originTime = oldspecieslist_combined[j].originTime;

            }
            // else //apparently went extinct, currently do nothing

        }

        //fill in blanks - new species
        for (int i = 0; i < newspecieslist.count(); i++)
            if (newspecieslist[i].ID == 0) {
                newspecieslist[i].ID = nextSpeciesID++;
                newspecieslist[i].parent = oldspecieslist_combined[parents[i]].ID;
                newspecieslist[i].originTime = itteration;
            }

    } else {
        //handle first time round - basically give species proper IDs
        for (int i = 0; i < newspecieslist.count(); i++) {
            newspecieslist[i].ID = nextSpeciesID++;
            newspecieslist[i].originTime = itteration;
        }
    }

    //straighten out parents in new array, and any other persistent info
    QHashIterator<int, int> ip(parents);

    while (ip.hasNext()) {
        ip.next();
        //link new to old. Key is specieslistnew indices, value is specieslistold index.
        if (newspecieslist[ip.key()].parent ==
                0) //not a new parent, so must be yet-to-be filled from parent - anagenetic descendent
            newspecieslist[ip.key()].parent = oldspecieslist_combined[ip.value()].parent;
        if (newspecieslist[ip.key()].originTime ==
                -1) //not a new time, so must be yet-to-be filled from parent
            newspecieslist[ip.key()].originTime = oldspecieslist_combined[ip.value()].originTime;
    }

    //finally go through newspecieslist and look at internalID - this is ID in the species_ID array
    //do same for colour array
    lookupPersistentSpeciesID.clear();

    for (int i = 0; i <= speciesID.count(); i++) lookupPersistentSpeciesID.append(0);


    for (int i = 0; i < newspecieslist.count();
            i++) lookupPersistentSpeciesID[newspecieslist[i].internalID] = newspecieslist[i].ID;


    //handle archive of old species lists (prior to last time slice)
    if (oldSpeciesList.count() > 0
            && timeSliceConnect > 1) { //if there IS an old species list, and if we are storing them
        archivedSpeciesLists.prepend(oldSpeciesList); //put the last old one in position 0
        while (archivedSpeciesLists.count() > timeSliceConnect - 1)
            archivedSpeciesLists.removeLast(); //trim list to correct size
        // will normally only remove one from end, unless timeSliceConnect has changed
        // TO DO - note species going extinct here?
    }
    oldSpeciesList = newspecieslist;
}

/*!
 * \brief Analyser::speciesIndex
 * \param genome
 * \return lookup_persistant_species_id or -1
 */
int Analyser::speciesIndex(quint64 genome)
//returns index (in genomeCount, genomeList, speciesID) for genome
{
    QList<quint64>::iterator i = qBinaryFind(genomeList.begin(), genomeList.end(), genome);

    if (i == genomeList.end()) return -1;

    return lookupPersistentSpeciesID[speciesID[i - genomeList.begin()]]; // this is QT bodgy way to get index apparently
}
