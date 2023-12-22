.. _simulations:

Setting up the Simulation
=========================

REvoSim provides the user with control of many elements of the simulation. These are introduced herein, and can be modified within the the Simulation tab of the Settings dock, or from the command line.

Environment settings
--------------------

:Change environmental files: REvoSim launches with a default environment comprising three stripes in different shades of blue. This can be changed by clicking on this button, which will launch a file explorer. Using this allows a single, or multiple environment images (any standard image format) to be loaded. Environment image files are currently limited to 100 x 100 pixels in size.

:Environment refresh rate: This dictates how many iterations there are between updates of the environmental images if an image stack has been loaded, and thus dictates the environmental rate of change (small numbers are faster).

:Environment mode: This dictates, if multiple files are loaded, the mode with which these are updated. Static uses just the first of the chosen images as the environment throughout a run. Once will run from the start of an image stack to the end, leaving the last image as the environment once this has been loaded. Loop returns to the first environment image once the last has been reached (which may result in a significant environmental change in the environment if the first and last image in a stack are different). Bounce will move from the start to end of an environmental image stack and then back again, repeating this for the duration of a run.

:Interpolate between images: This provides linear interpolation of the environment between refresh iterations, preventing large stepped changes in organism fitness.

:Toroidal environment: By default, when a juvenile settles outside the limits of an environment, it dies. Such boundary effects can be avoided by selecting toroidal environment, which wraps around in both directions (e.g. juveniles settling off the left enter the simulation on the right; EnviroGen can simulate toroidal  environments).

Simulation size
---------------

:Grid X: This dictates the size of the simulation grid in the X direction. If this is smaller than the environmental image it selects the left side of the image.

:Grid Y: This dictates the size of the simulation grid in the Y direction. If that is smaller than the environmental image it selects the top of the image.

:Slots: Every pixel in the simulation grid can have multiple digital organisms - each lives within a slot (this is equivalent to a Z axis). This dictates the number of slots (how these are filled during reproduction is outlined in the 2019 REvoSim paper).

Simulation settings
-------------------

:Fitness target: This is the target value used by the fitness algorithm as described in the REvoSim paper. In brief, it is the value that the hamming distance between the gemome words in the fitness system and the environmental masks (numbers dictated by the colour) is optimised towards. Changing it modifies the fitness landscape: a value of 48 - with one genome word - has the greatest number of genomes with peak fitness, and either side of this value, fewer genomes display peak fitness. This is shown in the graph below, where black points represent the number of genomes with peak fitness for all fitness targets. The red star is the default value for one word, 66; in this particular set of runs, there there are ~3000 genomes capable of peak fitness with the default target, but this can vary given differing masks. Note that there are some values for which no organisms will be capable of survival: if this occurs, REvoSim will provide a warning. Also note that the graph below can be created and distribution of fitnesses quantified using the count peaks option in the commands menu of REvoSim. When changing the number of genome words used by the fitness system from the organism tab, REvoSim will provide the option of updating the fitness target to a commensurate value for the new number of words. If you would like to further discuss the nature of fitness landscapes within REvoSim please contact the authors, as this is something we have thought about a lot. 


.. figure:: _static/targetFitnessSettleTolerance15.png
    :align: center


:Energy input: This is the amount of energy provided per pixel of the environment grid, which is then split between the digital organisms living in that pixel on the basis of their fitness. This is then used as part of the breeding system in REvoSim.

:Settle tolerance: This it controls the distance from the fitness target at which organisms are no longer viable (i.e. have zero fitness and, absent energy-based interactions, die). It also provides the maximum attainable fitness within the simulation.

:Recalculate fitness: For efficiency REvoSim only calculates organism fitness for any individual on initial settling: subsequent changes in the environment will not modify an individual's fitness. For some settings (e.g. rapidly changing environments or long-lived organisms) this may not be desirable. When checked, this option recalculates the fitness for every organism in the simulation every iteration, overcoming this limitation but also significantly slowing the simulation.

:No selection: This option turns off the fitness algorithm so energy is split equally between organisms in any cell, irrespective of their fitness.

Phylogeny settings
------------------

These radio buttons dictate the mode which by REvoSim tracks phylogeny.

:Off: Does not track phylogenies and is thus the fastest mode (this could be useful for - as an example - studies focussing on changes in fitness).
:Basic phylogeny: Identifies species in time slices to allow species to be coloured in the population view, and species diversity to be recorded.
:Phylogeny: Identifies species, and then records their phylogeny, allowing a tree to be created at the end of a run.
:Phylogeny and metrics: Does the same as *Phylogeny*, and also records a number of other metrics for each species, also output (when requested) at the end of a run.

*Note*: Moving between off and any form of tracking has a significant performance cost: there is little computational overhead moving between the different tracking options. Moving from basic to phylogeny to metrics does, however, come with an increasing memory overhead, as the trees and metrics are by necessity stored in RAM during a run, and written when the run completes. This could have implications for runs with a significant number of organisms and large numbers of iterations. See :ref:`logging` and :ref:`outputs` for more details REvoSim outputs.

Linkages
--------

This is a capability introduced in v3.0.0 that links REvoSim variables to image maps (or each other, a functionality that has yet to be added, but is easily implemented on request). As default, REvoSim offers limited linked variables, however the system is designed so more can be added with ease. Please contact the authors if you would like one to be added - we will happily do so.

As currently coded, the image stack selected is used to spatially control a variable: specifically, for integer variables, the value of the R channel is multiplied by the value of the selected variable as defined in the settings docker. So, for example, if energy is set to 5 overall, for a pixel with an R value of 0 in the image map the energy will be set to 0 for that iteration. If the R value is set to 255, the energy level for that pixel will be 1275 (5x225). *Note that this means that when using a linked integer variable it is likely that the default values will need to be changed.* For boolean variables, an R channel value of zero is off, all other zero values (1-255) is on.

Create new linkage
~~~~~~~~~~~~~~~~~~

To create a new linkage, use the push button labelled "Link a variable to an image mask". This creates a pop up window with the following options:

:Variable: This is the variable to which you would like to control using an image stack.
:Load Images: Clicking this image loads a dialogue allowing you to load an image stack that controls the variable defined above, as described in the introduction to linkages.
:Mode: This radio button defines the image mode for the stack controlling the variable. The modes are the same as used for the environment - see `Environment settings`_.
:Interpolate images: As with the environment settings, this defines whether REvoSim interpolates between images.
:Refresh rate: The update rate of the image stacks controlling variables does not need to be the same as e.g. the environmental refresh rate: this spin box defines how often (in iterations) the next image in the stack is loaded.

Edit linkage
~~~~~~~~~~~~

Once added, a linkage appears as a list item in the *Current linkages* text box on the bottom right of the simulation docker. To edit one double click on the linkage. Doing so will load a pre-filled dialogue with the same options as the add new linkage dialogue outlined above, all of which can be changed as required. The linkage can also be deleted using this dialogue box. 
