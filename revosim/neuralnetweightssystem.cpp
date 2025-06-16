#include "neuralnetweightssystem.h"
#include "neuralnet.h"
#include <QDebug>

NeuralNetWeightsSystem::NeuralNetWeightsSystem() : System("Neural Net Weights System")
{


}

NeuralNet *NeuralNetWeightsSystem::CreateNeuralNet()
{
    switch (useGenomeWordsCount)
    {
    case 2:
        return new NeuralNet(6,2,2);
    case 4:
        return new NeuralNet(10,3,3);
    case 6:
        return new NeuralNet(10,4,4);
    case 8:
        return new NeuralNet(8,6,5);
    default:
        qDebug()<<"ERROR - neural weights system requires genome length 2,4,6 or 8";

    }
    return 0; //null
}


void NeuralNetWeightsSystem::GetWeightsFromGenome(float *weights, const quint32 *genome)
{
    switch (useGenomeWordsCount)
    {
        case 2:
            GetWeightsFromGenome64(weights, genome);
            return;
        case 4:
            GetWeightsFromGenome128(weights, genome);
            return;
        case 6:
            GetWeightsFromGenome196(weights, genome);
            return;
        case 8:
            GetWeightsFromGenome256(weights, genome);
            return;
        default:
            qDebug()<<"ERROR - neural weights system requires genome length 2,4,6 or 8";

    }

}

void NeuralNetWeightsSystem::GetWeightsFromGenome64(float *weights, const quint32 *genome)
{
    //64 bit is 2 games history so 6 inputs, 2 in layer 1, 2 in layer 2
    //18 weights, so can use 9 per word in 3-bit couplets

    quint32 genomeWord = genome[useGenomeWords[0]];

    for (int i=0; i<9; i++)
    {
        float v = genomeWord & 7; //least sig 3 bits
        weights[i]=v-4; //so it's -4 to +3
        genomeWord >>= 3;
    }

    genomeWord = genome[useGenomeWords[1]];

    for (int i=9; i<18; i++)
    {
        float v = genomeWord & 7; //least sig 3 bits
        weights[i]=v-4;
        genomeWord >>= 3;
    }
}

void NeuralNetWeightsSystem::GetWeightsFromGenome128(float *weights, const quint32 *genome)
{
    //128 bit is 4 games history so 10 inputs, 3 in layer 1, 3 in layer 2
    //42 weights, so we need to combine to 64 bits, then use 63 of each

    quint32 genomeWord0 = genome[useGenomeWords[0]];
    quint32 genomeWord1 = genome[useGenomeWords[1]];
    quint64 genomeWord64 = (((quint64)genomeWord0) << 32) + (quint64)genomeWord1;

    for (int i=0; i<21; i++)
    {
        float v = genomeWord64 & 7; //least sig 3 bits
        weights[i]=v-4; //so it's -4 to +3
        genomeWord64 >>= 3;
    }

    genomeWord0 = genome[useGenomeWords[2]];
    genomeWord1 = genome[useGenomeWords[3]];
    genomeWord64 = (((quint64)genomeWord0) << 32) + (quint64)genomeWord1;

    for (int i=21; i<42; i++)
    {
        float v = genomeWord64 & 7; //least sig 3 bits
        weights[i]=v-4;
        genomeWord64 >>= 3;
    }
}

void NeuralNetWeightsSystem::GetWeightsFromGenome196(float *weights, const quint32 *genome)
{
     //This one is 4 game history (10 inputs), 4/4 in layers, 60 weights over 6 genomes.
     //10 per genome - easy!

    for (int g=0; g<6; g++)
    {
        quint32 genomeWord = genome[useGenomeWords[g]];

        for (int i=0; i<10; i++)
        {
            float v = genomeWord & 7; //least sig 3 bits
            weights[i + g*10]=v-4; //so it's -4 to +3
            genomeWord >>= 3;
        }
    }
}

void NeuralNetWeightsSystem::GetWeightsFromGenome256(float *weights, const quint32 *genome)
{
    //This one is 3 game history, 6/5 in layers, so 8 inputs, 83 inputs
    //First 80 are the 10 from each genome

    for (int g=0; g<8; g++)
    {
        quint32 genomeWord = genome[useGenomeWords[g]];

        for (int i=0; i<10; i++)
        {
            float v = genomeWord & 7; //least sig 3 bits
            weights[i + g*10]=v-4; //so it's -4 to +3
            genomeWord >>= 3;
        }

        //80 = most sig bits from g0-2


    }

    //Final 3 - cobble from some of those ignored top bits
    float v = (genome[useGenomeWords[0]] >> 31) * 4 +  (genome[useGenomeWords[1]] >> 31) * 2
              +  (genome[useGenomeWords[2]] >> 31);
    weights[80]=v-4;

    //81 = most sig bits from g3-5
    v = (genome[useGenomeWords[3]] >> 31) * 4 +  (genome[useGenomeWords[4]] >> 31) * 2
        +  (genome[useGenomeWords[5]] >> 31);
    weights[81]=v-4;

    //82 = 2nd most sign bits from g0-2
    v = ((genome[useGenomeWords[0]] >> 30) & 1) * 4 +  ((genome[useGenomeWords[1]] >> 30) & 1) * 2
        +  ((genome[useGenomeWords[2]] >> 30) & 1);
    weights[82]=v-4;
}

