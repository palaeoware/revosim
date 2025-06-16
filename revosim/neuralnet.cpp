#include "neuralnet.h"
#include "neuron_relu.h"
#include <QDebug>
//Assumptions:

//layer 0 is inputs
//last layer is single output layer

NeuralNet::NeuralNet(int inputs, int layer1, int layer2)
{
    inputCount = inputs;
    layer1Count = layer1;
    layer2Count = layer2;
    int weightCount = 0;
    for (int i=0; i<inputCount; i++)
    {
        Neuron_Input *n = new Neuron_Input();
        inputNeurons.append(n);
    }

    for (int i=0; i<layer1Count; i++)
    {
        Neuron *n = (Neuron *)new Neuron_ReLu();
        for (int j=0; j<inputCount; j++)
        {
            n->AddFeed(inputNeurons[j], weightCount++);

        }
        hiddenLayer_1.append(n);
    }

    for (int i=0; i<layer2Count; i++)
    {
        Neuron *n = (Neuron *)new Neuron_ReLu();
        for (int j=0; j<layer1Count; j++)
        {
            n->AddFeed(hiddenLayer_1[j], weightCount++);
        }
        hiddenLayer_2.append(n);
    }

    outputNeuron = new Neuron_Output();
    for (int j=0; j<layer2Count; j++)
    {
        outputNeuron->AddFeed(hiddenLayer_2[j], weightCount++);
    }

}

float NeuralNet::Evaluate(float *weights, float *inputs)
{
    for (int i=0; i<inputCount; i++)
        inputNeurons[i]->value = inputs[i];

    for (int i=0; i<hiddenLayer_1.count(); i++)
    {
        hiddenLayer_1[i]->Evaluate(weights);
    }
    for (int i=0; i<hiddenLayer_2.count(); i++)
    {
        hiddenLayer_2[i]->Evaluate(weights);
    }

    float output = outputNeuron->Evaluate(weights);
    return output;
}
