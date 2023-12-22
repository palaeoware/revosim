.. _settingsinteractions:

Configuring Interactions
========================

Interaction settings
--------------------

Interactions were added to REvoSim for the release of v3.0.0. These are tools that allow individuals (or segments of individuals' genomes) in REvoSim simulations to interact with other individuals in the same cell. This occurs when offspring are settling, or every iteration, as described below. Interactions can increase or decrease the fitness of an individual, or its energy. In general, interaction systems compare the genomes (or parts thereof) of two individuals - individual 1, to which any fitness modification is applied, and individual 2. When interactions are applied, individual 1 typically interacts with multiple other individuals (i.e. the program can loop through a random selection of individual 2s), and this process is repeated for every individual in a cell (except in instances where interactions are only applied on settling, i.e. fitness interactions when recalculate fitness is not selected).

:Genome blocks: The genomes of both individuals are split into two-bit blocks: each two bits can be a number from 0-3 (i.e. 00,01,10 or 11). For every two bit block, the numbers in each genome are used to look up a value from the a-priori interaction table (this is a 4x4 grid of user-set interaction numbers that can be modified by clicking *Edit the interactions grid*). The values are summed across all two bit blocks of the genome. For fitness interactions, this total is added to the fitness of organism 1. See below for energy interactions.

:Genome XOR: Here a bit shift is applied to the genome of individual 1, which is then compared to individual 2. More specifically, the bits in the genome of individual 1 are shifted one to the right, and the results of an exclusive or between the resulting binary string and the genome of individual two are subject to a bit count, providing a number. The process is then repeated with a bit shift in the other direction for individual 1, to provide a second number. For fitness interactions, this number is subtracted from the first, and the resulting number is used to modify the fitness of individual 1. See below for energy interactions.

:Interactions: This sets the number of interactions attempted by each organism. In fitness interactions this occurs when fitness is calculated - this means that, by default, this only occurs during settling, unless the simulation is set to recalculate fitness every iteration. When energy interactions are selected this is is the number of interactions that each individual attempts every iteration.

:Interactions Change Fitness: Interactions modify the fitness of organisms (see above for a note on when this is applied). Note that partners for each are chosen at random, and if an empty slot is selected, no interaction is performed. The total change to the fitness of an individual is the sum of all successful interactions.

:Interactions Change Energy: In energy interactions the absolute value of either the interactions lookup (genome blocks) or the difference between bit shifts (genome XOR) is subtracted from a predation target, currently hard coded as 12 * the number of genome words in use in the system. The absolute value of the result is then subtracted from the predation target, and that number n is used to determine what proportion of organism 2's energy is lost by that organism. n/(predation target) of organism 2's energy is lost.

:Interactions Do Nothing: Do not apply interactions.

:Edit the interactions grid: Edit the 4x4 grid of user-set interaction numbers.

:Min. Predator Score (Energy Only): It may occasionally be useful to prevent interactions from resulting in any transfer of energy between organisms unless the predator organism is sufficiently well adapted for predation. In this case, this setting can be used to prevent organisms from obtaining energy through interactions when their predation score is below some threshold.

:Predation Efficiency (Energy Only): When interactions result in the direct transfer of energy from one organism to another, only a fraction of the energy lost by organism 2 is gained by organism 1 (as is realistic). This fraction is 10 * the value for this setting (e.g. a value of 3 would represent 30% energy transfer efficiency).

:Restrict interactions: This setting only functions when interactions change energy, and after 100 iterations to allow trophic levels to become established. After this point, it prevents organisms from gaining energy through interactions unless their target organism has a current trophic level (prior to the interaction) at least 0.5 lower than their own trophic level. It thus prevents primary consumers from acting as facultative secondary consumers in addition to being primary consumers. Trophic level is defined over the lifetime of an organism, based on an average of the sources from which that organism gains its energy, weighted by the amount of energy gained from each source.


Pathogen settings
-----------------

REvoSim v3.0.0 also includes a newly added pathogens system. Given the abstract nature of the software, these could equally be considered equivalent to predators: in a basic sense they act as antagonists. At present there are two options, drift and evolve:

:Drift: With drifting pathogens, there is one pathogen per cell. Each has pathogen comprises a genome of *n* words. Pathogens mutate with a user-defined probability/generation, and are applied with a user-defined frequency. This system works by performing an exclusive or between the pathogen and each living creature, and then summing the ones. It then uses this sum to define a probability of that individual dying as a result: when the result of the XOR is zero (i.e. pathogen and organism genome are identical), the probability of the organism dying from the interaction is 0.5. Where the result of the XOR is maximised, this is 0. There is a linear relationship between these extremes (other probability distributions can be provided on request). An organism is killed by setting its age to 1, resulting in its removal in the next iteration.

:Evolve: Pathogens evolve for virulence when this setting is selected. With this setting, there are 5 pathogens per cell, whose fitness is defined by how many organisms they kill in an iteration. Every iteration, the pathogens for a given cell, and those orthogonal to it (i.e. a 3 x 3 square of pixels, minus the diagonals), are applied to the individuals in a given cell. The pathogen that kills the most organisms living in that cell (or the first one to reach this number if there is more than one) is applied to the organisms in the cell. It is then replicated, mutated following the pathogen mutate probability (currently this is set to n/256 chance of mutation irrespective of number of words the system applies to), and then placed back in the pathogens layer overwriting a previous pathogen at random prior to the for next pathogen iteration. Including the cells orthogonal to that which pathogens are being applied allows the pathogens to move across generations, and thus follow e.g. species and environments.

For both forms of pathogen, the follow options are available:

:Pathogen mutation: This defines the probability of a mutation occurring any iteration that pathogens are applied. Smaller numbers define a smaller probability. This works by generating an 8bit random number (0-255), and applying a mutation if that number is *less than* the integer in the checkbox so, for example, if this is 1, the probability of a mutation occurring is 1/256.

:Pathogen frequency: This is the frequency, in iterations, with which pathogens are applied.

There are a great many other mechanisms by which antagonists could operate, and if you are interested in modifications, please contact the authors.