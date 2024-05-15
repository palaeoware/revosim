.. _logging:

Running Log
===========

In addition to the Run end log and image logging (see :ref:`outputs`), plus the custom logs (see :ref:`mainmenu`), REvoSim has a versatile system to write customised logs at every polling iteration during a run. When enabled, this writes a file called REvoSim_log.txt to the output folder, then updates this by appending more data every polling iteration, giving an overview of the current run. The v3.0.0 logging systems functions by outputting text that has been entered in this dock (including line breaks). When doing so, it replaces key words/phrases surrounded by \*stars\* with values that document the state of the simulation. The three text areas/boxes in this dock allow different kinds of text for logs to be entered, as outlined below.

Buttons
-------

:Write to file: When this option is checked a log file is written during the course of every run.

:Instructions: This button creates a pop up window with simplified instructions for writing the running log. 

:Validate logs: This button will highlight, in red, any star-bounded \*key words\* that are not recognised by the logging system. 

:Command line log file: This option outputs an XML file for the current log settings that can be loaded from the command line for batch runs. 

:v2.0.0 log: This populates the text areas with the default running log text from v2.0.0.

:v2.0.0 CSV log: This populates the text areas with the default running log text from v2.0.0, formatted as a CSV.

Header text
-----------

This text area can be used for text that should appear at the start of a log file, but is only written when the file is created. This could include the settings for any given run, comments relevant to the experiment, or a header for the remaining columns, for example.

Iteration text
--------------

This text is written to the log file every polling iteration. This is likely to include statistics about the grid as a whole, for example, number of living organisms, iteration number, or mean fitness. A full list of options is provided below. 

:Write header from iteration log: This writes a header in the header text area based on the iteration text. Unrecognised keywords will be coloured in red.

Species text
------------

This text is written to the log file for each species, for every polling iteration. This data might include species ID, its origin time, the ID of its parents, and the number of organisms in that species.  A full list of options is provided below. 

:Write header from species log: This writes a header in the header text area based on the species text. Unrecognised keywords will be coloured in red.

v3.0.0 log options
------------------

In v3.0.0 and later, custom logs can be created by entering outputs in the text areas as described above, including \*keywords\*. These keywords are replaced with outputs for further analysis when the log is written (e.g. \*iteration\* is replaced with the current iteration number). 

Keywords are show below, first per iteration keywords, then per species keywords (although some can be used in either context). The escape sequence for a star (\*) is two stars (\*\*).

Iteration keywords
~~~~~~~~~~~~~~~~~~

:\*dumpGenomes\*: This writes the genomes of every living digital organism, and the X then Y coordinate in which they are found, separated by commas.
:\*gridBreedEntries\*: The number of organisms in the grid which are attempting to breed at the polling iteration.
:\*gridBreedFails\*: The number of failed breeding attempts in the polling iteration for all organisms in the grid.
:\*gridBreedSuccess\*: The number of successful breeds in the polling iteration for all organisms in the grid.
:\*gridMeanFitness\*: The mean fitness of all the organisms in the grid at polling iteration.
:\*gridNumberAlive\*: The number of organisms alive at polling iteration.
:\*gridGeneration\*: This calculates the average age (thus generation time) of all successful parents in the polling iteration. Note that it calculates this based on the assumption (which is true for the majority of settings), that organisms will only successfully breed once in their lifetime. This assumption can be checked using the \*gridNumberAlive\* and \*gridBreedSuccess\* outputs to calculate the mean number of breeding individuals at any generation.
:\*gridTrophicHistograms\*: This outputs a histogram of the trophic levels of all the organisms in the grid, at 0.1 intervals between 0 and 3.
:\*gridSpeciesRange\*: This reports the average range of the species alive at the polling iteration: it is a mean of the number of pixels each species is found in, across all species.
:\*iteration\*: The current iteration.
:\*printSettings\*: This prints a string of all REvoSim settings at the polling iteration, or start of the run if placed in the header.
:\*printTime\*: A string showing the time.
:\*speciesCount\*: The number of species alive at polling iteration.

Species keywords
~~~~~~~~~~~~~~~~

