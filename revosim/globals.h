/**
 * @file
 * Header: Version
 *
 * All REvoSim code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All REvoSim code is Copyright 2008-2019 by Mark D. Sutton, Russell J. Garwood,
 * and Alan R.T. Spencer.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version. This program is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY.
 */

#ifndef GLOBALS_H
#define GLOBALS_H

//Save File Version
#define FILEVERSION 3

//Legal Stuff
#define COPYRIGHT "Copyright © 2008-2023 Mark D. Sutton, Russell J. Garwood, Euan N. Furness, Alan R.T.Spencer"
#define LICENCE "This program comes with ABSOLUTELY NO WARRANTY. This is free software, and you are welcome to redistribute it under the conditions of the GPL v3 license"

//Programme Name
#define PRODUCTNAME "REvoSim"
#define PRODUCTTAG "The [R]apid [Evo]lutionary [Sim]ulator"

//Email
#define EMAIL "palaeoware@gmail.com"

//Github
#define GITURL "https://github.com/"
#define GITREPOSITORY "palaeoware/revosim"
#define GITISSUE "/issues"

//Readthedocs
#define READTHEDOCS "https://revosim.readthedocs.io/en/latest/"

//Some defines
#define GRID_X 200
#define GRID_Y 200
#define SLOTS_PER_GRID_SQUARE 200
#define MAX_GENOME_WORDS 32

#define SPECIES_MODE_NONE 0
#define SPECIES_MODE_BASIC 1
#define SPECIES_MODE_PHYLOGENY 2
#define SPECIES_MODE_PHYLOGENY_AND_METRICS 3

#define ENV_MODE_STATIC 0
#define ENV_MODE_ONCE 1
#define ENV_MODE_LOOP 2
#define ENV_MODE_BOUNCE 3

#define PATH_MODE_DRIFT 0
#define PATH_MODE_EVOLVE 1
#define PATH_DEPTH 5

#define TROPHIC_SEED_DEFAULT 0
#define TROPHIC_SEED_3TIER 1
#define TROPHIC_SEED_5TIER 2

#define LOG_FITNESS 0
#define LOG_RECOMBINATION 1
#define LOG_MUTATION 2
#define LOG_SPECIATION 3
#define LOG_DISPARITY 4
#define LOG_CUSTOM 5
#define LOG_ITERATION 6
#define LOG_SPECIES 7
#define LOG_HEADER 8
#define LOG_DUMP_INDIVIDUALS 9

#define RESEED_MODE_SINGLE_RANDOM 0
#define RESEED_MODE_SINGLE_KNOWN 1
#define RESEED_MODE_DUAL_RANDOM 2
#define RESEED_MODE_DUAL_IDENTICAL 3
#define RESEED_MODE_DUAL_KNOWN 4

//RJG - Add new linkages here - this is a define in order to be able to provide text to the command line parser (needed before simulation manager is created)
#define LINKAGES_LIST "Energy", "No_selection", "Mutation_rate"

#endif // GLOBALS_H
