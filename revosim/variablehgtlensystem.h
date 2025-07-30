#ifndef VARIABLEHGTLENSYSTEM_H
#define VARIABLEHGTLENSYSTEM_H


#include "system.h"
#include <math.h>
#include <QVector>

class VariableHgtLenSystem : public System
{
public:
    VariableHgtLenSystem();
    bool setGenomeWordsFromString(QString s, int maxsize);
    quint32 transferLength (const quint32 *genome);
    quint32 returnBitcount(const quint32 *genome);
    int transformableGenomeSize;
    quint32 returnCumulativeDistributionAtN(int n);


private:
    void createCumulativeLinearDistribution();
    QVector <quint32> cumulativeDistribution;  
};

#endif // VARIABLEHGTSYSTEM_H
