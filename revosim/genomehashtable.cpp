#include "genomehashtable.h"
#include "simmanager.h"
#include "speciesidsystem.h"
#include <cstring>
#include "custommemorymanager.h"

//support classes -defined in same file - only have constructors
BinData::BinData()
{
    subHash = nullptr;
    genomeEntries = 0;
}


//Implements a custom hashtable for genome rapid insertion and lookup
//Using bitcount of genome as initial hash key
//Buckets are autosorted as repeat entries are added, and repeats are counted
//This enables linear search to check most common genomes first

//Also - if bucket gets too full, it is replaced by a second layer hash table - can go as deep as 4 layers
//using bitcount of a masked version of the genome - if two genomes have the same bitcount
//they may well NOT have the same if masked with 01010101010... or 0010010010001 etc.
//This should keep memory usage under control, but avoid long linear searches

//Actual data for genomes is kept at level 0 in a custom set of big chunks, for cache reasons

//construct - give it a level in heirarchy. 0 means base level (different behaviour)
GenomeHashTable::GenomeHashTable(int level)
{
    levelInHierarchy = level;
    sis = simulationManager->speciesIDSytem;

    bins = sis->useGenomeWordsCount * simulationManager->speciesIDSytem->maskMaxBitCountPerWord[level]; //33 bins per word, as based on bitcount

    //make bindata structure
    for (int i = 0; i < bins; i++)
        binData.append(BinData());

    //convenience values
    genomeSize = simulationManager->simulationSettings->genomeSize;
    offsetToPointer = genomeSize;  //offsets will be autoscaled to 4 bytes
    stride = offsetToPointer + sizeof (void *) / sizeof(quint32); //as we store genome then pointer to entry
    entries_per_block = RAW_DATA_BLOCK_SIZE / (stride * sizeof(quint32));
    rawCount = 0;
    insertCount = 0;
}

//Destructor - does do deletion to ensure all QT data in binentries is destroyed
//Even though as much of the memory as possible is assigned with CustomMemoryManager which
//avoids the need for individual free calls
GenomeHashTable::~GenomeHashTable()
{
    for (int i = 0; i < bins; i++)
    {
        foreach (SpeciesBinEntry * e, binData.at(i).binEntries) delete e; //delete all entries
        if (binData.at(i).subHash != nullptr) delete binData.at(i).subHash; //and recurse if necessary
    }
}

float GenomeHashTable::sumFrequencies(float data[][32], int group)
{
    float total = 0;
    for (int i = 0; i < bins; i++)
    {
        if (binData.at(i).subHash != nullptr)
        {
            total += binData.at(i).subHash->sumFrequencies(data, group);
        }
        else
        {
            for (int j = 0; j < binData.at(i).binEntries.count(); j++)
            {
                SpeciesBinEntry *be = binData.at(i).binEntries.at(j);
                if (be->group == group)
                {
                    total += be->occurrenceCount;
                    for (int iiii = 0; iiii < genomeSize; iiii++)
                        for (int jjj = 0; jjj < 32; jjj++)
                        {
                            if (tweakers[31 - jjj] & be->genome[iiii]) data[iiii][jjj]
                                += static_cast<float>(be->occurrenceCount);
                        }
                }
            }
        }
    }
    return total;
}


//This is recursive, and gets the full set of genomes for one particular word
void GenomeHashTable::getSetForOneWordFromAllGenomes(QSet<quint32> *set, int group, int word)
{
    if (word<0 || word>=genomeSize)
    {
        qDebug()<<"Error in getSetForOneWordFromAllGenomes - illegal word requested";
        return;
    }
    for (int i = 0; i < bins; i++)
    {
        if (binData.at(i).subHash != nullptr)
        {
            binData.at(i).subHash->getSetForOneWordFromAllGenomes(set, group, word);
        }
        else
        {
            for (int j = 0; j < binData.at(i).binEntries.count(); j++)
            {
                SpeciesBinEntry *be = binData.at(i).binEntries.at(j);
                if (be->group == group)
                {
                    set->insert(be->genome[word]);
                }
            }
        }
    }
    return;
}

int GenomeHashTable::getBin(quint32 *genome)
{
    return sis->bitcountAllMasked(genome, levelInHierarchy);
}

//get the pointer to the SpeciesBinEntry from the custom memory structure
//which is arranged [genome words][pointer]
SpeciesBinEntry *GenomeHashTable::entryByIndex(int thebin, int i)
{
    BinData *bd = (BinData *) &binData.at(thebin);
    if (i >= bd->genomeEntries || i < 0) return nullptr;

    int chunkNumber = i / entries_per_block;
    int chunkOffset = i % entries_per_block;

    quint32 *retval = (quint32 *) (bd->binChunks.at(chunkNumber));
    retval += stride * chunkOffset;
    retval += offsetToPointer;
    return  (SpeciesBinEntry *) (*((SpeciesBinEntry **)retval));
}

