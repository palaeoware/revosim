#ifndef SPECIESBINENTRY_H
#define SPECIESBINENTRY_H

#include <QtGlobal>
#include <QVarLengthArray>
class SpeciesBinEntry
{
public:
    SpeciesBinEntry();
    quint32 *genome;
    int occurrenceCount;
    int group;
    int index; //only used in Heuristic version
    QVarLengthArray<quint32> positions; //holds coded x,y,z positions for all critters with this genome
    void *operator new(size_t size);
    void operator delete(void *p);
};

#endif // SPECIESBINENTRY_H
