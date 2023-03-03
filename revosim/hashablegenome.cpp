#include "hashablegenome.h"
#include <cstring>
#include <QDebug>


//NO LONGER USED!

//Provides a class that plays nicely with qHash for storing variable length genomes
//Does not reimplement = operator, which will work but will copy pointer to genome
//For deep copy copying data, use constuctor with pointer to genome to copy from
//or use the copyFrom method

//HashableGenome assigns its own genome memory if necessary and frees in the destructor


//Use an existing pointer to data
HashableGenome::HashableGenome(quint32 *g, int size)
{
    genome = g;
    length = size;
    ownsData = false;
}

//Copy from existing hashable genome (deep copy) - this is system copy constructor
HashableGenome::HashableGenome(const HashableGenome &from)
{
    length = from.length;
    genome = mallocData();
    std::memmove(genome,from.genome,length*4);
}

//Construct using given size (allocates genome memory)
HashableGenome::HashableGenome(int size)
{
    length = size;
    genome = mallocData();
}

//get own memory
quint32 *HashableGenome::mallocData()
{
    ownsData=true;
    return (quint32*) malloc(length*4);
}

//free memory in destructor if assigned
HashableGenome::~HashableGenome()
{
    if (ownsData) free(genome);
}

//Override the = operator - does DEEP copy
HashableGenome& HashableGenome::operator=(HashableGenome other)
{
    length = other.length;
    genome = mallocData();
    copyDataFrom(&other);
    return *this;
}

//copy data from other hashable genome - deep copy. If length is different, re-allocs
void HashableGenome::copyFrom(HashableGenome *from)
{
    if (!ownsData)
    {
        length = from->length;
        genome = mallocData();
    }
    else
    {
        if (length!=from->length)
        {
            free(genome);
            length = from->length;
            genome = mallocData();
        }
    }
    copyDataFrom(from);
}

//internal function for doing actual copy using memmove
inline void HashableGenome::copyDataFrom(HashableGenome *from)
{
    std::memmove(genome,from->genome,length*4);
}