//add something to raw data, getting new chunk if necessary
void GenomeHashTable::insertIntoRawData(quint32 *genome, BinData *bd, SpeciesBinEntry *binEntry)
{
    rawCount++;

    int chunkNumber = bd->genomeEntries / entries_per_block;
    int chunkOffset = bd->genomeEntries % entries_per_block;

    quint32 *chunkptr;
    if (chunkOffset == 0)
    {
        //need a new chunk
        chunkptr = (quint32 *)customMemoryManager->alloc(RAW_DATA_BLOCK_SIZE);
        bd->binChunks.append(chunkptr);
    }
    else
        chunkptr = bd->binChunks.at(chunkNumber);

    chunkptr += chunkOffset * stride;
    std::memcpy(chunkptr, genome, offsetToPointer * sizeof(quint32)); //copy actual genome
    binEntry->genome = chunkptr;
    chunkptr += offsetToPointer;
    *((SpeciesBinEntry **)chunkptr) = binEntry; //insert pointer to entry after genome

    bd->genomeEntries++;
    return;
}

//returns the entry it inserts - which is owned by whatever subhash it's in
SpeciesBinEntry *GenomeHashTable::insert(quint32 *genome, quint32 position)
{
    insertCount++;
    int bin = getBin(genome);

    BinData *bd = (BinData *) &binData.at(bin);
    GenomeHashTable *sh = bd->subHash;
    if (sh != nullptr) //has a subhash - insert there
    {
        SpeciesBinEntry *newEntry = sh->insert(genome, position);
        if (newEntry == nullptr) return newEntry;
        if (levelInHierarchy == 0) insertIntoRawData(genome, bd, newEntry); //but put in rawdata too if this is level0
        return newEntry;
    }
    else
    {
        QVector<SpeciesBinEntry *> *binPointer = (QVector<SpeciesBinEntry *> *) & (binData.at(bin).binEntries);

        int rcount = binPointer->count();
        for (int i = 0; i < rcount; i++)
        {
            if (sis->isEqual(genome, binPointer->at(i)->genome)) //check for same in speciesID system
            {
                int newcount = binPointer->at(i)->occurrenceCount += 1;
                binPointer->at(i)->positions.append(position);
                //swap this and previous entry if this one is now more frequent
                if (i > 0) //if not already first entry
                {
                    if (newcount > binPointer->at(i - 1)->occurrenceCount)
                    {
                        //swap the two entries in list
                        SpeciesBinEntry **binData = (SpeciesBinEntry **)binPointer->data();
                        SpeciesBinEntry *temp = binData[i - 1];
                        binData[i - 1] = binData[i];
                        binData[i] = temp;
                    }
                }
                return nullptr; //i.e. no new record inserted - though count increased
            }
        }
        //Run off the end and didn't find it, so insert at end (which is fine, must be just one of them)
        if (binPointer->count() > MAX_BIN_ENTRIES && levelInHierarchy < MAX_MASKS - 2)
        {
            // has got too big - split it into subhashes
            sh = bd->subHash = new GenomeHashTable(levelInHierarchy + 1);
            //Move all the entries into the subhash
            for (int i = 0; i < rcount; i++)
            {
                sh->copyEntryRehashed(binPointer->at(i));
            }
            //and add this one
            SpeciesBinEntry *newEntry = sh->insert(genome, position);
            if (levelInHierarchy == 0) insertIntoRawData(genome, bd, newEntry);
            binPointer->clear(); //remove refs to them
            return newEntry;
        }
        else
        {
            SpeciesBinEntry *newEntry = new SpeciesBinEntry();
            if (levelInHierarchy == 0) insertIntoRawData(genome, bd, newEntry);
            newEntry->positions.append(position);
            binPointer->append(newEntry);

            return newEntry;
        }
    }
}

//used to move entries down to a lower level
void GenomeHashTable::copyEntryRehashed(SpeciesBinEntry *entry)
{
    int bin = getBin(entry->genome);
    QVector<SpeciesBinEntry *> *binPointer = (QVector<SpeciesBinEntry *> *) & (binData.at(bin).binEntries);
    binPointer->append(entry);
}

//is it in there? Possibly not used
bool GenomeHashTable::contains(quint32 *genome)
{
    int bin = getBin(genome);
    GenomeHashTable *sh = binData.at(bin).subHash;
    if (sh != nullptr) //there is a subhash, so check there
    {
        return sh->contains(genome);
    }
    else
    {
        QVector<SpeciesBinEntry *> *binPointer = (QVector<SpeciesBinEntry *> *) & (binData.at(bin).binEntries);
        int rcount = binPointer->count();
        for (int i = 0; i < rcount; i++) //check in sequence
            if (sis->isEqual(genome, binPointer->at(i)->genome)) return true; //found it

        return false; //no, not found
    }
}

//custom new and delete to use custom memory manager
void *GenomeHashTable::operator new (size_t size)
{
    return customMemoryManager->alloc(size);
}

//this is just the memory freeing bit of deletion - which we do nothing for - cmm clears it all after each iteration
void GenomeHashTable::operator delete (void *p)
{
    //do nothing!
    //RJG - compiler warning suppression
    Q_UNUSED(p);
}
