#ifndef VARIABLEBREEDSYSTEM_H
#define VARIABLEBREEDSYSTEM_H

#include "system.h"
#include <math.h>
#include <QVector>

class VariableBreedSystem : public System
{
public:
    VariableBreedSystem();
    //RJG - Redo the distribution when genomewords changes - hence new version of inherited function
    bool setGenomeWordsFromString(QString s, int maxsize);
    bool returnAsexual(const quint32 *genome);
    QString printDistribution() const;

private:
    ///RJG - system uses a cumulative standard normal distribution from -3 to 3 for breeding probabilitie
    QVector <quint32> cumulativeDistribution;
    void createCumulativeNormalDistribution();
    void createCumulativeLinearDistribution();

};

#endif // VARIABLEBREEDSYSTEM_H
