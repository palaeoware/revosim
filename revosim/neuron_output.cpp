#include "neuron_output.h"
#include "math.h"
Neuron_Output::Neuron_Output(): Neuron()
{

}

float Neuron_Output::ActivationFunction(float v)
{
    //Sigmoidal, https://www.v7labs.com/blog/neural-networks-activation-functions
    return (1.0/(1+exp(-v)));
}
