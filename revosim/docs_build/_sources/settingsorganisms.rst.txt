.. Below allows arrows to be included - needs to be above page label in newer versions of sphinx
.. include:: <isonum.txt>

.. _organisms:

Configuring your Organisms
==========================

At the core of the REvoSim simulation are the digital organisms. A number of properties of these organisms can be defined in the simulation, in the Organism tab of the Settings dock. These settings are as follows:

Organism settings
-----------------

:Chance of mutation: This dictates the chance of a mutation when an organism breeds (if it is set to *n*, there is a *n*/256 chance of mutation). The default is 10, which thus equates to a 10/256 chance that a randomly selected bit in the words of the genome to which this is applied (see below) is flipped (from 0 to 1 or 1 to 0).

:Variable mutation: This uses the number of ones in a user-defined section of the genome to dictate the probability of a mutation occurring. This uses a cumulative standard normal distribution from -3 to 3, created using the math.h complementary error function and then scaled between zero and the maximum random number. This allows the probability of mutation occurring to be controlled by generating a random number, and applying a mutation with 99.865% probability if an individual has no ones in a 32-bit section of controlling genome, to 0.24579% with 31 ones. If you are using this you will probably want the mutation log enabled (Tools |rarr| Custom Logging |rarr| Variable mutation log). This outputs a histogram for the grid showing the number of ones in the non-coding genome (as well as providing an overview of mutation probabilities).

:Start age: Every iteration each organism loses one from its age counter. This setting dictates the value at which this counter is set when an organism is born. As such, this dictates generation times within the software.

Breed settings
-----------------

:Breed threshold: Within a pixel, the energy provided each iteration is split between the digital organisms living within the pixel based on their fitness. A full description of how this is achieved can be found in the REvoSim paper. When a digital organism has stored enough energy to pay the breed cost and still exceed the breed threshold, it attempts to breed.

:Breed cost: This is the amount of resource that is removed from an organisms when it successfully breeds.

:Maximum difference to breed: In sexual breeding mode, if two organisms attempting to breed have a hamming distance greater than this value when genomes are compared, breeding fails.

:Use max difference to breed: Depending on the nature of a study, maximum difference to breed may not be desired. This tick box dictates whether it is enforced. If unticked, in sexual modes, breeding failure does not occur on the basis of genomic distance.

:Breed only within species: When this checkbox is ticked, during sexual selections, digital organisms can only breed with other members of the same species. This only occurs if species tracking is turned on.

:Multiple breed lists: In order to promote the coexistence of different genotypes within cells (by reducing the pressure on organisms to be breed compatible with all other organisms in their cell), this option creates multiple breed lists per cell. Organisms are placed onto one of 66 breed lists based on the remainder when their bit count is divided by 67, accompanied by a level of jitter created through adding a random integer between -1 and 1, to allow mixing between adjacent lists.

:Breed mode: REvoSim offers multiple breed modes, given the above caveats (e.g. breed within species):

    :Obligate sexual: Organisms can only breed with other individuals (this is the case in most animals, for example).

    :Facultative sexual: Organisms can reproduce with other individuals, or themselves (see e.g. plants).  Asexual reproduction is more common in this mode when populations are small.

    :Asexual mode: In asexual mode, self-breeding is enforced and organisms are cloned when they have the required energy reserves to allow breeding.

    :Variable mode:  This uses the same approach as variable mutation - the number of ones in a user-defined portion of the genome is used to define the probability of breeding asexually v.s. sexually. It uses the same distribution, and when this is enabled you can select the recombination log enabled (Tools |rarr| Custom Logging |rarr| Recombination logging). Note that sexual reproduction when using this mode is facultative, so can include self pairing. The recombination log records how often each breed mode is used across all breeding organisms each polling iteration, and then reports this.

Settle settings
-----------------

:Dispersal: This figure dictates the extent to which juveniles disperse on settling. Small numbers equate to significant dispersal, larger numbers increase the likelihood that juveniles settle in the same pixel as their parent. How this is achieved is described in full in the REvoSim paper.

:Nonspatial settling: For some evolutionary phenomena, the impact of space/dispersal may have an unknown impact and not be the element of interest within a simulation, and thus be undesirable. This tick box allows juveniles to be randomly placed within the simulation (note that with a non-uniform environment, space will still have some impact on the simulation).

Genome Words and Systems
------------------------

From REvoSim 3.0.0, genomes are no longer limited to 64-bits in length - rather the user can set the genome length of organisms to be between 1 and 32 words, each of which is 32-bits. All options (and associated functions) that can be applied to bits in a genome are called systems, and these systems can be applied to different words of the genome to achieve the desired outcome. Thus, for instance, to match the behaviour of REvoSim v2.0.0, organisms can be set to include a 2 word genome, the fitness system can be applied to word 0, and the breed and species ID systems can be applied to words 0 and 1. Where there is an on/off toggle in the settings for an option, systems are only applied if enabled in that toggle. 

:Genome Size: This dictates the number of genome word for all organisms in the simulation. Its minimum is limited by the words entered in the systems below (i.e. if a system is applied to word number four, you cannot reduce this value below that word). 

The options below this allow systems to be applied to words of the genome. Counts use C++ numbering, thus start from zero, and above nine continue from A-V. When a string is entered, the label will turn green if it can be applied with the current settings. If a string requires that the number of words is increased, this is implemented in the genome size option above. If an option is not possible, the label turns red. Some options, when changed, provide further prompts suggesting changes of relevant settings to achieve a viable configuration. Options dictate the following:

:Fitness: Which words are used in the REvoSim fitness algorithm.

:Breed: Words used to calculate breeding compatibility.

:Mutate: Words to which mutations can be applied.

:Variable Mutate: Words used to dictate mutation probability.

:Pathogens: Words used by the pathogen system.

:Species ID: Which words are used in species searches for reproductively isolated clusters.

:Interactions: Words use for the interactions systems.

:Visualisation 1: Words used for visualisation system 1 (see :ref:`populationscene`).

:Visualisation 2: Words used for visualisation system 2. 

