#ifndef VISUALISATIONSYSTEM_H
#define VISUALISATIONSYSTEM_H

#include "system.h"
#include <QColor>

class VisualisationSystem : public System
{
public:
    VisualisationSystem();
    QColor returnRGB(const int n, const int m);
    QString returnGenomeString(const quint32 *genome, int localGenomeSize);
};

#endif // VISUALISATIONSYSTEM_H
