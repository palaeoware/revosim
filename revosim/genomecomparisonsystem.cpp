#include "genomecomparisonsystem.h"
#include "simmanager.h"

GenomeComparisonSystem::GenomeComparisonSystem() : System("Genome Comparison System")
{

}

//RJG when right clicking unless we want all, we need to find the modal genome
int GenomeComparisonSystem::findModalGenome(int x, int y)
{
    //RJG - data structure for the genome search
    quint32 genomes[SLOTS_PER_GRID_SQUARE][MAX_GENOME_WORDS];
    int arraypos = 0; //Position in array
    int counts[SLOTS_PER_GRID_SQUARE];

    for (int c = 0; c < maxUsed[x][y]; c++)
    {
        //RJG - If living critter
        if (critters[x][y][c].age > 0)
        {
            //RJG - get genome
            quint32 *g = critters[x][y][c].genomeWords;

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

    //RJG - find max frequency, get genome
    int max = -1;
    quint32 maxg[MAX_GENOME_WORDS] = {0};

    for (int i = 0; i < arraypos; i++)
        if (counts[i] > max)
        {
            max = counts[i];
            for (int j = 0; j < useGenomeWordsCount; j++) maxg[useGenomeWords[j]] = genomes[i][useGenomeWords[j]];
        }

    for (int c = 0; c < maxUsed[x][y]; c++)
        if (critters[x][y][c].age > 0)
            if (isEqual(maxg, critters[x][y][c].genomeWords))
                return c;

    return -1;
}
