#ifndef PATHOGENSSYSTEM_H
#define PATHOGENSSYSTEM_H

#include "system.h"
#include <QVector>
#include <QColor>

class PathogensSystem : public System
{
public:
    PathogensSystem();
    bool willDie(const quint32 *genome, int n, int m, int c);
    bool setGenomeWordsFromString(QString s, int maxsize);
    void replicate(int nlocal, int mlocal, int n, int m, int c);
    void mutate();
    void reset();
    QColor returnRGB(int n, int m, int word);

private:
    QVector <quint32> pathogenProbabilityDistribution;
    void redoProbabilityDistribution();
    quint32 *newpathogens[GRID_X][GRID_Y][PATH_DEPTH]; //RJG - Pathogen overlay for variable size genomes
};

#endif // PATHOGENSSYSTEM_H
