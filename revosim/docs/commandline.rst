.. _commandline:

Command Line Options
====================

REvoSim provides command line options to enable full control of simulations from command line or batch file environments. Every parameter configurable in the GUI can be set from the command line, and the program can be instructed to execute a simulation and then exit.

This document does not provide instructions on how to write a batch file, or determine correct paths (which will differ between operating systems).

Command line switches are documented below, and examples are given of their usage. Any combination of switches in any order is allowed. Note that on some systems, path names with spaces may cause problems: these are best avoided, but alternatively paths can be encased in double quotes if they contain a space.

Note in particular the --auto command, which automatically starts a simulation, and exits the program at its end. This will be required in most batch scenarios, as will the -k switch to turn on one or both types of logging.

e.g. a command line to run a similation on the environment files in "c:\\revosim\\data\\env1" for 50000 iterations, using normal (running) logs, and with settings at defaults except for 'toroidal' (on):

c:\\revosim\\revosim.exe -e c:\\revosim\\data\\env1 -t=On -k=Normal \--auto=50000

On Linux this command (assuming you are in the same folder as the REvoSim binary):

./revosim -e /home/user/environment/ -t=On -k=Normal \--auto=50000

The easiest way of modifying multiple variables for runs - and of setting custom log text, which was added in REvoSim v3.0.0 - is to use the software to output a settings files, and then to load this from the command line (\--settings).

Running via SSH
---------------

In many instances it may be beneficial to run REvoSim on a remote machine. With remote desktopping solutions that provide a GUI, this can be achieved very easily following the normal operating procedures. However, due to the development of REvoSim, launching the software via the command line launches the GUI. This means that it is not possible to launch the command line version via SSH unless X-forwarding or another such solution is implemented. However, if no interaction with the GUI is required, there are a number of solutions that can bypass this requirement. The simplest, that will work on the majority of systems with Qt installed, is to include the flag -platform offscreen:

./revosim \--auto 1000 -platform offscreen

This co-opts a Qt platform plugin to simply render to an offscreen buffer. Another option is to install XVFB - this is an "X server that can run on machines with no display hardware and no physical input device" (`More information <https://www.x.org/releases/X11R7.6/doc/man/man1/Xvfb.1.xhtml>`_). Once installed, you can launch revosim as follows:

xvfb-run -a  ./revosim \--auto 1000 

In any case, once a run is complete, you can then use e.g. SFTP to collect output files as required. 

Single-letter switches
----------------------

