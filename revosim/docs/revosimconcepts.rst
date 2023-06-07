.. _revosimconcepts:

Concepts and Example Usage
==========================

Concepts
--------

There are a number of underlying concepts that are useful to consider when using REvoSim. Please bear in mind that the model itself is fully described in the publications listed on the :ref:`Introduction` page. Prior to publishing any work using the software, or for more details on the following, please do check these out. To see how the software fits into the wider field, the paper 
`Digital Evolution for Ecology Research: A Review <https://www.frontiersin.org/articles/10.3389/fevo.2021.750779/full>`_ provides a useful overview.

**Species concept**

REvoSim is an individual-based simulation, and thus during runs population(s) of digital organisms evolve under a set of rules. As they evolve, species can emerge. These are stem from the species concept employed by REvoSim which is based upon reproductive isolation, and is thus akin to the biological species concept. The implementation of this (the species algorithm) is described in the `2019 paper <https://doi.org/10.1111/pala.12420>`_ documenting the model, but involves a pairwaise comparison of all individuals within a simulation every polling iteration to identify reproductively isolated clusters, and then a tracking function between these iterations. The level of dissimilarity required for reproductive isolation to occur is under user control: the option Maximum difference to breed dictates how many bits different two genomes are before they are considered isolated by this algorithm. Lowering this number creates more specitation-prone simulations.

Within this framework, runs in REvoSim - that typically start with a single genome - also start with a single species, that will go on to dominate the simulation. As a simulation runs, however, the population evolves, and under most settings species start to appear when populations specialise to a point when none can interbreed with any other organism alive within the simulation. Sometimes these will be sympatric (i.e. they occur in a single block of colour) but often, as a simulation runs, species will evolve in, and then start to track, particular colours (~niches) in their environment.

**Time**

Time in a REvoSim simulation is measured in iterations. Every iteration, the algorithms that compise the model are completed once. In order to link this to real world time units, it is useful to consider the average time per generation.

Example Usage
-------------

To do.
