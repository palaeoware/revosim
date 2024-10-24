#ifndef CELLSETTINGS_H
#define CELLSETTINGS_H


class CellSettings
{
public:
    CellSettings();

    /***** CELL VARIABLES *****/
    //All of these apply to a single cell, and so are grouped here
    int slotsPerSquare; //Number of critters per grid square
    int startAge; //Age of individuals at birth
    int target; //Fitness target
    int settleTolerance; //Outside this value settling fails
    int dispersal; //Extent to which juveniles dispers
    int food; //Energy per cell
    int interactions; // Maximum ecological interactions each organism has upon settling
    int breedThreshold; //Energy accumulation needed to breed
    int breedCost; //Cost of breeding - cost applies for success
    int mutate; //Probability of mutating out of 255
    int pathogenMutate; //Pathogen probability to mutate any given iteration out of 255
    int pathogenFrequency; //Frequency of applying pathogens
    int minDeltaPredatorness; //Threshold difference in predator score for direct enregy theft to occur if such interactions are enabled.
    float predationEfficiency; // Conversion factor applied during direct energy theft interactions
    int croppingFrequency; //Rate at which biological cropping occurs
    //Reproduction modes - variable breed uses non coding genome to dictate recombination
    bool asexual;
    bool obligateSexual;
    bool facultativeSexual;
    bool variableBreed;
    bool variableMutate; // Use non coding genome to dictate mutation probability
    bool breedSpecies; //Breed only within species
    bool breedDifference; //Enforce max difference to breed
    bool pathOn; //Apply pathogens
    bool multiBreedList; // Segregate breed lists by genome bit count
    bool interactBlocks; //Apply ecology using blocks system
    bool noSelection; //Don't apply fitness within a cell
    bool interactFitness; //Interactions influence fitness
    bool interactEnergy; //Interactions influence energy directly
    bool interactWithinSpecies; //Allows interactions between members of the same species
    bool hgtTransform;
    int hgtTransferLength;
    int hgtTransferChance;

};

#endif // CELLSETTINGS_H
