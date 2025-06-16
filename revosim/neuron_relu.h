#ifndef NEURON_RELU_H
#define NEURON_RELU_H

#include "neuron.h"

class Neuron_ReLu : public Neuron
{
public:
    Neuron_ReLu();
private:
    float ActivationFunction(float v) override;
};

#endif // NEURON_RELU_H
