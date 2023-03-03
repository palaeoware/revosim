#ifndef VARIABLEMUTATESYSTEM_H
#define VARIABLEMUTATESYSTEM_H

#include "system.h"
#include <QVector>

class VariableMutateSystem : public System
{
public:
    VariableMutateSystem();
    //RJG - Redo the distribution when genomewords changes - hence new version of inherited function
    bool setGenomeWordsFromString(QString s, int maxsize);
    bool willMutate(const quint32 *genome);
    quint32 returnCumulativeDistributionAtN(int n);
    quint32 returnBitcount(const quint32 *genome);
private:
    ///RJG system uses  a cumulative standard normal distribution from -3 to 3 for breeding probabilitie
    QVector <quint32> cumulativeNormalDistribution;
    void redoCumulativeNormalDistribution();
};

#endif // VARIABLEMUTATESYSTEM_H
