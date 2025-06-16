#include "cellsettings.h"

CellSettings::CellSettings()
{
    /***** CELL VARIABLES *****/
    //These can be defined per cell
    slotsPerSquare = 100;
    startAge = 15;
    settleTolerance = 15;
    target = 66;
    dispersal = 15;
    food = 2000;
    breedThreshold = 500;
    interactions = 10;
    breedCost = 500;
    mutate = 10;
    pathogenMutate = 5;
    pathogenFrequency = 5;
    minDeltaPredatorness = 0;
    predationEfficiency = 10;
    croppingFrequency = 0;
    asexual = false;
    obligateSexual = true;
    facultativeSexual = false;
    variableBreed = false;
    variableMutate = false;
    breedSpecies = false;
    breedDifference = true;
    pathOn = false;
    interactBlocks = false;
    multiBreedList = false;
    noSelection = false;
    interactFitness = false;
    interactEnergy = false;
    interactWithinSpecies = true;
    prisonersDilemmaFitness = true;
}
