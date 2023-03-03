#include "speciesbinentry.h"
#include "simmanager.h"
#include "custommemorymanager.h"

//An entry in the genomehastable, with an occurrence count and a group

SpeciesBinEntry::SpeciesBinEntry()
{
    occurrenceCount = 1;
    group = -1;
}

//custom new and delete, as per genomehashtable
void *SpeciesBinEntry::operator new (size_t size)
{
    return customMemoryManager->alloc(size);
}

void SpeciesBinEntry::operator delete (void *p)
{
    Q_UNUSED(p);
}

