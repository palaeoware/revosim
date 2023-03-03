#ifndef GENOMEHASHTABLE_H
#define GENOMEHASHTABLE_H
#include <QtGlobal>
#include "speciesidsystem.h"
#include "speciesbinentry.h"
#include <QVector>
#include <QMutex>
#include <QMutexLocker>

//block size for allocations of genes/entry pointers
#define RAW_DATA_BLOCK_SIZE 16384

class GenomeHashTable;

class BinData
{
public:
    BinData();
    int genomeEntries;
    QVector<quint32 *> binChunks;
    QVector<SpeciesBinEntry *> binEntries;
    GenomeHashTable *subHash;
};

class GenomeHashTable
{
public:
    GenomeHashTable(int level);
    ~GenomeHashTable();
    QVector<SpeciesBinEntry *> * getAllEntries(int thebin);
    QVector<SpeciesBinEntry *> * getAllEntries();
    void * operator new(size_t size);
    void operator delete(void *p);

    int levelInHierarchy;
    SpeciesIDSystem* sis;
    //QVector<QMutex*> mutexes;
    QVector<BinData> binData;
    SpeciesBinEntry *insert(quint32 * genome, quint32 position);
    bool contains(quint32 *genome);
    int bins;
    void copyEntryRehashed(SpeciesBinEntry *entry);
    float sumFrequencies(float data[][32], int group);
    SpeciesBinEntry *entryByIndex(int thebin, int i);
    int rawCount, insertCount;

    int stride;
    int offsetToPointer;
    int genomeSize;
    int entries_per_block;

private:
    int getBin(quint32 *genome);

    void insertIntoRawData(quint32 *genome, BinData *bd, SpeciesBinEntry *binEntry);
};

#endif // GENOMEHASHTABLE_H
