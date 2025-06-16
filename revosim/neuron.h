#ifndef NEURON_H
#define NEURON_H
#include <QList>

class Neuron;

struct Feeder
{
public:
    int weight; //index
    Neuron *neuron;
};

class Neuron
{
public:
    Neuron(int inputCount);
    float value;
    QList<Feeder *>inputs;
    int inputCount;

    virtual float Evaluate(float *weights);
    Neuron();
    ~Neuron();
    void AddFeed(Neuron *input, int weight);
private:
    virtual float ActivationFunction(float v);
};


#endif // NEURON_H
