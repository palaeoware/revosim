#include "neuron.h"
#include <QList>
#include <QDebug>

void Neuron::AddFeed(Neuron *input, int weight)
{
    Feeder *feeder = new Feeder();
    feeder->neuron = input;
    feeder->weight = weight; //index for weight
    inputs.append(feeder);
    inputCount = inputs.count();
}

Neuron::Neuron()
{
    inputCount=0;
    value =  0;

}

float Neuron::Evaluate(float *weights)
{
    float sum = 0;
    for (int i=0; i<inputCount; i++)
    {
        sum+=inputs[i]->neuron->value * weights[inputs[i]->weight];
        //qDebug()<<"Sum now "<<sum;
    }

    value = ActivationFunction(sum);
    return value;
}


float Neuron::ActivationFunction(float v)
{
    return v;
}

Neuron::~Neuron()
{
    for (int i=0; i<inputCount; i++)
        delete inputs[i];
}
