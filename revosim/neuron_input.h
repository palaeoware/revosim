#ifndef NEURON_INPUT_H
#define NEURON_INPUT_H

#include "neuron.h"

class Neuron_Input : public Neuron
{
public:
    Neuron_Input();
    float Evaluate(float *weights) override;
};

#endif // NEURON_INPUT_H
