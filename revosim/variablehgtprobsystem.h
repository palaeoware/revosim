#ifndef VARIABLEHGTPROBSYSTEM_H
#define VARIABLEHGTPROBSYSTEM_H


#include "system.h"
#include <math.h>
#include <QVector>

class VariableHgtProbSystem : public System
{
public:
    VariableHgtProbSystem();
    bool setGenomeWordsFromString(QString s, int maxsize);
    bool variableWillTransform(const quint32 *genome);
    quint32 returnBitcount(const quint32 *genome);
    QVector <quint64> cumulativeDistribution;
    quint32 returnCumulativeDistributionAtN(int n);

private:
    void createCumulativeLinearDistribution();
    void createCumulativeLogLinearDistribution();
};


#endif // VARIABLEHGTPROBSYSTEM_H
