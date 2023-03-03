#include "visualisationsystem.h"
#include "simmanager.h"
#include <bitset>

VisualisationSystem::VisualisationSystem() : System("Visualisation system")
{

}

//RJG - system for visualising gene words
QColor VisualisationSystem::returnRGB(const int n, const int m)
{
    //RJG - data structure for mode
    quint32 genomes[SLOTS_PER_GRID_SQUARE][MAX_GENOME_WORDS];
    int counts[SLOTS_PER_GRID_SQUARE];
    int arraypos = 0; //Position in array

    //RJG - if empty we can return black
    if (totalFitness[n][m] == 0) return Qt::black;

    //RJG - Otherwise we should find the modal genome
    //RJG - Do this by checking each slot against the list of genomes we've already found
    for (int c = 0; c < maxUsed[n][m]; c++)
    {
        //RJG - If living critter
        if (critters[n][m][c].age > 0)
        {
            //RJG - get genome
            quint32 *g = critters[n][m][c].genomeWords;

            //RJG - find frequencies
            bool found = false;
            for (int i = 0; i < arraypos; i++)
                if (isEqual(g, genomes[i]))
                {
                    counts[i]++;
                    found = true;
                    break;
                }

            if (!found)
            {
                for (int i = 0; i < useGenomeWordsCount; i++) genomes[arraypos][useGenomeWords[i]] = g[useGenomeWords[i]];
                counts[arraypos++] = 1;
            }

        }
    }


    //RJG - find max frequency, record genome
    int max = -1;
    quint32 maxg[MAX_GENOME_WORDS] = {0};

    for (int i = 0; i < arraypos; i++)
        if (counts[i] > max)
        {
            max = counts[i];
            for (int j = 0; j < useGenomeWordsCount; j++) maxg[useGenomeWords[j]] = genomes[i][useGenomeWords[j]];

        }

    int bitsPerChannel = (useGenomeWordsCount * 32) / 3;

    std::bitset<MAX_GENOME_WORDS * 32> bitSetGenome;

    //RJG - convert genome to contiguous bitset for easy counting etc.
    int cnt = 0;
    for (int i = 0; i < useGenomeWordsCount; i++)
    {
        std::bitset<32> wordBitSet(maxg[useGenomeWords[i]]);
        for (int j = 0; j < 32; j++)
        {
            bitSetGenome[cnt] = wordBitSet[j];
            cnt++;
        }
    }

    //RJG - work out bits per channel
    int RGB[3] = {0};
    int channel = 0;
    for (int i = 0; i < useGenomeWordsCount * 32; i += bitsPerChannel)
    {
        for (int j = i; j < i + bitsPerChannel; j++) if (bitSetGenome[j])RGB[channel]++;
        channel++;
    }


    //RJG - now we have counts for each channel - normalise to 255 for each channel
    double dRGB[3];
    for (int i = 0; i < 3; i++) dRGB[i] = static_cast<double>(RGB[i]) * (255. / static_cast<double>(bitsPerChannel));
    for (int i = 0; i < 3; i++) RGB[i] = static_cast<int>(dRGB[i]);

    //RJG - return the colour
    return QColor(RGB[0], RGB[1], RGB[2]);
}

