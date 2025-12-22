/**
 * @file
 * Main Programme
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

#include "darkstyletheme.h"
#include "mainwindow.h"
#include "globals.h"

#include <QApplication>
#include <QSplashScreen>
#include <QString>
#include <QStyle>
#include <QCommandLineParser>
#include <QHash>
#include <QTimer>

//check values for booleans, convert Yes,No,On,Off etc to 1,0
QString boolValue(QString v)
{
    v = v.toLower();
    if (v == "on") return "1";
    if (v == "off") return "0";
    if (v == "1") return "1";
    if (v == "0") return "0";
    if (v == "y") return "1";
    if (v == "n") return "0";
    if (v == "yes") return "1";
    if (v == "no") return "0";
    if (v == "true") return "1";
    if (v == "false") return "0";
    return "1"; //default is on if not recognised
}

//parse the command line and convert to QHash of options
QHash<QString, QString> *parse(QCoreApplication *app)
{
    QCommandLineParser *parser = new QCommandLineParser();
    parser->setApplicationDescription("REvoSim is an individual-based evolutionary model. You are using the command line option. See documentation or Garwood et al. (2019) Palaeontology for description of software.");
    parser->addHelpOption();
    parser->setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);

    //short-form options
    QCommandLineOption opt_a(QStringList() << "a" << "startage",
                             QCoreApplication::translate("main", "Starting age for organisms."),
                             QCoreApplication::translate("main", "age (integer)"));
    parser->addOption(opt_a);

    QCommandLineOption opt_b(QStringList() << "b" << "breedthreshold",
                             QCoreApplication::translate("main", "Breed threshold."),
                             QCoreApplication::translate("main", "threshold (integer)"));
    parser->addOption(opt_b);

    QCommandLineOption opt_c(QStringList() << "c" << "breedcost",
                             QCoreApplication::translate("main", "Breed cost."),
                             QCoreApplication::translate("main", "cost (integer)"));
    parser->addOption(opt_c);

    QCommandLineOption opt_d(QStringList() << "d" << "maxdifftobreed",
                             QCoreApplication::translate("main", "Maximum difference to breed."),
                             QCoreApplication::translate("main", "maxdifftobreed (integer)"));
    parser->addOption(opt_d);

    QCommandLineOption opt_e(QStringList() << "e" << "environment",
                             QCoreApplication::translate("main", "Directory containing environment images."),
                             QCoreApplication::translate("main", "directory"));
    parser->addOption(opt_e);

    QCommandLineOption opt_f(QStringList() << "f" << "usemaxdifftobreed",
                             QCoreApplication::translate("main", "Use maximum difference to breed criterion"),
                             QCoreApplication::translate("main", "On/Off"));
    parser->addOption(opt_f);

    QCommandLineOption opt_g(QStringList() << "g" << "breedwithinspecies",
                             QCoreApplication::translate("main", "Only allow breeding within a species"),
                             QCoreApplication::translate("main", "On/Off"));
    parser->addOption(opt_g);

    //RJG - H is help

    QCommandLineOption opt_i(QStringList() << "i" << "disperal",
                             QCoreApplication::translate("main", "maximum dispersal distance."),
                             QCoreApplication::translate("main", "distance (integer)"));
    parser->addOption(opt_i);

    QCommandLineOption opt_j(QStringList() << "j" << "outputpath",
                             QCoreApplication::translate("main", "path for output logs."),
                             QCoreApplication::translate("main", "path"));
    parser->addOption(opt_j);


    QCommandLineOption opt_k(QStringList() << "k" << "logtype",
                             QCoreApplication::translate("main", "logs to generate."),
                             QCoreApplication::translate("main", "Phylogeny/Normal/Both"));
    parser->addOption(opt_k);

    QCommandLineOption opt_l(QStringList() << "l" << "excludenodescendents",
                             QCoreApplication::translate("main", "Exclude species without descendents from phylogeny logs"),
                             QCoreApplication::translate("main", "On/Off"));
    parser->addOption(opt_l);

    QCommandLineOption opt_m(QStringList() << "m" << "environmentmode",
                             QCoreApplication::translate("main", "Environment file cycling mode."),
                             QCoreApplication::translate("main", "mode (Static|Once|Loop|Bounce)"));
    parser->addOption(opt_m);

    QCommandLineOption opt_n(QStringList() << "n" << "energy",
                             QCoreApplication::translate("main", "Energy input."),
                             QCoreApplication::translate("main", "energy (integer)"));
    parser->addOption(opt_n);


    QCommandLineOption opt_o(QStringList() << "o" << "tolerance",
                             QCoreApplication::translate("main", "Settle tolerance."),
                             QCoreApplication::translate("main", "Settle tolerance (integer)"));
    parser->addOption(opt_o);

    QCommandLineOption opt_p(QStringList() << "p" << "phylogeny",
                             QCoreApplication::translate("main", "Phylogeny logging mode."),
                             QCoreApplication::translate("main", "Off|Basic|Phylogeny|Metrics"));
    parser->addOption(opt_p);

    QCommandLineOption opt_q(QStringList() << "q" << "recalcfitness",
                             QCoreApplication::translate("main", "recalculate fitness each iteration."),
                             QCoreApplication::translate("main", "On/Off")
                            );
    parser->addOption(opt_q);

    QCommandLineOption opt_r(QStringList() << "r" << "refreshrate",
                             QCoreApplication::translate("main", "environment refresh rate."),
                             QCoreApplication::translate("main", "rate (integer)"));
    parser->addOption(opt_r);

    QCommandLineOption opt_s(QStringList() << "s" << "slots",
                             QCoreApplication::translate("main", "Slots per pixel."),
                             QCoreApplication::translate("main", "slots (integer)"));
    parser->addOption(opt_s);

    QCommandLineOption opt_t(QStringList() << "t" << "toroidal",
                             QCoreApplication::translate("main", "Toroidal environment"),
                             QCoreApplication::translate("main", "On/Off"));
    parser->addOption(opt_t);

    QCommandLineOption opt_u(QStringList() << "u" << "mutation",
                             QCoreApplication::translate("main", "Chance of mutation (0-255)."),
                             QCoreApplication::translate("main", "chance (integer)"));
    parser->addOption(opt_u);

    QCommandLineOption opt_v(QStringList() << "v" << "csv",
                             QCoreApplication::translate("main", "Use CSV format for normal log."),
                             QCoreApplication::translate("main", "On/Off"));
    parser->addOption(opt_v);

    QCommandLineOption opt_w(QStringList() << "w" << "interpolate",
                             QCoreApplication::translate("main", "Interpolate environmental images"),
                             QCoreApplication::translate("main", "On/Off"));
    parser->addOption(opt_w);

    QCommandLineOption opt_x(QStringList() << "x" << "gridx",
                             QCoreApplication::translate("main", "Grid (image) size, x."),
                             QCoreApplication::translate("main", "size (integer)"));
    parser->addOption(opt_x);

    QCommandLineOption opt_y(QStringList() << "y" << "gridy",
                             QCoreApplication::translate("main", "Grid (image) size, y."),
                             QCoreApplication::translate("main", "size (integer)"));
    parser->addOption(opt_y);

    QCommandLineOption opt_z(QStringList() << "z" << "genomesize",
                             QCoreApplication::translate("main", "Number of words in genome."),
                             QCoreApplication::translate("main", "size (integer)"));
    parser->addOption(opt_z);

    //long string only options

    QCommandLineOption opt_polling(QStringList() << "polling",
                                   QCoreApplication::translate("main", "Set polling rate for logging and screen refresh."),
                                   QCoreApplication::translate("main", "rate [integer]"));
    parser->addOption(opt_polling);

    QCommandLineOption opt_auto(QStringList() << "auto",
                                QCoreApplication::translate("main", "Automatically start simulation and exit program after completion of specified number of iterations"),
                                QCoreApplication::translate("main", "iterations [integer]"));
    parser->addOption(opt_auto);

    QCommandLineOption opt_speciesBurnIn(QStringList() << "speciesburnin",
                                         QCoreApplication::translate("main", "Set a species burn in - if using you should also set the duration."),
                                         QCoreApplication::translate("main", "On/Off"));
    parser->addOption(opt_speciesBurnIn);

    QCommandLineOption opt_speciesBurnInDuration(QStringList() << "speciesburninduration",
                                                 QCoreApplication::translate("main", "Sets the number of static noise environmental images you would like to prepend to your own image sequence"),
                                                 QCoreApplication::translate("main", "images [integer]"));
    parser->addOption(opt_speciesBurnInDuration);

    QCommandLineOption opt_nonspatial(QStringList() << "nonspatial",
                                      QCoreApplication::translate("main", "Use non-spatial simulation mode."),
                                      QCoreApplication::translate("main", "On/Off"));
    parser->addOption(opt_nonspatial);

    QCommandLineOption opt_minspeciessize(QStringList() << "minspeciessize",
                                          QCoreApplication::translate("main", "Minimum species size to appear in logs"),
                                          QCoreApplication::translate("main", "size [integer]"));
    parser->addOption(opt_minspeciessize);

    QCommandLineOption opt_fitnesstarget(QStringList() << "fitnesstarget",
                                         QCoreApplication::translate("main", "Fitness Target"),
                                         QCoreApplication::translate("main", "target [integer]"));
    parser->addOption(opt_fitnesstarget);

    QCommandLineOption opt_breed(QStringList() << "breed",
                                 QCoreApplication::translate("main", "Breeding mode"),
                                 QCoreApplication::translate("main", "Obligate/Facultative/Variable/Asexual"));
    parser->addOption(opt_breed);

    QCommandLineOption opt_variablemutate(QStringList() << "variablemutate",
                                          QCoreApplication::translate("main", "Variable mutation rates"),
                                          QCoreApplication::translate("main", "On/Off"));
    parser->addOption(opt_variablemutate);

    QCommandLineOption opt_noGUI(QStringList() << "nogui",
                                 QCoreApplication::translate("main", "Don't update GUI"),
                                 QCoreApplication::translate("main", "On/Off"));
    parser->addOption(opt_noGUI);

    QCommandLineOption opt_pathogens(QStringList() << "pathogens",
                                     QCoreApplication::translate("main", "Turn pathogens on or off"),
                                     QCoreApplication::translate("main", "On/Off"));
    parser->addOption(opt_pathogens);

    QCommandLineOption opt_pathogenMutate(QStringList() << "pathogenmutate",
                                          QCoreApplication::translate("main", "Chance of mutation (0-255)."),
                                          QCoreApplication::translate("main", "chance (integer)"));
    parser->addOption(opt_pathogenMutate);

    QCommandLineOption opt_pathogenFrequency(QStringList() << "pathogenfrequency",
                                             QCoreApplication::translate("main", "Frequency pathogens are applied."),
                                             QCoreApplication::translate("main", "frequency (integer)"));
    parser->addOption(opt_pathogenFrequency);

    QCommandLineOption opt_pathogenEvolve(QStringList() << "pathogenevolve",
                                          QCoreApplication::translate("main", "Set pathogens to evolve (on or off) - default is drift"),
                                          QCoreApplication::translate("main", "On/Off"));
    parser->addOption(opt_pathogenEvolve);

    QCommandLineOption opt_customLogging(QStringList() << "customlogging",
                                         QCoreApplication::translate("main", "Record all custom logs."),
                                         QCoreApplication::translate("main", "On/Off"));
    parser->addOption(opt_customLogging);

    QCommandLineOption opt_disparityLogging(QStringList() << "disparityLogging",
                                            QCoreApplication::translate("main", "Record disparity log."),
                                            QCoreApplication::translate("main", "On/Off"));
    parser->addOption(opt_disparityLogging);

    QCommandLineOption opt_interactblocks(QStringList() << "interactblocks",
                                          QCoreApplication::translate("main", "Turn block interactions on/off."),
                                          QCoreApplication::translate("main", "On/Off"));
    parser->addOption(opt_interactblocks);

    QCommandLineOption opt_multibreedlist(QStringList() << "multibreedlist",
                                          QCoreApplication::translate("main", "Turn multiple breed lists on/off."),
                                          QCoreApplication::translate("main", "On/Off"));
    parser->addOption(opt_multibreedlist);

    QCommandLineOption opt_interactrate(QStringList() << "interactrate",
                                        QCoreApplication::translate("main", "Frequency at which interactions occur."),
                                        QCoreApplication::translate("main", "frequency (integer)"));
    parser->addOption(opt_interactrate);

    QCommandLineOption opt_minpredatorscore(QStringList() << "minpredatorscore",
                                            QCoreApplication::translate("main", "Minimum predator score required for direct energy theft."),
                                            QCoreApplication::translate("main", "threshold (integer)"));
    parser->addOption(opt_minpredatorscore);

    QCommandLineOption opt_predationefficiency(QStringList() << "predationefficiency",
                                               QCoreApplication::translate("main", "Trophic efficiency of direct energy theft predation."),
                                               QCoreApplication::translate("main", "integer"));
    parser->addOption(opt_predationefficiency);

    QCommandLineOption opt_log(QStringList() << "log" << "logFile",
                               QCoreApplication::translate("main", "XML File containing the log outputs."),
                               QCoreApplication::translate("main", "file"));
    parser->addOption(opt_log);

    QCommandLineOption opt_v2log(QStringList() << "v2log",
                                 QCoreApplication::translate("main", "Initiates v2.0.0 logging style."),
                                 QCoreApplication::translate("main", "On/Off"));
    parser->addOption(opt_v2log);

    QCommandLineOption opt_interactfitness(QStringList() << "interactfitness",
                                           QCoreApplication::translate("main", "Interactions modify fitness."),
                                           QCoreApplication::translate("main", "On/Off"));
    parser->addOption(opt_interactfitness);

    QCommandLineOption opt_interactenergy(QStringList() << "interactenergy",
                                          QCoreApplication::translate("main", "Interactions modify energy."),
                                          QCoreApplication::translate("main", "On/Off"));
    parser->addOption(opt_interactenergy);

    QCommandLineOption opt_li_population(QStringList() << "li_population",
                                         QCoreApplication::translate("main", "Log images for population"),
                                         QCoreApplication::translate("main", "On/Off"));
    parser->addOption(opt_li_population);

    QCommandLineOption opt_li_fitness(QStringList() << "li_fitness",
                                      QCoreApplication::translate("main", "Log images for mean fitness"),
                                      QCoreApplication::translate("main", "On/Off"));
    parser->addOption(opt_li_fitness);

    QCommandLineOption opt_li_sys_visualisation(QStringList() << "li_sys_visualisation",
                                                QCoreApplication::translate("main", "Log images for visualisation system 1"),
                                                QCoreApplication::translate("main", "On/Off"));
    parser->addOption(opt_li_sys_visualisation);

    QCommandLineOption opt_li_sys_visualisation2(QStringList() << "li_sys_visualisation2",
                                                 QCoreApplication::translate("main", "Log images for visualisation system 2"),
                                                 QCoreApplication::translate("main", "On/Off"));
    parser->addOption(opt_li_sys_visualisation2);

    QCommandLineOption opt_li_species(QStringList() << "li_species",
                                      QCoreApplication::translate("main", "Log images for species"),
                                      QCoreApplication::translate("main", "On/Off"));
    parser->addOption(opt_li_species);

    QCommandLineOption opt_li_settles(QStringList() << "li_settles",
                                      QCoreApplication::translate("main", "Log images for settles"),
                                      QCoreApplication::translate("main", "On/Off"));
    parser->addOption(opt_li_settles);

    QCommandLineOption opt_li_fails(QStringList() << "li_fails",
                                    QCoreApplication::translate("main", "Log images for breed/settle fails"),
                                    QCoreApplication::translate("main", "On/Off"));
    parser->addOption(opt_li_fails);

    QCommandLineOption opt_li_environment(QStringList() << "li_environment",
                                          QCoreApplication::translate("main", "Log images for environenment"),
                                          QCoreApplication::translate("main", "On/Off"));
    parser->addOption(opt_li_environment);

    QCommandLineOption opt_sys_fitness(QStringList() << "sys_fitness",
                                       QCoreApplication::translate("main", "Fitness system"),
                                       QCoreApplication::translate("main", "Word string"));
    parser->addOption(opt_sys_fitness);

    QCommandLineOption opt_sys_breed(QStringList() << "sys_breed",
                                     QCoreApplication::translate("main", "Breed system"),
                                     QCoreApplication::translate("main", "Word string"));
    parser->addOption(opt_sys_breed);

    QCommandLineOption opt_sys_mutate(QStringList() << "sys_mutate",
                                      QCoreApplication::translate("main", "Mutate system"),
                                      QCoreApplication::translate("main", "Word string"));
    parser->addOption(opt_sys_mutate);

    QCommandLineOption opt_sys_var_mutate(QStringList() << "sys_var_mutate",
                                          QCoreApplication::translate("main", "Variable mutate system"),
                                          QCoreApplication::translate("main", "Word string"));
    parser->addOption(opt_sys_var_mutate);

    QCommandLineOption opt_sys_var_breed(QStringList() << "sys_var_breed",
                                         QCoreApplication::translate("main", "Variable breed system"),
                                         QCoreApplication::translate("main", "Word string"));
    parser->addOption(opt_sys_var_breed);

    QCommandLineOption opt_sys_pathogens(QStringList() << "sys_pathogens",
                                         QCoreApplication::translate("main", "Pathogens system"),
                                         QCoreApplication::translate("main", "Word string"));
    parser->addOption(opt_sys_pathogens);

    QCommandLineOption opt_sys_species_ID(QStringList() << "sys_species_ID",
                                          QCoreApplication::translate("main", "Species ID system"),
                                          QCoreApplication::translate("main", "Word string"));
    parser->addOption(opt_sys_species_ID);

    QCommandLineOption opt_sys_interactions(QStringList() << "sys_interactions",
                                            QCoreApplication::translate("main", "Interactions system"),
                                            QCoreApplication::translate("main", "Word string"));
    parser->addOption(opt_sys_interactions);

    QCommandLineOption opt_sys_visualisation(QStringList() << "sys_visualisation",
                                             QCoreApplication::translate("main", "Visualisation system"),
                                             QCoreApplication::translate("main", "Word string"));
    parser->addOption(opt_sys_visualisation);

    QCommandLineOption opt_sys_visualisation2(QStringList() << "sys_visualisation2",
                                              QCoreApplication::translate("main", "visualisation2 system"),
                                              QCoreApplication::translate("main", "Word string"));
    parser->addOption(opt_sys_visualisation2);

    QCommandLineOption opt_settings(QStringList() << "settings",
                                    QCoreApplication::translate("main", "Load a REvoSim settings file."),
                                    QCoreApplication::translate("main", "file"));
    parser->addOption(opt_settings);

    QCommandLineOption opt_maxthreads(QStringList() << "maxthreads",
                                      QCoreApplication::translate("main", "Specify maximum threads to use"),
                                      QCoreApplication::translate("main", "thread count (integer)"));
    parser->addOption(opt_maxthreads);

    QStringList linkagesList = {LINKAGES_LIST};
    QString linkages = linkagesList.join("/");
    QCommandLineOption opt_L1_variable(QStringList() << "L1_variable",
                                       QCoreApplication::translate("main", "Variable to be linked (required)."),
                                       QCoreApplication::translate("main", linkages.toUtf8().constData()));
    parser->addOption(opt_L1_variable);


    QCommandLineOption opt_L1_imageSequence(QStringList() << "L1_imageSequence",
                                            QCoreApplication::translate("main", "Directory containing linkage mask images (required)."),
                                            QCoreApplication::translate("main", "directory"));
    parser->addOption(opt_L1_imageSequence);

    QCommandLineOption opt_L1_mode(QStringList() << "L1_mode",
                                   QCoreApplication::translate("main", "Image file cycling mode (defaults to static)."),
                                   QCoreApplication::translate("main", "mode (Static|Once|Loop|Bounce)"));
    parser->addOption(opt_L1_mode);

    QCommandLineOption opt_L1_interpolate(QStringList() << "L1_interpolate",
                                          QCoreApplication::translate("main", "Image interpolation (defaults to true)."),
                                          QCoreApplication::translate("main", "On/Off"));
    parser->addOption(opt_L1_interpolate);

    QCommandLineOption opt_L1_change_rate(QStringList() << "L1_change_rate",
                                          QCoreApplication::translate("main", "Image refresh rate (defaults to 100)."),
                                          QCoreApplication::translate("main", "rate (integer)"));
    parser->addOption(opt_L1_change_rate);

    QCommandLineOption opt_L2_variable(QStringList() << "L2_variable",
                                       QCoreApplication::translate("main", "Second variable to be linked (required)."),
                                       QCoreApplication::translate("main", linkages.toUtf8().constData()));
    parser->addOption(opt_L2_variable);


    QCommandLineOption opt_L2_imageSequence(QStringList() << "L2_imageSequence",
                                            QCoreApplication::translate("main", "Directory containing second linkage mask images (required)."),
                                            QCoreApplication::translate("main", "directory"));
    parser->addOption(opt_L2_imageSequence);

    QCommandLineOption opt_L2_mode(QStringList() << "L2_mode",
                                   QCoreApplication::translate("main", "Image file cycling mode (defaults to static)."),
                                   QCoreApplication::translate("main", "mode (Static|Once|Loop|Bounce)"));
    parser->addOption(opt_L2_mode);

    QCommandLineOption opt_L2_interpolate(QStringList() << "L2_interpolate",
                                          QCoreApplication::translate("main", "Image interpolation (defaults to true)."),
                                          QCoreApplication::translate("main", "On/Off"));
    parser->addOption(opt_L2_interpolate);

    QCommandLineOption opt_L2_change_rate(QStringList() << "L2_change_rate",
                                          QCoreApplication::translate("main", "Image refresh rate (defaults to 100)."),
                                          QCoreApplication::translate("main", "rate (integer)"));
    parser->addOption(opt_L2_change_rate);

    parser->process(*app); //parse the command line

    //hash to hold converted form of parsing
    QHash<QString, QString> *hashResults = new QHash<QString, QString>();

    //RJG - options
    if (parser->isSet(opt_a)) hashResults->insert("a", parser->value(opt_a));
    if (parser->isSet(opt_b)) hashResults->insert("b", parser->value(opt_b));
    if (parser->isSet(opt_c)) hashResults->insert("c", parser->value(opt_c));
    if (parser->isSet(opt_d)) hashResults->insert("d", parser->value(opt_d));
    if (parser->isSet(opt_e)) hashResults->insert("e", parser->value(opt_e));
    if (parser->isSet(opt_f)) hashResults->insert("f", boolValue(parser->value(opt_f)));
    //RJG - H is help (this is automated by Qt)
    if (parser->isSet(opt_g)) hashResults->insert("g", boolValue(parser->value(opt_g)));
    if (parser->isSet(opt_i)) hashResults->insert("i", parser->value(opt_i));
    if (parser->isSet(opt_j)) hashResults->insert("j", parser->value(opt_j));
    if (parser->isSet(opt_k)) hashResults->insert("k", parser->value(opt_k));
    if (parser->isSet(opt_l)) hashResults->insert("l", boolValue(parser->value(opt_l)));
    if (parser->isSet(opt_m)) hashResults->insert("m", parser->value(opt_m));
    if (parser->isSet(opt_n)) hashResults->insert("n", parser->value(opt_n));
    if (parser->isSet(opt_o)) hashResults->insert("o", parser->value(opt_o));
    if (parser->isSet(opt_p)) hashResults->insert("p", parser->value(opt_p));
    if (parser->isSet(opt_q)) hashResults->insert("q", boolValue(parser->value(opt_q)));
    if (parser->isSet(opt_r)) hashResults->insert("r", parser->value(opt_r));
    if (parser->isSet(opt_s)) hashResults->insert("s", parser->value(opt_s));
    if (parser->isSet(opt_t)) hashResults->insert("t", boolValue(parser->value(opt_t)));
    if (parser->isSet(opt_u)) hashResults->insert("u", parser->value(opt_u));
    if (parser->isSet(opt_v)) hashResults->insert("v", boolValue(parser->value(opt_v)));
    if (parser->isSet(opt_w)) hashResults->insert("w", boolValue(parser->value(opt_w)));
    if (parser->isSet(opt_x)) hashResults->insert("x", parser->value(opt_x));
    if (parser->isSet(opt_y)) hashResults->insert("y", parser->value(opt_y));
    if (parser->isSet(opt_z)) hashResults->insert("z", parser->value(opt_z));
    //RJG - Positional arguments
    if (parser->isSet(opt_auto)) hashResults->insert("auto", parser->value(opt_auto));
    if (parser->isSet(opt_speciesBurnIn)) hashResults->insert("speciesburnin", boolValue(parser->value(opt_speciesBurnIn)));
    if (parser->isSet(opt_speciesBurnInDuration)) hashResults->insert("speciesburninduration", parser->value(opt_speciesBurnInDuration));
    if (parser->isSet(opt_nonspatial)) hashResults->insert("nonspatial", boolValue(parser->value(opt_nonspatial)));
    if (parser->isSet(opt_polling)) hashResults->insert("polling", parser->value(opt_polling));
    if (parser->isSet(opt_minspeciessize)) hashResults->insert("minspeciessize", parser->value(opt_minspeciessize));
    if (parser->isSet(opt_fitnesstarget)) hashResults->insert("fitnesstarget", parser->value(opt_fitnesstarget));
    if (parser->isSet(opt_breed)) hashResults->insert("breed", parser->value(opt_breed));
    if (parser->isSet(opt_variablemutate)) hashResults->insert("variablemutate", boolValue(parser->value(opt_variablemutate)));
    if (parser->isSet(opt_noGUI)) hashResults->insert("nogui", boolValue(parser->value(opt_noGUI)));
    if (parser->isSet(opt_pathogens)) hashResults->insert("pathogens", boolValue(parser->value(opt_pathogens)));
    if (parser->isSet(opt_pathogenMutate)) hashResults->insert("pathogenmutate", parser->value(opt_pathogenMutate));
    if (parser->isSet(opt_pathogenEvolve)) hashResults->insert("pathogenevolve", boolValue(parser->value(opt_pathogenEvolve)));
    if (parser->isSet(opt_pathogenFrequency)) hashResults->insert("pathogenfrequency", parser->value(opt_pathogenFrequency));
    if (parser->isSet(opt_customLogging)) hashResults->insert("customlogging", boolValue(parser->value(opt_customLogging)));
    if (parser->isSet(opt_disparityLogging)) hashResults->insert("disparityLogging", boolValue(parser->value(opt_disparityLogging)));
    if (parser->isSet(opt_interactblocks)) hashResults->insert("interactblocks", boolValue(parser->value(opt_interactblocks)));
    if (parser->isSet(opt_multibreedlist)) hashResults->insert("multibreedlist", boolValue(parser->value(opt_multibreedlist)));
    if (parser->isSet(opt_interactrate)) hashResults->insert("interactrate", parser->value(opt_interactrate));
    if (parser->isSet(opt_interactfitness)) hashResults->insert("interactfitness", boolValue(parser->value(opt_interactfitness)));
    if (parser->isSet(opt_interactenergy)) hashResults->insert("interactenergy", boolValue(parser->value(opt_interactenergy)));
    if (parser->isSet(opt_v2log)) hashResults->insert("v2log", boolValue(parser->value(opt_v2log)));
    if (parser->isSet(opt_log))hashResults->insert("opt_log", parser->value(opt_log));
    if (parser->isSet(opt_predationefficiency)) hashResults->insert("predationefficiency", parser->value(opt_predationefficiency));
    if (parser->isSet(opt_minpredatorscore)) hashResults->insert("minpredatorscore", parser->value(opt_minpredatorscore));
    if (parser->isSet(opt_settings)) hashResults->insert("settings", parser->value(opt_settings));
    if (parser->isSet(opt_maxthreads)) hashResults->insert("maxthreads", parser->value(opt_maxthreads));

    //RJG - Logging images
    if (parser->isSet(opt_li_population)) hashResults->insert("li_population", boolValue(parser->value(opt_li_population)));
    if (parser->isSet(opt_li_fitness)) hashResults->insert("li_fitness", boolValue(parser->value(opt_li_fitness)));
    if (parser->isSet(opt_li_sys_visualisation)) hashResults->insert("li_coding", boolValue(parser->value(opt_li_sys_visualisation)));
    if (parser->isSet(opt_li_sys_visualisation2)) hashResults->insert("li_noncoding", boolValue(parser->value(opt_li_sys_visualisation2)));
    if (parser->isSet(opt_li_species)) hashResults->insert("li_species", boolValue(parser->value(opt_li_species)));
    if (parser->isSet(opt_li_settles)) hashResults->insert("li_settles", boolValue(parser->value(opt_li_settles)));
    if (parser->isSet(opt_li_fails)) hashResults->insert("li_fails", boolValue(parser->value(opt_li_fails)));
    if (parser->isSet(opt_li_environment)) hashResults->insert("li_environment", boolValue(parser->value(opt_li_environment)));
    //RJG - Systems
    if (parser->isSet(opt_sys_fitness))hashResults->insert("sys_fitness", parser->value(opt_sys_fitness));
    if (parser->isSet(opt_sys_breed))hashResults->insert("sys_breed", parser->value(opt_sys_breed));
    if (parser->isSet(opt_sys_mutate))hashResults->insert("sys_mutate", parser->value(opt_sys_mutate));
    if (parser->isSet(opt_sys_var_mutate))hashResults->insert("sys_var_mutate", parser->value(opt_sys_var_mutate));
    if (parser->isSet(opt_sys_var_breed))hashResults->insert("sys_var_breed", parser->value(opt_sys_var_breed));
    if (parser->isSet(opt_sys_pathogens))hashResults->insert("sys_pathogens", parser->value(opt_sys_pathogens));
    if (parser->isSet(opt_sys_species_ID))hashResults->insert("sys_species_ID", parser->value(opt_sys_species_ID));
    if (parser->isSet(opt_sys_interactions))hashResults->insert("sys_interactions", parser->value(opt_sys_interactions));
    if (parser->isSet(opt_sys_visualisation))hashResults->insert("sys_visualisation", parser->value(opt_sys_visualisation));
    if (parser->isSet(opt_sys_visualisation2))hashResults->insert("sys_visualisation2", parser->value(opt_sys_visualisation2));
    //RJG - Linkages
    if (parser->isSet(opt_L1_variable)) hashResults->insert("L1_variable", parser->value(opt_L1_variable));
    if (parser->isSet(opt_L1_imageSequence)) hashResults->insert("L1_imageSequence", parser->value(opt_L1_imageSequence));
    if (parser->isSet(opt_L1_mode)) hashResults->insert("L1_mode", parser->value(opt_L1_mode));
    if (parser->isSet(opt_L1_interpolate)) hashResults->insert("L1_interpolate", boolValue(parser->value(opt_L1_interpolate)));
    if (parser->isSet(opt_L1_change_rate)) hashResults->insert("L1_change_rate", parser->value(opt_L1_change_rate));
    if (parser->isSet(opt_L2_variable)) hashResults->insert("L2_variable", parser->value(opt_L2_variable));
    if (parser->isSet(opt_L2_imageSequence)) hashResults->insert("L2_imageSequence", parser->value(opt_L2_imageSequence));
    if (parser->isSet(opt_L2_mode)) hashResults->insert("L2_mode", parser->value(opt_L2_mode));
    if (parser->isSet(opt_L2_interpolate)) hashResults->insert("L2_interpolate", boolValue(parser->value(opt_L2_interpolate)));
    if (parser->isSet(opt_L2_change_rate)) hashResults->insert("L2_change_rate", parser->value(opt_L2_change_rate));

    delete parser;

    return hashResults;
}

/*!
 * \brief qMain
 * \param argc
 * \param argv
 * \return The application
 */
int main(int argc, char *argv[])
{

    //This has QPixmap images use the @2x images when available
    //See this bug for more details on how to get this right: https://bugreports.qt.io/browse/QTBUG-44486#comment-327410
    QApplication application(argc, argv);

    //Close on last window close
    QApplication::setQuitOnLastWindowClosed(true);

    //Style program with our dark style
    QApplication::setStyle(new DarkStyleTheme);

    QHash<QString, QString> *parsedHash = parse(&application);

    if (!parsedHash->contains("auto"))
    {
        QPixmap splashPixmap(":/palaeoware_logo_square.png");
        QSplashScreen *splash = new QSplashScreen(splashPixmap, Qt::WindowStaysOnTopHint);
        splash->setAttribute(Qt::WA_DeleteOnClose, true);
        splash->show();
        splash->showMessage("<font><b>" + QString(PRODUCTNAME) + " - " + QString(PRODUCTTAG) + "</b></font>", Qt::AlignHCenter, Qt::white);
        QApplication::processEvents();
        QTimer::singleShot(5000, splash, SLOT(close()));
    }

    MainWindow window;

    window.setOptionsFromParser(parsedHash);

    window.show();

    return QApplication::exec();
}

