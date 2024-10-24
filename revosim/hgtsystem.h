#ifndef HGT
#define HGT


#include "system.h"
#include <QVector>
#include <QColor>

class HgtSystem : public System
{
public:
    HgtSystem();
    bool willTransform();
    quint32 generateMask(quint32 mask);
    quint32 GenerateTransform(quint32 genometransfer, quint32 mask);
    void Transform(quint32* genome, quint32 genometransfer, quint32 mask);
private:
    int generateTransferLength();

};


#endif // PATHOGENSSYSTEM_H
