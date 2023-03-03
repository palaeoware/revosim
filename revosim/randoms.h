#ifndef RANDOMS_H
#define RANDOMS_H

#include <QRandomGenerator>

class randoms
{
public:
    randoms();
    quint8 rand8();
    quint16 rand16();
    quint32 rand32();
    quint64 rand64();

private:
    quint16 randoms16[65536];
    quint32 randoms32[65536];
    quint64 randoms64[65536];
    const quint8 mask8 = ~0;
    const quint16 mask16 = ~0;
    void newRandoms8();
    void newRandoms16();
    void newRandoms32();
    void newRandoms64();
    quint16 nextRandom8, nextRandom16, nextRandom32, nextRandom64;
};

#endif // RANDOMS_H
