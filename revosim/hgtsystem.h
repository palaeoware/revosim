#ifndef HGT
#define HGT


#include "system.h"
#include <QVector>
#include <QColor>

class HgtSystem : public System
{
public:
    HgtSystem();
    void generateMask (quint32* genome, quint32* mask);
    bool willTransform();
    void generateTransfer(quint32* donorGenome, quint32* positionMask, quint32* donorMask);
    void transformRecipient(quint32* genome, quint32* positionMask, quint32* donorMask);
    void transform(quint32* genome, quint32* positionMask, quint32* donorMask);
    void shift(quint32* mask, quint32* maskofdonor);


private:
    quint32 generateTransferLength(quint32* genome);
    quint32 circularLeftShift(quint32 currentWord, quint32 nextWord);
    quint32 circularRightShift(quint32 currentWord, quint32 previousWord);
};


#endif // PATHOGENSSYSTEM_H
