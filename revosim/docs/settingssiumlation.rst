.. _simulations:

Setting up the Simulation
==========================

REvoSim provides the user with control of many elements of the simulation. These are introduced herein, and can be modified within the the Simulation tab of the Settings dock.

:Change environmental files: REvoSim launches with a default environment comprising three stripes in different shades of blue. This can be changed by clicking on this button, which will launch a file explorer. Using this allows a single, or multiple environment images (any standard image format) to be loaded. Environment image files are currently limited to 100 x 100 pixels in size.

:Environment refresh rate: This dictates how many iterations there are between updates of the environmental images if an image stack has been loaded, and thus dictates the environmental rate of change (small numbers are faster).

:Environment mode: This dictates, if multiple files are loaded, the mode with which these are updated. Static uses just the first of the chosen images as the environment throughout a run. Once will run from the start of an image stack to the end, leaving the last image as the environment once this has been loaded. Loop returns to the first environment image once the last has been reached (which may result in a significant environmental change in the environment if the first and last image in a stack are different). Bounce will move from the start to end of an environmental image stack and then back again, repeating this for the duration of a run.

:Interpolate between images: This provide linear interpolation of the environment between refresh iterations.

:Toroidal environment: By default, when a juvenile settles outside the limits of an environment, it dies. Such boundary effects can be avoided by selecting toroidal environment, which wraps around in both directions (e.g. juveniles settling off the left enter the simulation on the right; EnviroGen can simulate toroidal  environments).

:Grid X: This dictates the size of the simulation grid in the X direction. If this is smaller than the environmental image it selects the left side of the image.

:Grid Y: This dictates the size of the simulation grid in the Y direction. If that is smaller than the environmental image it selects the top of the image.

:Slots: Every pixel in the simulation grid can have multiple digital organisms - each lives within a slot (this is equivalent to a Z axis). This dictates the number of slots (how these behave are treated during reproduction is outlined in the REvoSim paper).

:Fitness target: This is the target value used by the fitness algorithm as described in the REvoSim paper. In brief, it is the value that the hamming distance between the gemome and the environmental masks (numbers dictated by the colour) is optomised towards. Changing it modifies the fitness landscape: a value of 48 has the greatest number of organisms with peak fitness, and either side of this value, fewer organisms are capable of peak fitness this is shown in the graph below. In this, black points represent the number of genomes with peak fitness for all fitness targets, the red star is the default value 66; in this particular set of runs, there there are ~3000 genomes capable of peak fitness with the default target, but this can vary given differing masks. Note that there are some values for which no organisms will be capable of survival: if this occurs, REvoSim will provide a warning. Also note that the graph below can be created and distribution of fitnesses quantified using the count peaks option in the commands menu of REvoSim.

.. figure:: _static/targetFitnessSettleTolerance15.png
    :align: center

:Energy input: This is the amount of energy provided per pixel of the environment grid, which is then split between the digital organisms living in that pixel on the basis of their fitness. This is then used as part of the breeding system in REvoSIm.

:Settle tolerance: ​ This it controls the distance from the fitness target at which organisms are no longer viable (i.e. have zero fitness and die). It also provides the maximum attainable fitness within the simulation.

:Recalculate fitness: For efficiency REvoSim only calculates organism fitness for any individual on initial settling: subsequent changes in the environment will not modify an individual's fitness. For some settings (e.g. rapidly changing enviironments or long-lived organisms) this may not be desirable. When checked, this option recalculates the fitness for every organiusm in the simulation every iteration, overcoming this limitation but also significantly slowing the simulation.

:Phylogeny settings: These radio buttons dictate the mode which by REvoSim tracks phylogeny. Off does not track phylogenies and is thus the fastest mode (this could be useful for - as an example - studies focussing on changes in fitness). Basic phylogeny identifies species in time slices to allow species to be coloured in the population view, and species diversity to be recorded. The option phylogeny identifies species, and then records their phylogeny, allowing a tree to be created at the end of a run. Phylogeny and metrics does this, and also records a number of other metrics for each species, also output (when requested) at the end of a run. Note that moving between off and any form of tracking has a significant performance cost: there is little computational overhead moving between the different tracking options. Moving from basic to phylogeny to metrics does, however, come with an increasing memory overhead, as the trees and metrics are by necessity stored in RAM during a run, and written when the run completes. This could have implications for runs with a significant number of organisms run for extended periods. See :ref:`logging` and :ref:`outputs` for more details REvoSim outputs.
