#ifndef VARIABLEHGTIDSYSTEM_H
#define VARIABLEHGTIDSYSTEM_H


#include "system.h"
#include <math.h>
#include <QVector>

class VariableHgtIdSystem : public System
{
public:
    VariableHgtIdSystem();
    bool setGenomeWordsFromString(QString s, int maxsize);
    bool tryTransform(quint32* genome, quint32* maskofgenome, quint32* maskofdonor);



private:
    void createLinearDistribution();
    QVector <quint32> linearDistribution;
};

#endif // VARIABLEHGTSYSTEM_H
