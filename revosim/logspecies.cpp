/**
 * @file
 * Log Species
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

#include "logspecies.h"
#include "simmanager.h"
#include "analysistools.h"
#include "mainwindow.h"

#include <QTextStream>



/*!
 * \brief LogSpecies::LogSpecies
 */
LogSpecies::LogSpecies()
    = default;

/*!
 * \brief LogSpecies::~LogSpecies
 */
LogSpecies::~LogSpecies()
{
    qDeleteAll(dataItems);
    qDeleteAll(children);
}

/*!
 * \brief LogSpecies::writeDataLine
 *
 * Write multiple lines of data for this species
 *
 * \param start
 * \param end
 * \param speciesID
 * \param parentID
 * \return QString
 */

QString LogSpecies::writeDataLine(quint64 start, quint64 end, quint64 speciesID, quint64 parentID)
{
    QString outstring;
    QTextStream out(&outstring);

    foreach (LogSpeciesDataItem *di, dataItems)
    {
        if (di->iteration >= start && di->iteration < end)
        {
            out << speciesID << "," << parentID << ",";
            out << di->iteration << "," << di->size << ",";
            //RJG - with v3.0.0 we need to output multiple words
            QString genome;
            for (int i = 0; i < simulationManager->simulationSettings->genomeSize; i++) genome += QString("W%1_%2_").arg(i).arg(di->sampleMultiWordGenome[i]);
            genome.chop(1);
            out << genome;
            QString genomeString(System::returnGenomeString(di->sampleMultiWordGenome, simulationManager->simulationSettings->genomeSize));
            out << "," << genomeString << "," << di->sharedCSVoutput() << "\n";
        }
    }
    return outstring;
}

/*!
 * \brief LogSpecies::writeData
 *
 * Writes the data as a string
 *
 * \param childIndex
 * \param lastTimeBase
 * \param killFluff
 * \param parentID
 * \return QString
 */
QString LogSpecies::writeData(int childIndex, quint64 lastTimeBase, bool killFluff, quint64 parentID)
{
    //modelled on writeNewickString
    int cc = children.count();
    quint64 speciesID = ids++;
    if (lastTimeBase == 0) lastTimeBase = timeOfFirstAppearance;
    if (cc <= childIndex)
        return writeDataLine(lastTimeBase, timeOfLastAppearance, speciesID, parentID);

    int nextchildindex = cc; //for if it runs off the end
    quint64 thisgeneration = 0;
    bool genvalid = false;
    for (int i = childIndex; i < cc; i++)
    {
        if (!genvalid || children[i]->timeOfFirstAppearance == thisgeneration)
        {
            if (!(children[i]->isFluff()))
            {
                genvalid = true;
                thisgeneration = children[i]->timeOfFirstAppearance;
            }
        }
        else
        {
            nextchildindex = i;
            break;
        }
    }

    if (!genvalid) return writeDataLine(lastTimeBase, timeOfLastAppearance, speciesID, parentID);

    //now recurse onto (a) this with new settings, and (b) the children
    QString s;
    QTextStream out(&s);
    out << writeData(nextchildindex, thisgeneration, killFluff, speciesID); //my 'offspring'
    for (int i = childIndex; i < nextchildindex; i++)
    {

        if (!(children[i]->isFluff()))
            out << children.at(i)->writeData(0, thisgeneration, killFluff, speciesID);
    }
    out << writeDataLine(lastTimeBase, thisgeneration, speciesID, parentID);

    return s;

}

/*!
 * \brief LogSpecies::maxSizeIncludingChildren
 *
 * Finds maxSize of this AND all child lineages - for fluff removal
 *
 * \return int recurseMaxSize
 */
quint32 LogSpecies::maxSizeIncludingChildren()
{
    quint32 recurseMaxSize = maxSize;
    for (int i = 0; i < children.count(); i++)
    {
        quint32 maxSizeTemp = children[i]->maxSizeIncludingChildren();
        if (maxSizeTemp > recurseMaxSize) recurseMaxSize = maxSizeTemp;
    }
    return recurseMaxSize;
}

/*!
 * \brief LogSpecies::isFluff
 *
 * Tests if a species has become 'extinct', this will return TRUE if:
 * a) the species is only fetured in one iteration
 * b) one has no descendants and is smaller than minSpeciesSize (used by Newick string)
 * Will return FALSE otherwise
 *
 * \return bool
 */
bool LogSpecies::isFluff()
{
    // Always fluff if only in one iteration
    if (timeOfFirstAppearance == timeOfLastAppearance) return true;

    // Is this a 'fluff' species - i.e. one has no descendants and is smaller than minSpeciesSize?
    // Used by filter of writeNewickString and other recursives
    if (children.count() != 0 && !allowExcludeWithDescendants)
        return false;

    quint32 recurseMaxSize = maxSize;
    if (allowExcludeWithDescendants) recurseMaxSize = maxSizeIncludingChildren();

    return recurseMaxSize <= simulationManager->simulationSettings->minSpeciesSize;
}

/*!
 * \brief LogSpecies::writeNewickString
 *
 * childIndex is a bodge to allow recursive calls on the same item
 * moving up through list of children, grouping by creation time
 * send it a 0 initially
 * and lastTimeBase needs to be timeOfFirstAppearance initially too.
 * no, it can be 0 - picked up in functoin
 * killFluff flag - ignore any children that have a single iteration life and no children of their own
 *
 * \param childIndex
 * \param lastTimeBase
 * \param killFluff
 * \return
 */
QString LogSpecies::writeNewickString(int childIndex, quint64 lastTimeBase, bool killFluff)
{
    //recursively generate Newick-format text description of tree
    //bl is branch length. For simple nodes - just last appearance time - first
    int cc = children.count();
    quint64 bl;
    quint64 speciesID = ids++;
    if (lastTimeBase == 0) lastTimeBase = timeOfFirstAppearance;
    if (cc <= childIndex)
    {
        bl = timeOfLastAppearance - lastTimeBase;
        QString s = QString ("ID%1-%2:%3").arg(speciesID).arg(maxSize).arg(bl);
        return s;
    }
    int nextchildindex = cc; //for if it runs off the end
    quint64 thisgeneration = 0;
    bool genvalid = false;
    for (int i = childIndex; i < cc; i++)
    {
        if (!genvalid || children[i]->timeOfFirstAppearance == thisgeneration)
        {
            if (!(children[i]->isFluff()))
            {
                genvalid = true;
                thisgeneration = children[i]->timeOfFirstAppearance;
            }
        }
        else
        {
            //OK, run too far - i is now next childIndex
            nextchildindex = i;
            break;
        }
    }

    if (!genvalid)
    {
        //actually no children
        bl = timeOfLastAppearance - lastTimeBase;
        QString s = QString ("ID%1-%2:%3").arg(speciesID).arg(maxSize).arg(bl);
        return s;
    }
    bl = thisgeneration - lastTimeBase;

    //now recurse onto (a) this with new settings, and (b) the children
    QString s;
    QTextStream out(&s);
    out << "(" << writeNewickString(nextchildindex, thisgeneration, killFluff);
    for (int i = childIndex; i < nextchildindex; i++)
    {
        if (!(children[i]->isFluff()))
            out << "," << children.at(i)->writeNewickString(0, thisgeneration, killFluff);
    }
    out << ")ID" << speciesID << "-" << maxSize << ":" << bl;
    return s;

}
