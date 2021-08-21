.. _organisms:

Configuring your Organisms
==========================

At the core of the REvoSim simulation are 64-bit digital organisms. A number of properties of these organisms can be defined in the suimulation, in the Organism tab of the Settings dock. These settings are as follows:

:Chance of mutation: This dictates the chance of a mutation when an organism breeds (if it is set to *n*, there is a *n*/256 chance of mutation). The default is 10, which thus equates to a 10/256 chance that a randomly selected bit in the genome is flipped (from 0 to 1 or 1 to 0).

:Start age: Every iteration each organism loses one from its age counter. This setting dictates the value at which this counter is set when an organism is born. As such, this dictates generation times within the software, and will also impact on the number of times any individual can breed in its lifetime.

:Breed threshold: Within a pixel, the energy provided each iteration is split between the digital organisms living within the pixel based on their fitness. A full description of how this is achieved can be found in the REvoSim paper. When a digital organism has stored enough energy to pay the breed cost and still exceed the breed threshold, it attempts to breed.

:Breed cost: This is the amount of resource that is removed from an organisms when it successfully breeds.

:Maximum difference to breed: In sexual breeding mode, if two organisms attempting to breed have a hamming distance greater than this value when genomes are compared, breeding fails.

:Use maximum difference to breed: Depending on the nature of a study, maximum difference to breed may not be desired. This tickbox dictates whether it is enforced. If unticked, in sexual modes, breeding failure does not occur on the basis of genomic distance.

:Breed only within species: When this checkbox is ticked, during sexual selections, digital organisms can only breed with other members of the same species. This only occurs if species tracking is turned on.

:Breed mode: In sexual mode, organisms can breed with other individuals (or themselves) given the caveats outlined above. In asexual mode, self-breeding is enforced, and organisms are cloned when they have the required energy reserves to allow breeding.

:Dispersal: This figure dictates the extent to which juveniles disperse on settling. Small numbers equate to significant dispersal, larger numbers increase the likelihood that juveniles setting in the same pixel as their parent. How this is achieved is described in full in the REvoSim paper.

:Nonspatial settling: For some evolutionary phenomena, the impact of space/dispersal may have an unknown impact and not be the element of interest within a simulation, and thus be undesirable. This tickbox allows juveniles to be randomly placed within the simulation (note that with a non-uniform environment, space will still have some impact on the simulation).
