#include "neuron_relu.h"

Neuron_ReLu::Neuron_ReLu() : Neuron()
{

}

float Neuron_ReLu::ActivationFunction(float v)
{
    if (v<0)
        return 0;
    else
        return v;
}