:\*Ca\*: This is the sum of the gene frequency differences from the origin of a species for the word(s) included in the fitness algorithm.
:\*completeSpeciesData\*: This writes the complete species data for any given species, of the form described for the end run log (see :ref:`outputs`).
:\*Cr\*: This is the sum of the gene frequency differences from the last polling iteration for the word(s) included in the fitness algorithm.
:\*currentGeneFrequencies\*: This writes the mean number of on bits for every position of the genome across a species.
:\*originTime\*: This is the polling iteration at which the species was first identified as reproductively isolated (see 2019 paper for a description of the species algorithm).
:\*originalGeneFrequencies\*: This writes the mean number of on bits for every position of the genome across a species at its origination.
:\*NCa\*:  This is the sum of the gene frequency differences from the origin of a species for the word(s) *not* included in the fitness algorithm.
:\*NCr\*: This is the sum of the gene frequency differences from the last polling iteration for the word(s) *not* included in the fitness algorithm.
:\*speciesGenomeDiversity\*: This is the number of distinct genomes included within a species.
:\*speciesDiversityPerWord\*: This is the number of distinct genomes for each word, considered independently.
:\*speciesID\*: REvoSim's ID for a species, which is useful for correlating statistics with the tree output by the software.
:\*speciesMeanEnvironmentalFitness\*: The mean fitness of the species from the environmental fitness algorithm.
:\*speciesMeanFitness\*: The mean fitness of the species from the environmental fitness algorithm plus any interactions that impact on fitness.
:\*speciesMeanRunningEnergy\*: This is the mean total lifetime energy of the organisms within a species (see :ref:`settingsinteractions`).
:\*speciesMeanRunningStolenEnergy\*: This is the mean total lifetime stolen energy of the organisms within a species (see :ref:`settingsinteractions`).
:\*speciesModalGenome\*: This outputs the modal genome of the species.
:\*speciesParent\*: This outputs the REvoSim species ID for the parent species.
:\*speciesSize\*: The number of individuals within the species.
:\*speciesTrophicLevel\*: The mean trophic level of the organisms in a species.
:\*speciesPerCellDiversity\*: Outputs eight columns of data on diversity of a species within each cell. For each cell, a mean pairwise difference is calculcated between all members of the species. The log system outputs (a) the mean of these means, over all cells occupied by the species, and (b) the 5th, 10th, 25th, 50th (median), 75th, 90th and 95th percentiles of the population of these means. 
v2.0.0 log
----------

The v2.0.0 log is structured as follows: 

:Timestamp: The first line is a time stamp highlighting when the run was written, in the following format: 2018-12-30T11:57:51
:Settings: A printout of all REvoSim settings for this run then follows, divided into integers and then bools. This means that at any point it is possible to revisit and check all settings for that run.
:Legend: There is then an explanation of the structure of the log files. Every iteration, the log records data about the simulation to file in a format designed to be easy to parse into a range of analytical environments (e.g. R, Python). This structure is as follows for each iteration:

::

  - [I] Iteration Number
  - [P] Population Grid Data:
    - Number of living digital organisms
    - Mean fitness of living digital organisms
    - Number of entries on the breed list
    - Number of failed breed attempts
    - Number of species
    - Trophic histograms
  - [S] Species Data:
    - Species ID
    - Species origin (iterations)
    - Species parent
    - Species current size (number of individuals)
    - Species mean Environmental Fitness
    - Species current genome (the modal genome of the species, the genome that occurs most frequently)
    - Species trophic level (the mean trophic level of individuals in the species)
    - Species genome diversity
  
:Log data: The log then begins. Iterations are separated by new line breaks. Every iteration has a single [I] line, one [P] line, and then an [S] line for every species above the minimum species size. We note that it does not exclude species without descendents because it is written during the simulation, appending to the file for speed. To filter out those species without descendents would introduce the need to store and then regularly filter the log data, and thus would come with a notable computational overhead.

:CSV format: If the 'Log file formatted as CSV' option is checked in output settings, the log file has a different and simpler format using the 'comma separated value' system. This may be easier to parse in some software (e.g. spreadsheets). A single header row is generated at the start of the file, providing titles for columns. Subsequent rows are generated for each [S] record described above, but these also include columns with the [I] and [P] records in each row. All fields described above are included in the output.

This logging system is designed to allow as many potential elements of a RevoSim run to be quantified as possible. Should any further measures or statistics be required, please file a `feature request <https://github.com/palaeoware/revosim/issues>`_.
