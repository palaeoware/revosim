.. _mainmenu:

Main Menu
=========

The Main Menu, located at the top of the program window, allows access to all program actions, functions, and settings. The menu is currently sub-divided into three sections:

1. :ref:`mainmenu-commands`
2. :ref:`mainmenu-tools`
3. :ref:`mainmenu-help`

.. _mainmenu-commands:

Commands
--------

.. figure:: _static/commandMenu.png
    :align: center

    The commands menu

The Commands menu holds the majority of available actions and program functions related to running of one or more simulations. The options are as follows:

:Load Random Numbers: Allows user-selected random numbers to be loaded through a custom file, if you so wish. See :ref:`customrandomnumbers`.
:Run .... Reseed with known: These options are provided as alternatives to the buttons on the top toolbar of the GUI.See :ref:`maintoolbar`.
:Go slow: This option slows the simulation to allow environmental changes and the visualisation in the population view to be viewed more clearly. It achieves this by adding a 30ms delay to every iteration.
:Save: This saves the current state of the REvoSim simulation, allowing it to be loaded later, including the masks, organisms, and all settings. This is saved as a binary file to allow the minimum file size possible.
:Load: This loads the above REvoSim file.
:Save settings: This saves the settings of REvoSim in a given state. This includes all user-defined variables, but nothing else. These are saved as a human-readable XML file.
:Load settings: Loads a settings file.
:Count peaks: This is provided to help the user understand the fitness landscape of their run (albeit in simple terms). See :ref:`countpeaks`.
:Exit: Quits REvoSim.

.. _mainmenu-tools:

Tools
-----

.. figure:: _static/toolsMenu.png
    :align: center


The Tools menu allows access to the built-in dockable widgets (called 'Docks') which alter or extend the core program functions. This includes the main simulation settings dock, described in :ref:`organisms` and :ref:`simulations`, the output dock, covered in :ref:`outputs`, and the genome comparison dock (:ref:`genomecomparison`).

The development version of the software also has a series of custom logs, generally written at the logging iteration, and placed in the same folder as the main log (many of these can now be achieved using the new logging tools in versions since REvoSim 3.0.0):

:Fitness logging: This is largely obsolete from v3.0.0 as the same functionality is possible with the custom log. It is kept here for backward compatibility.
:Recombination logging: This log outputs the proportion of asexual v.s. sexual breeds when variable breeding is enabled, for each iteration for the grid as a whole. It also outputs the total breed attempts and fails, and also number of living organisms.
:Variable mutation logging: This outputs the iteration number, and the number of ones in the part of the genome in the variable mutate system.
:Speciation logging: For every speciation event above minimum species size, this outputs iteration number, number of species in event, shared cells, and then for each species the ID, number of individuals, number of cells occupied total, and number of cells in which species is sole occupant. Requires phylogeny mode to be set to *Phylogeny and metrics*.
:Disparity logging: For each polling iteration this does a dump of every single genome, plus its X and Y coordinates.

All logs contain a copy of the run settings, and explanatory text.

.. _mainmenu-help:

Help
----

.. figure:: _static/helpMenu.png
    :align: center

The Help menu contains links to useful program information. The first link will allow you to report a bug or request a feature. The next links to the Palaeoware code repository, and the third to REvoSim documentation. The fourth will load an about dialogue.
