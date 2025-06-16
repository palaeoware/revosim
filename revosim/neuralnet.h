#ifndef NEURALNET_H
#define NEURALNET_H

#include "neuron.h"
#include "neuron_input.h"
#include "neuron_output.h"
#include <QList>


class NeuralNet
{
public:
    NeuralNet(int inputs, int layer1, int layer2);
    int inputCount, layer1Count, layer2Count;
    QList<Neuron_Input *> inputNeurons;
    Neuron_Output *outputNeuron;
    QList<Neuron *> hiddenLayer_1;
    QList<Neuron *> hiddenLayer_2;

    float Evaluate(float *weights, float *inputs);
};

#endif // NEURALNET_H
