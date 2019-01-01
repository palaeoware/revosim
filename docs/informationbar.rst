.. _information:

Information Bar
===============

At the bottom of the GUI - below the population and environment views - is an information bar, which provides an overview of a number of elements of the current simulation. This is updated every polling generation. The statistics it provides are:

:Batch: When RevoSIm is running in batch mode this shows how many runs are completed out of the total number requested.
:Iteration: The number of iterations that had been completed since the start of the simulation at the last polling interval.
:Iterations per hour: This provides an indication of the speed at which RevoSim is running, and thus it is realtively easy to calculate  how long any given run will take.
:Organisms: This is a count of the total number of digital organisms alive at the last polling iteration.
:Milliseconds per iteration: This is an alternative measure of speed.
:Mean fitness: This is the mean fitness of all living organisms in the simulation at the last polling iteration.
:Species: If species tracking is on, this will provide the number of species at last poll once a speciation event has occurred.
:Environment: This is the index of the current environmental image, along with the total number that have been loaded. By default RevoSim loads with a single environmental image.