These can be used as either in single letter form (-a) or long form (\--startage - note the double -). All require a value, which can be separated from the switch using a space or an equals - or in single character mode, without a separator at all (this doesn't work in -- long form, where a separator IS required). Where no parameter is noted in square brackets this is a boolean option, which is either On or Off (you can also use 1/0, true/false, yes/no, y/n - which are not case sensitive). e.g.

- -a30
- -a=30
- -a 30
- \--startage=30
- \--startage 30

For a boolean:

- -q On
- -q 1
- -q=truE
- -q Y
- \--recalcfitness True
- \--recalcfitness = Y

Full list of single-letter switches (also available using switch -h, help):

- -a, --startage <age (integer)>                          Starting age for organisms.
- -b, --breedthreshold <threshold (integer)>              Breed threshold.
- -c, --breedcost <cost (integer)>                        Breed cost.
- -d, --maxdifftobreed <maxdifftobreed (integer)>         Maximum difference to breed.
- -e, --environment <directory>                           Directory containing environment images.
- -f, --usemaxdifftobreed <On/Off>                        Use maximum difference to breed criterion
- -g, --breedwithinspecies <On/Off>                       Only allow breeding within a species
- -i, --disperal <distance (integer)>                     maximum dispersal distance.
- -j, --outputpath <path>                                 path for output logs.
- -k, --logtype <Phylogeny/Normal/Both>                   logs to generate (phylogeny is end run log, normal is running log).
- -l, --excludenodescendents <On/Off>                     Exclude species without descendents from phylogeny logs
- -m, --environmentmode <mode (Static|Once|Loop|Bounce)>  Environment file cycling mode.
- -n, --energy <energy (integer)>                         Energy input.
- -o, --tolerance <tolerance (integer)>                   Settle tolerance.
- -p, --phylogeny <Off|Basic|Phylogeny|Metrics>           Phylogeny logging mode.
- -q, --recalcfitness <On/Off>                            recalculate fitness each iteration.
- -r, --refreshrate <rate (integer)>                      environment refresh rate.
- -s, --slots <slots (integer)>                           Slots per pixel.
- -t, --toroidal <On/Off>                                 Toroidal environment
- -u, --mutation <chance (integer)>                       Chance of mutation (0-255).
- -v, --csv <On/Off>                                      Use CSV format for normal log.
- -w, --interpolate <On/Off>                              Interpolate environmental images
- -x, --gridx <size (integer)>                            Grid (image) size, x.
- -y, --gridy <size (integer)>                            Grid (image) size, y.
- -z, --genomesize <size (integer)>                       Number of words in genome.
  
Long option only switches
-------------------------
We ran out of letters! These require the long format, with \--. Otherwise they work as above.

- --polling <rate [integer]>                              Set polling rate for logging and screen refresh.
- --auto <iterations [integer]>                           Automatically start simulation and exit program after completion of specified number of iterations
- --nonspatial <On/Off>                                   Use non-spatial simulation mode.
- --minspeciessize <size [integer]>                       Minimum species size to appear in logs
- --fitnesstarget <target [integer]>                      Fitness Target
- --breed <Obligate/Facultative/Variable/Asexual>         Breeding mode
- --variablemutate <On/Off>                               Variable mutation rates
- --nogui <On/Off>                                        Don't update GUI
- --pathogens <On/Off>                                    Turn pathogens on or off
- --pathogenmutate <chance (integer)>                     Chance of mutation (0-255).
- --pathogenfrequency <frequency (integer)>               Frequency pathogens are applied.
- --customlogging <On/Off>                                Record all custom logs.
- --disparityLogging <On/Off>                             Record disparity log.
- --interactblocks <On/Off>                               Turn block interactions on/off.
- --multibreedlist <On/Off>                               Turn multiple breed lists on/off.
- --interactrate <frequency (integer)>                    Frequency at which interactions occur.
- --pathogenevolve <On/Off>                               Set pathogens to evolve (on or off). If this is not set, the default is drift.
- --minpredatorscore <threshold (integer)>                Minimum predator score required for direct energy theft.
- --predationefficiency <integer>                         Trophic efficiency of direct energy theft predation.
- --interactXOR <On/Off>                                  Turns XOR interactions mechanism on/off.
- --log, --logFile <file>                                 XML File containing the log outputs.
- --v2log <On/Off>                                        Initiates v2.0.0 logging style.
- --interactfitness <On/Off>                              Interactions modify fitness.
- --interactenergy <On/Off>                               Interactions modify energy.
- --li_population <On/Off>                                Log images for population
- --li_fitness <On/Off>                                   Log images for mean fitness
- --li_sys_visualisation <On/Off>                         Log images for visualisation system 1
- --li_sys_visualisation2 <On/Off>                        Log images for visualisation system 2
- --li_species <On/Off>                                   Log images for species
- --li_settles <On/Off>                                   Log images for settles
- --li_fails <On/Off>                                     Log images for breed/settle fails
- --li_environment <On/Off>                               Log images for environenment
- --sys_fitness <Word string>                             Fitness system
- --sys_breed <Word string>                               Breed system
- --sys_mutate <Word string>                              Mutate system
- --sys_var_mutate <Word string>                          Variable mutate system
- --sys_var_breed <Word string>                           Variable breed system
- --sys_pathogens <Word string>                           Pathogens system
- --sys_species_ID <Word string>                          Species ID system
- --sys_interactions <Word string>                        Interactions system
- --sys_visualisation <Word string>                       Visualisation system
- --sys_visualisation2 <Word string>                      visualisation2 system
- --settings <file>                                       Load a REvoSim settings file.
- --L1_variable <Energy/No_selection/Mutation_rate>       Variable to be linked (required).
- --L1_imageSequence <directory>                          Directory containing linkage mask images (required).
- --L1_mode <mode (Static|Once|Loop|Bounce)>              Image file cycling mode (defaults to static).
- --L1_interpolate <On/Off>                               Image interpolation (defaults to true).
- --L1_change_rate <rate (integer)>                       Image refresh rate (defaults to 100).
- --L2_variable <Energy/No_selection/Mutation_rate>       Second variable to be linked (required).
- --L2_imageSequence <directory>                          Directory containing second linkage mask images (required).
- --L2_mode <mode (Static|Once|Loop|Bounce)>              Image file cycling mode (defaults to static).
- --L2_interpolate <On/Off>                               Image interpolation (defaults to true).
- --L2_change_rate <rate (integer)>                       Image refresh rate