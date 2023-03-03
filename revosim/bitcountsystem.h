#ifndef BITCOUNTSYSTEM_H
#define BITCOUNTSYSTEM_H

#include "system.h"

class BitCountSystem : public System
{
public:
    BitCountSystem();
    int calculateBitCountWithJitter(const quint32 *genome);
};

#endif // BITCOUNTSYSTEM_H
