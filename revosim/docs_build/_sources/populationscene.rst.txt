.. _populationscene:

Population Scene
================

The Population Scene is the program's visual output showing what is taking place within the defined siumlation space. When the program is first started, or after a Reset is called, the Population Scene will default to showing a single coloured pixel in the center of the population grid, unless Dual Reseed is selected in which case two coloured pixels with be shown. This pixel represents the starting genome of the seeding organism: black pixels are those without any living digital organisms. 

.. figure:: _static/populationSceneDefault.png
    :align: center

    Figure 3.3.1 - Default Population Scene showing a single starting genome in the centre.

.. figure:: _static/populationSceneDuelReseed.png
    :align: center

    Figure 3.3.2 - Default Population Scene showing a two genomes, one on left and another on the right, as set by the Duel Reseed option.

The Population Scene can be set using the dropdown menu to show different output modes. The the Population Scene currently supports the following visual modes:

1. Population Count
2. Mean Fitness
3. Coding Genome as Colour
4. Non-Coding Genome as Colour
5. Gene Frequencies
6. Settles
7. Breed/Settle Fails (R=Breed; G=Settle)
8. Species

Population Count
----------------
Each grid-square is visualized with a grey-level representing the current number of creatures alive in the square

.. figure:: _static/populationSceneCount.png
    :align: center

Mean Fitness
------------

Each grid-square is visualized with a grey-level representing the mean fitness of all creatures alive in the square, scaled so that white is maximum fitness (=‘Settle Tolerance’).

.. figure:: _static/populationSceneMeanFitness.png
    :align: center

Coding Genome as Colour
-----------------------

For each grid-square, the most commonly occurring (modal) genome is computed. The 32-bit coding part of this genome is then converted to a colour, where the level of red is the (scaled) count of 1’s in the least significant 11 bits, the level of green is the (scaled) count of ‘1’s in the next least significant 11 bits, and the level of blue is the (scaled) count of ‘1’s in the most significant 10 bits. This visualization approach provides a quick means of distinguishing genomes, ensuring that small genomic changes result in small changes in colour. It does not, however, guarantee that the same colour will in all cases represent the same genome (as many very different genomes may possess the same bit-counts in each of the three 11/10 bit segments).

.. figure:: _static/populationSceneCodingGenome.png
    :align: center

Non-coding Genome as Colour
---------------------------


For each grid-square, the most commonly occurring (modal) genome is computed. The 32-bit non-coding part of this genome is then converted to a colour, where the level of red is the (scaled) count of 1’s in the least significant 11 bits, the level of green is the (scaled) count of ‘1’s in the next least significant 11 bits, and the level of blue is the (scaled) count of ‘1’s in the most significant 10 bits. This visualization approach provides a quick means of distinguishing genomes, ensuring that small genomic changes result in small changes in colour. It does not, however, guarantee that the same colour will in all cases represent the same genome (as many very different genomes may possess the same bit-counts in each of the three 11/10 bit segments).

.. figure:: _static/populationSceneNonCodingGenome.png
    :align: center

Gene Frequencies
----------------

For each grid-square, the mean value of the first three coding bits is computed, and a composite colour created using the mean of the least significant bit as red, the mean of the next least significant bit as green, and the mean of the third least significant bit as blue.

.. figure:: _static/populationSceneGenes.png
    :align: center

Settles
-------

Each grid-square is visualized with a grey-level representing the number of successful ‘settling’ events in that square since the last visualization.

.. figure:: _static/populationSceneSettles.png
    :align: center

Breed/Settle Fails (R=Breed; G=Settle)
--------------------------------------

Each grid-square is assigned a colour representing the number of ‘fails’ since the last visualization. The number of ‘breed fails’ (attempts to breed that were aborted due to compatibility) provides the red level, and the number of ‘settle fails’ (attempts to settle that resulted in a fitness of 0 and hence the death of the settling creature) provides the green level. Fail visualizations are scaled non-linearly (using *v* = 100*f*0.8, where *f* = mean fails per iteration, and v is visualized intensity on a scale 0-255). Fail visualization maps the edge of species or subspecies ranges, as it highlights cells where gene-flow is restricted by any mechanism.

.. figure:: _static/populationSceneBreedFail.png
    :align: center

Species
-------

REvoSim simulations often produce discrete species, i.e. reproductively isolated gene-pools, and for many macroevolutionary studies the identification and tracking of the fate of species is a requirement. This visualisation option assigns a unique colour to each species, and colours each grid-square with the species in the lowest occupied slot of that square.

.. figure:: _static/populationSceneSpecies.png
    :align: center
