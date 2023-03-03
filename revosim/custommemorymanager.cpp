#include "custommemorymanager.h"

#include <QDebug>

//Simple memory manager -grabs blocks as needed
//and doles out memory as required
//no 'free' functionality - just reset at the end of an iteration

CustomMemoryManager::CustomMemoryManager()
{
    currentBlock = 0;
}

void CustomMemoryManager::allocNewBlock()
{
    //get a new block with malloc

    void *mem = malloc(BLOCK_SIZE);
    if (mem==nullptr)
    {
        qDebug()<<"Out of memory!";
        exit(1);
    }
    Block b;
    b.firstFreePos=0;
    b.memory=mem;
    blocks.append(b);
}

//custom alloc method
void * CustomMemoryManager::alloc(size_t size)
{
    //ensure memory is aligned to 8 byte boundary
    if (size%8!=0)
    {
        size = ((size/8)+1)*8;
    }

    if (size>BLOCK_SIZE)
    {
        //should never happen!
        qDebug()<<"Alloc call to custom memory manager is too big "<<size;
        exit(0);
    }
    //does current block exist?
    if (currentBlock >= blocks.length()) allocNewBlock();

    if ((BLOCK_SIZE - blocks[currentBlock].firstFreePos)<size)
    {
        currentBlock++;
        if (currentBlock >= blocks.length()) allocNewBlock();
    }

    //work out the position - have to cast as can't to arithmetic with void pointers
    void *allocedmem = (quint8 *) blocks[currentBlock].memory + blocks[currentBlock].firstFreePos;
    blocks[currentBlock].firstFreePos += size;
    return allocedmem;
}

//reset all memory usage - don't free any blocks though, will be reused
void CustomMemoryManager::reset()
{
    //free all my allocated blocks by simply resetting
    currentBlock=0;
    for (int i=0; i<blocks.length(); i++) blocks[i].firstFreePos=0;
}

//probably never needed - free all memory
void CustomMemoryManager::freeAll()
{
    for (int i=0; i<blocks.length(); i++) free(blocks[i].memory);
    currentBlock=0;
    blocks.clear();
}
