#ifndef MUTATIONSYSTEM_H
#define MUTATIONSYSTEM_H

#include "system.h"

class MutationSystem : public System
{
public:
    MutationSystem();
    void doMutation(quint32 *genome);
};

#endif // MUTATIONSYSTEM_H
