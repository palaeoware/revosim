#ifndef SIMSETTINGS_H
#define SIMSETTINGS_H

#include <QStringList>
#include <globals.h>

class SimSettings
{
public:
    SimSettings();

    int gridX;        //Can't be used to define arrays
    int gridY;
    int environmentChangeRate;
    int maxDifference; //Maximum difference to breed, key for species definitions
    int maxDifferenceHgt; //Maximum difference to preform HGT
    int speciesSamples; //no longer used - keep for backwards compat of files
    int speciesSensitivity; //no longer used - keep for backwards compat of files
    int timeSliceConnect; //no longer used - keep for backwards compat of files
    int pathogenMode;
    int trophicSeedMode;
    int genomeSize;
    int reseedMode;
    int hgtMode;

    quint8 environmentMode;
    qint16 a_priori_interaction[4][4];
    quint64 minSpeciesSize;
    quint64 lastSpeciesCalculated;
    quint32 reseedGenome[MAX_GENOME_WORDS];

    //bool reseedKnown;
    //bool reseedDual;
    bool recalculateFitness;
    bool nonspatial;
    bool toroidal;
    bool gui;
    bool speciesLogging;
    bool speciesLoggingToFile;
    bool fitnessLoggingToFile;
    bool logging;
    bool predationRestriction;
    bool environmentInterpolate;
    bool linkagesOn;
    bool randomReseedBeforeGenetic;
    bool hgtrandomlength;
    bool variableHgtProb;
    bool variableHgtLen;
    bool variableHgtId;
    bool hgtId;
};

#endif // SIMSETTINGS_H
