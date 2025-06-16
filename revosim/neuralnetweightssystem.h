#ifndef NEURALNETWEIGHTSSYSTEM_H
#define NEURALNETWEIGHTSSYSTEM_H

#include "system.h"
#include "neuralnet.h"

class NeuralNetWeightsSystem : public System
{
public:
    NeuralNetWeightsSystem();

    void GetWeightsFromGenome(float *weights, const quint32 *genome);
    NeuralNet* CreateNeuralNet();
private:
    void GetWeightsFromGenome64(float *weights, const quint32 *genome);
    void GetWeightsFromGenome128(float *weights, const quint32 *genome);
    void GetWeightsFromGenome196(float *weights, const quint32 *genome);
    void GetWeightsFromGenome256(float *weights, const quint32 *genome);
};

#endif // NEURALNETWEIGHTSSYSTEM_H
