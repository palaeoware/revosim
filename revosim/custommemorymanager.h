#ifndef CUSTOMMEMORYMANAGER_H
#define CUSTOMMEMORYMANAGER_H

#define BLOCK_SIZE 65536

#include <QVector>

class Block
{
public:
    void *memory;
    quint32 firstFreePos;
};

class CustomMemoryManager
{
public:
    CustomMemoryManager();
    void *alloc(size_t size);
    void reset();
    int currentBlock;
    QVector<Block> blocks;
    void freeAll();
private:
    void allocNewBlock();
};

extern CustomMemoryManager *customMemoryManager;

#endif // CUSTOMMEMORYMANAGER_H
