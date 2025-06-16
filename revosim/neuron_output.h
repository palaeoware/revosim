#ifndef NEURON_OUTPUT_H
#define NEURON_OUTPUT_H

#include "neuron.h"

class Neuron_Output : public Neuron
{
public:
    Neuron_Output();
private:
    float ActivationFunction(float v) override;
};

#endif // NEURON_OUTPUT_H
