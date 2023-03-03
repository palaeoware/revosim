/**
 * @file
 * System
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

#include "system.h"
#include "simmanager.h"
#include <QDebug>
#include <bitset>


/*
 * Base class for a 'genome interaction' system
*/


System::System(QString n)
{
    name = n;
    useGenomeWordsCount = 0;
}

quint32 System::bitCount(const quint32 genome)
{
#ifdef __GNUC__
    return __builtin_popcount(genome);
#elif defined _MSC_VER
    //return bitCounts[genome / 65536] + bitCounts[genome & 65535];
    return __popcnt(genome);
#else
    return bitCounts[genome / 65536] + bitCounts[genome & 65535];
#endif

}

QString System::returnGenomeString(const quint32 *genome, int localGenomeSize)
{
    QString outString;
    QTextStream out(&outString);

    for (int i = 0; i < localGenomeSize; i++)
    {
        quint32 genomeWord = genome[i];
        for (int j = 0; j < 32; j++)
            if (tweakers[31 - j] & genomeWord) out << "1";
            else out << "0";
    }
    return outString;
}

QString System::returnGenomeInUseString(const quint32 *genome)
{
    QString outString;
    QTextStream out(&outString);
    for (int i = 0; i < useGenomeWordsCount; i++)
    {
        quint32 genomeWord = genome[useGenomeWords[i]];
        for (int j = 0; j < 32; j++)
            if (tweakers[31 - j] & genomeWord) out << "1";
            else out << "0";
    }
    return outString;
}


QString System::returnName()
{
    return name;
}

//When we have multiple systems (e.g. visualisation) good to be able to name them differently
void System::setName(QString nameLocal)
{
    name = nameLocal;
}
// Convert text string to genome codes. Returns false if not possible.
bool System::setGenomeWordsFromString(QString s, int maxsize)
{
    for (int i = 0; i < s.length(); i++)
    {
        int word = returnNumberforChar(s.at(i));

        if (word < 0 || word >= maxsize)
        {
            qInfo() << "Can't convert " << s << " to genome word list - " << s.at(i) << " is out of range";
            useGenomeWordsCount = 0;
            return false;
        }
        useGenomeWords[i] = word;
    }

    useGenomeWordsCount = s.length();
    return true;
}

// Convert char to number - included in own function so can access when setting GUI options
int System::returnNumberforChar(QChar c)
{
    int word = -1;
    if (c.isDigit())word = c.digitValue();
    else if (c.isLetter())
    {
        word = c.toUpper().toLatin1() - QChar('A').toLatin1() + 10;
    }
    return word;
}


// Convert char to number - included in own function so can access when setting GUI options
int System::returnMaxFromString(QString s)
{
    int max = -1;
    for (int i = 0; i < s.length(); i++)
    {
        int word = returnNumberforChar(s.at(i));
        if (word > max) max = word;
    }
    return max;
}



bool System::isEqual(quint32 *a, quint32 *b)
{
    for (int i = 0; i < useGenomeWordsCount; i++)
    {
        int w = useGenomeWords[i];
        if (a[w] != b[w]) return false;
    }
    return true; // got to end, so must have had max or fewer differences
}


//Debug method
QString System::report()
{
    QString t;
    t = "System name: " + name + "   System uses genes: ";
    for (int i = 0; i < useGenomeWordsCount; i++)
        t += QString::number(useGenomeWords[i]) + " ";
    return t;
}

QString System::returnGenomeWordInUseString()
{
    QString t;
    for (int i = 0; i < useGenomeWordsCount; i++)
        if (useGenomeWords[i] < 10) t += QString::number(useGenomeWords[i]);
        else t += QChar(useGenomeWords[i] + QChar('A').toLatin1() - 10);
    return t;
}

int System::returnUseGenomeWordsCount()
{
    return useGenomeWordsCount;
}

bool System::returnGenomeWordInUse(int n)
{
    for (int i = 0; i < useGenomeWordsCount; i++)if (useGenomeWords[i] == n) return true;
    return false;
}
