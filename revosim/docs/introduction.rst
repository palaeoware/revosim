Introduction
============

Overview
--------

REvoSim can be used to study a range of evolutionary processes. It is based on digital organisms (each is a binary string of a user-defined length), within an environment defined by the RGB values of a two-dimensional image. It is highly abstracted, and is designed for computational efficiency. For versatility there are a large number of user-defined variables: an overview is provided below. This assumes the software has already been installed (instructions can be found on the page :ref:`requirements`). We recommend reading the paper below for full discussion of REvoSim's approach, potential limitations, and its strengths:

Garwood, R.J., Spencer A.R.T. and Sutton, M.D., 2019. REvoSim: Organism-level simulation of macro- and microevolution. Palaeontology. `https://doi.org/10.1111/pala.12420 <https://doi.org/10.1111/pala.12420>`_

That paper is based on version 2.0.0, versions from 3.0.0 have a number of additional capabilities including, for example, multi-length genomes (in v2.0.0 these were 64 bits). These changes are documented in the paper:

Furness, E.N., Garwood, R.J. & Sutton, M.D. 2023. REvoSim v3: A fast evolutionary simulation tool with ecological processes. Journal of Open Source Software 8(89): 5284. doi: `10.21105/joss.05284 <10.21105/joss.05284>`

A utility program, EnviroGen, is available to generate environments for REvoSim and thus provide a high level of control over the nature of the environment used for simulations. This is documented separately.

In brief, controls for the simulation are found on the toolbar at the top of the main window. Hovering a mouse over each toolbar button (or any other areas for user input in REvoSim) will provide an overview of what it does. The buttons are as follows:

:Run: Launch a simulation
:Run for: Launch a simulation and then allow it to continue for a set number of iterations.
:Batch: Repeat Run for *n* times.
:Pause: Pause a simulation.
:Stop: Cancel a simulation.
:Reset: Reset the simulation and reseed with a random digital organism in the central pixel.
:Reseed: Launch a dialogue to allow the simulation to be reseeded with a known genome, with two individuals that share a (random or user defined) genome, or with a large number of organisms with fixed genome with known properties (see :ref:`settingsinteractions`).
:Genome: Launch Genome Comparison Dock which allows genomes to be inspected and compared.
:Settings: Launch Settings Dock which allows variables to be defined.
:Logging: Output options are included in a separate dock, launched by clicking this button.
:Tests: Switch REvoSim to test mode and run software tests.
:About: Launch dialogue with information about REvoSim.

The main part of the window comprises two panels:

:Population view: Provides an overview of the population alive at any given polling iteration. The information shown can be selected with a drop down menu at the top.
:Environment: Shows the RGB environment which is used to calculate organism fitness, or - if present - image stacks being used to control the underlying variables.

Below this is the :ref:`information`, which shows a number of statistics for the given run, updated each polling iteration. These include population size, number of species, iterations and speed. You can find more information on the :ref:`windowlayout` pages.

Simulation setup
----------------

Variables can be defined within the settings dock on the right. Full descriptions of these and their implications can be found in the REvoSim paper. Clicking settings on the toolbar at the top of a window toggles the visibility of this dock. At the bottom of the dock are three tabs, each of which has variables associated with different aspects of the simulation.

:Organism tab: This includes the variables which dictate the behaviour of the digital organisms in a REvoSim run. This includes chance of mutation, starting age (i.e. length of life), breed threshold and cost, mode of breeding, and breed settings. More information: :ref:`organisms`
:Simulation tab: This includes the settings for the environment and associated files, simulation size, fitness target (i.e. the nature of the fitness lanscape), energy input, settle tolerance, and species tracking. More information: :ref:`simulations`
:Interactions tab: Here you can find the settings for interactions between individuals, and also settings for the predator system in REvoSim.  More information: :ref:`interactions`

Output setup
------------

The output can be configured within the logging dock on the left, which has two tabs at the bottom of the dock.

:Output tab: This includes output options for the simulation: save directory, refresh rate, and logging/output options. More information: :ref:`outputs`
:Running log: From v3.0.0 REvoSim includes a highly customisable running log, the contents of which can be defined using this tab. More information: :ref:`outputs`

Quick start
-----------

A simulation - using default settings and environment - can be started by hitting the Run button. In addition to the visualisation, runs can be analysed using log files which are placed by default in a folder called *REvoSim_output* on the desktop for all operating systems. A log is written during a run when "Write Log Files" (Logging dock, Output tab) is checked, and the phylogenetic tree and other more detailed statistics for a run can be written at any point by clicking the button "Write data (including tree) for current run".
