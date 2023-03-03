/**
 * @file
 * Header: System
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

#ifndef SYSTEM_H
#define SYSTEM_H

#include <QtGlobal>
#include "globals.h"
#include "critter.h"
#include <QString>

class System
{
public:
    //Pass system name in constructor
    System(QString n);
    //Convert text string to genome codes stored in the UseGenomeWords array
    int returnUseGenomeWordsCount();
    //This is the array of which genome words the system applies to
    bool returnGenomeWordInUse(int n);
    //This returns a string of words in use
    QString returnGenomeWordInUseString();
    //Name of system for e.g. logging
    QString returnName();
    //Set name for e.g. multiple visualisation systems
    void setName(QString nameLocal);
    //Convert text string to genome codes stored in the UseGenomeWords array
    bool setGenomeWordsFromString(QString s, int maxsize);
    //Max number from string - this is useful for updating the GUI
    int returnMaxFromString(QString s);
    //Return bitcount of word
    static quint32 bitCount(const quint32 genome);

    //general binary to string static
    static QString returnGenomeString(const quint32 *genome, int localGenomeSize);
    //Binary to string for just words in use
    QString returnGenomeInUseString(const quint32 *genome);

    //Compare genome words
    bool isEqual(quint32 *a, quint32 *b);
    //Structures to store which words are in use
    int useGenomeWordsCount;
    int useGenomeWords[MAX_GENOME_WORDS];
protected:
    //E.g. 01 - apply to first two words, 1,3 to first and third, etc. etc.
    //How many are in use
    //System name
    QString name;
    //Report - for debugging
    QString report();
    //Work out number from char - this is sued in genome word from string, but also for working out max
    int returnNumberforChar(QChar c);
};

#endif // SYSTEM_H
