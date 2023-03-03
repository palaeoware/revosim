#include "interactionsystem.h"
#include "simmanager.h"
#include "system.h"

InteractionSystem::InteractionSystem() : System("Interaction system")
{

}

int InteractionSystem::performInteractFitness(quint32 *genome, quint32 *targetGenome, int own_fitness, bool interactBlocks)
{
    //RJG - interaction based on blocks. Two interacting genomes split into four blocks, eight bits each
    if (interactBlocks)
    {
        for (int i = 0; i < useGenomeWordsCount; i++)
        {
            quint32 running_own_genome = genome[useGenomeWords[i]];
            quint32 running_target_genome = targetGenome[useGenomeWords[i]];
            // Find interacting genome chunks and modify fitness based on the chunk contents, as dictated by the interactions lookup table
            for (int chunk = 0; chunk < 4; chunk++)
            {
                quint32 Org1_Coord = running_own_genome % 256;
                quint32 Org2_Coord = running_target_genome % 256;
                own_fitness += interaction[Org1_Coord][Org2_Coord]; // Modify fitness
                running_own_genome = running_own_genome / 256;
                running_target_genome = running_target_genome / 256;
            }
        }
    }
    //RJG - interaction based on XORing genomes, coupled with a bitshift
    else
    {
        for (int i = 0; i < useGenomeWordsCount; i++)
        {
            quint32 temp_target_genome = targetGenome[useGenomeWords[i]];
            quint32 temp_own_genome = genome[useGenomeWords[i]];
            temp_own_genome >>= 1; // Bitshift
            temp_own_genome += ((genome[useGenomeWords[i]] % 2) * 2147483648); // Wrap around the lost digit
            own_fitness += bitCount(temp_own_genome ^ temp_target_genome);

            temp_own_genome = genome[useGenomeWords[i]];
            temp_own_genome <<= 1; // Bitshift
            temp_own_genome += (genome[useGenomeWords[i]] / 2147483648); // Wrap around the lost digit
            own_fitness -= bitCount(temp_own_genome ^ temp_target_genome);
        }
    }
    if (own_fitness < 0) // Prevents underflow of TotalFitness variable
    {
        own_fitness = 0;
    }
    return own_fitness;
}

int InteractionSystem::performInteractEnergy(quint32 *genome, quint32 *targetGenome, int minPredatorDelta, int targetEnergy, bool interactBlocks)
{
    int predationTarget = 12 * useGenomeWordsCount;
    int checktotal = 0;
    if (interactBlocks)
    {
        for (int i = 0; i < useGenomeWordsCount; i++)
        {
            quint32 running_own_genome = genome[useGenomeWords[i]];
            quint32 running_target_genome = targetGenome[useGenomeWords[i]];
            for (int chunk = 0; chunk < 4; chunk++)
            {
                quint32 Org1_Coord = running_own_genome % 256;
                quint32 Org2_Coord = running_target_genome % 256;
                checktotal += interaction[Org1_Coord][Org2_Coord]; // Modify checktotal, to determine predator score
                running_own_genome = running_own_genome / 256;
                running_target_genome = running_target_genome / 256;
            }
        }
        checktotal = predationTarget - qAbs(predationTarget - checktotal);
        if (checktotal > minPredatorDelta) return (checktotal * targetEnergy / predationTarget);
        else return 0;
    }
    else
    {
        for (int i = 0; i < useGenomeWordsCount; i++)
        {
            quint32 temp_target_genome = targetGenome[useGenomeWords[i]];
            quint32 temp_own_genome = genome[useGenomeWords[i]];
            temp_own_genome >>= 1; // Bitshift
            temp_own_genome += ((genome[useGenomeWords[i]] % 2) * 2147483648); // Wrap around the lost digit
            checktotal += bitCount(temp_own_genome ^ temp_target_genome);

            temp_own_genome = genome[useGenomeWords[i]];
            temp_own_genome <<= 1; // Bitshift
            temp_own_genome += (quint32)(genome[useGenomeWords[i]] / 2147483648); // Wrap around the lost digit
            checktotal -= bitCount(temp_own_genome ^ temp_target_genome);
        }
        checktotal = predationTarget - qAbs(predationTarget - checktotal);
        //if (checktotal > minPredatorDelta) return (checktotal * checktotal * targetEnergy / predationTarget / predationTarget); // This is the nonlinear verison
        if (checktotal > minPredatorDelta) return (checktotal * targetEnergy / predationTarget); // This is the linear version.
        else return 0;
    }
}
