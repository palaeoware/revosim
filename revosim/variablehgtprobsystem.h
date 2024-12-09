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
    bool willTransfer(const quint32 *genome);

private:

    QVector <quint64> cumulativeDistribution;
    void createCumulativeLinearDistribution();
    void createCumulativeLogLinearDistribution();
};


#endif // VARIABLEHGTPROBSYSTEM_H
