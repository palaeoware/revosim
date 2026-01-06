#include "logsimulation.h"
#include "simmanager.h"
#include "mainwindow.h"
#include "analysistools.h"
#include <QXmlStreamWriter>

LogSimulation::LogSimulation(SimSettings *settings)
{
    simSettings = settings;
    csvOutput = false;
    CreateReplacementText();
}

void LogSimulation::CreateReplacementText()
{
    replacementText.clear();
    //RJG - List of all replacements/tags which will be useful for numerous things
    //RJG - Simulation wide tags
    replacementText.insert("*iteration*", "Iteration_number");
    replacementText.insert("*gridNumberAlive*", "Number_living_organisms");
    replacementText.insert("*gridMeanFitness*", "Grid_mean_fitness");
    replacementText.insert("*gridBreedEntries*", "Grid_breed_list_entries");
    replacementText.insert("*gridBreedFails*", "Grid_bred_fails");
    replacementText.insert("*gridBreedSuccess*", "Grid_bred_success");
    replacementText.insert("*speciesCount*", "Species_count");
    replacementText.insert("*gridTrophicHistograms*", "Trophic_histograms");
    replacementText.insert("*gridGeneration*", "Grid_generatrion_time");
    // RJG - Species tags
    replacementText.insert("*speciesID*", "Species_ID");
    replacementText.insert("*originTime*", "Origin_time_of_species");
    replacementText.insert("*speciesParent*", "Species_parent");
    replacementText.insert("*speciesSize*", "Species_size");
    replacementText.insert("*speciesModalGenome*", "Modal_species_genome");
    replacementText.insert("*speciesMeanFitness*", "species_Mean_Fitness");
    replacementText.insert("*speciesMeanEnvironmentalFitness*", "species_Mean_Environmental_Fitness");
    replacementText.insert("*speciesMeanRunningEnergy*", "species_Mean_Running_Energy");
    replacementText.insert("*speciesMeanRunningStolenEnergy*", "species_Mean_Running_Stolen_Energy");
    replacementText.insert("*speciesTrophicLevel*", "species_Trophic_Level");
    replacementText.insert("*speciesGenomeDiversity*", "species_Genome_Diversity");
    replacementText.insert("*completeSpeciesData*", LogSpeciesDataItem::headersForSharedOutput());
    replacementText.insert("*printSettings*", "*printSettings*");
    replacementText.insert("*printTime*", "*printTime*");
    replacementText.insert("*dumpGenomes*", "Genome_dump");
    replacementText.insert("*dumpSpecies*", "Species_dump");

    replacementText.insert("*gridSpeciesRange*", "Grid_species_range");
    QStringList frequenciesHeaders;
    for (int ii = 0; ii < simSettings->genomeSize; ii++)
        for (int jj = 0; jj < 32; jj++)
            frequenciesHeaders.append(QString("O_%1_%2").arg(ii).arg(jj));
    replacementText.insert("*originalGeneFrequencies*", frequenciesHeaders.join(","));
    frequenciesHeaders.clear();
    for (int ii = 0; ii < simSettings->genomeSize; ii++)
        for (int jj = 0; jj < 32; jj++)
            frequenciesHeaders.append(QString("C_%1_%2").arg(ii).arg(jj));
    replacementText.insert("*currentGeneFrequencies*", frequenciesHeaders.join(","));
    replacementText.insert("*Ca*", "sum_fitness_difference_from_origin_(Ca)");
    replacementText.insert("*NCa*", "sum_breed_only_difference_from_origin_(NCa)");
    replacementText.insert("*Cr*", "sum_fitness_difference_from_last_(Cr)");
    replacementText.insert("*NCr*", "sum_breed_only_difference_from_last_(NCr)");
}
/********** Access functions **********/
QString LogSimulation::printHeaderText()
{
    return headerText;
}

//This is currently not in use, as it's generated from the user input text as required for iteration or species log
QString LogSimulation::printDefaultHeaderText()
{
    QString outString;
    QTextStream out(&outString);

    if (csvOutput)
    {
        out << "Iteration_Number,Living_Organism_Count,Mean_Fitness,Breed_Entries,Failed_Breeds,Species_Count,Species_ID,Species_Origin,Species_Parent,Species_Population,Species_Modal_Genome,Changes_Since_Origination_Coding,Changes_Since_Origination_NonCoding,Changes_Since_Last_Coding,Changes_Since_Last_NonCoding";
        for (int ii = 0; ii < 64; ii++) out << ",f" << ii;
        out << ",sum_coding_initial_differences,sum_noncoding_initial_differences,sum_coding_since_last_differences,sum_noncoding_since_last_differences";
        out << "," + LogSpeciesDataItem::headersForSharedOutput() + "<br />";
    }
    else
    {
        out << "New run *printTime*<br /><br />===================<br /><br />*printSettings*<br /><br />===================<br />";
        out << "\nFor each iteration, this log features:<br /><br />";
        out << "- [I] Iteration Number<br /><br />";
        out << "- [P] Population Grid Data:<br />";
        out << "-- Number of living digital organisms<br />";
        out << "-- Mean fitness of living digital organisms<br />";
        out << "-- Number of entries on the breed list<br />";
        out << "-- Number of failed breed attempts<br />";
        out << "-- Number of species<br />";
        out << "- [S] Species Data:<br />";
        out << "-- Species id<br />";
        out << "-- Species origin (iterations)<br />";
        out << "-- Species parent<br />";
        out << "-- Species current size (number of individuals)<br />";
        out << "-- Species mean fitness<br />";
        out << "-- Species mean environmental (non-interaction) fitness<br />";
        out << "-- Species current modal genome<br />";
        out << "<br />";
        out << "**Note that this excludes species with less individuals than minimum species size, but is not able to exlude species without descendants, which can only be achieved with the end-run log.**<br /><br />";
        out << "===================<br /><br />";
    }
    return outString;
}

QString LogSimulation::printLogIterationText()
{
    return logIterationText;
}

QString LogSimulation::printDefaultLogIterationText()
{
    QString outString;
    QTextStream out(&outString);

    if (csvOutput) return (""); //RJG - not needed for CSV logs
    else out << "<br />[I] *iteration*<br />[P] *gridNumberAlive*,*gridMeanFitness*,*gridBreedEntries*,*gridBreedFails*,*speciesCount*<br />";
    return outString;
}

QString LogSimulation::printLogSpeciesText()
{
    return logSpeciesText;
}

QString LogSimulation::printDefaultLogSpeciesText()
{

    QString outString;
    QTextStream out(&outString);

    if (csvOutput)
        out << "*iteration*,*gridNumberAlive*,*gridMeanFitness*,*gridBreedEntries*,*gridBreedFails*,*speciesCount*,*speciesID*,*originTime*,*speciesParent*,*speciesSize*,*speciesModalGenome*<br />";
    else out << "[S] *speciesID*,*originTime*,*speciesParent*,*speciesSize*,*speciesMeanFitness*,*speciesMeanEnvironmentalFitness*,*speciesModalGenome*,*speciesGenomeDiversity*<br />";

    return outString;
}

/********** Utility functions **********/
QString LogSimulation::printSettings()
{

    CellSettings cellsettings = *simulationManager->cellSettingsMaster;

    QString settings;
    QTextStream settingsOut(&settings);

    if (!csvOutput)
    {

        settingsOut << "REvoSim settings:\n\n";
        settingsOut << "- Integers:\n";
        settingsOut << "-- Grid X: " << simSettings->gridX << "\n";
        settingsOut << "-- Grid Y: " << simSettings->gridY << "\n";
        settingsOut << "-- Settle tolerance: " << cellsettings.settleTolerance << "\n";
        settingsOut << "-- Start age: " << cellsettings.startAge << "\n";
        settingsOut << "-- Dispersal: " << cellsettings.dispersal << "\n";
        settingsOut << "-- Food: " << cellsettings.food << "\n";
        settingsOut << "-- Breed cost: " << cellsettings.breedCost << "\n";
        settingsOut << "-- Mutate: " << cellsettings.mutate << "\n";
        settingsOut << "-- Pathogen mutate: " << cellsettings.pathogenMutate << "\n";
        settingsOut << "-- Pathogen frequency: " << cellsettings.pathogenFrequency << "\n";
        settingsOut << "-- Max diff to breed: " << simSettings->maxDifference << "\n";
        settingsOut << "-- Breed threshold: " << cellsettings.breedThreshold << "\n";
        settingsOut << "-- Slots per square: " << cellsettings.slotsPerSquare << "\n";
        settingsOut << "-- Fitness target: " << cellsettings.target << "\n";
        settingsOut << "-- Environmental change rate: " << simSettings->environmentChangeRate << "\n";
        settingsOut << "-- Minimum species size: " << simSettings->minSpeciesSize << "\n";
        settingsOut << "-- Environment mode: " << simSettings->environmentMode << "\n";
        settingsOut << "-- Species mode: " << speciesMode << "\n";
        settingsOut << "-- Genome size: " << simSettings->genomeSize << "\n";
        settingsOut << "-- Interaction attempts per organism per iteration: " << cellsettings.interactions << "\n";
        settingsOut << "-- Cropping rate: " << cellsettings.croppingFrequency << "\n";
        settingsOut << "-- Species burn in duration: " << simSettings->speciesBurnInDuration << "\n";

        settingsOut << "\n- Bools:\n";
        settingsOut << "-- Recalculate fitness: " << simSettings->recalculateFitness << "\n";
        settingsOut << "-- No selection: " << cellsettings.noSelection << "\n";
        settingsOut << "-- Toroidal environment: " << simSettings->toroidal << "\n";
        settingsOut << "-- Interpolate environment: " << simSettings->environmentInterpolate << "\n";
        settingsOut << "-- Nonspatial settling: " << simSettings->nonspatial << "\n";
        settingsOut << "-- Enforce max diff to breed: " << cellsettings.breedDifference << "\n";
        settingsOut << "-- Only breed within species: " << cellsettings.breedSpecies << "\n";
        settingsOut << "-- Pathogens enabled: " << cellsettings.pathOn << "\n";
        settingsOut << "-- Variable mutate: " << cellsettings.variableMutate << "\n";
        settingsOut << "-- Exclude species without descendants: " << allowExcludeWithDescendants << "\n";
        settingsOut << "-- Interactions using blocks rather than genome XOR: " << cellsettings.interactBlocks << "\n";
        settingsOut << "-- Interactions change fitness: " << cellsettings.interactFitness << "\n";
        settingsOut << "-- Interactions change energy: " << cellsettings.interactEnergy << "\n";
        settingsOut << "-- Multiple breed lists: " << cellsettings.multiBreedList << "\n";
        settingsOut << "-- Random reseed before genetic: " << simSettings->randomReseedBeforeGenetic << "\n";
        settingsOut << "-- Species burnin: "  << simSettings->speciesBurnIn << "\n";

        settingsOut << "-- Breeding: ";
        if (cellsettings.obligateSexual) settingsOut << "obligate sexual" << "\n";
        else if (cellsettings.facultativeSexual) settingsOut << "facultative sexual" << "\n";
        else if (cellsettings.asexual) settingsOut << "asexual" << "\n";
        else settingsOut << "variable" << "\n";

        settingsOut << "-- Pathogen mopde: ";
        if (simulationManager->simulationSettings->pathogenMode == PATH_MODE_DRIFT) settingsOut << "Drift\n";
        else settingsOut << "Evolve\n";

        settingsOut << "\n- Systems:\n";
        for (auto &s : simulationManager->systemsList) settingsOut << "-- " << s->returnName() << " is applied to words " << s->returnGenomeWordInUseString() << "\n";

        if (cellsettings.interactBlocks)
        {
            settingsOut << "\n- Interactions Matrix:\n";
            settingsOut << "{";
            for (int i1 = 0; i1 < 4; i1++)
            {
                settingsOut << "{";
                for (int i2 = 0; i2 < 4; i2++)
                {
                    settingsOut << simSettings->a_priori_interaction[i1][i2];
                    if (i2 < 3)
                        settingsOut << ",";
                }
                settingsOut << "}";
                if (i1 < 3)
                    settingsOut << ",";
            }
            settingsOut << "}";
        }
    }
    else
    {
        settingsOut << "REvoSim settings:\n";
        settingsOut << "Integers\n";
        settingsOut << "Grid X,";
        settingsOut << "Grid Y,";
        settingsOut << "Settle tolerance,";
        settingsOut << "Start age,";
        settingsOut << "Disperal,";
        settingsOut << "Food,";
        settingsOut << "Breed cost,";
        settingsOut << "Mutate,";
        settingsOut << "Pathogen mutate,";
        settingsOut << "Pathogen frequency,";
        settingsOut << "Max diff to breed,";
        settingsOut << "Breed threshold,";
        settingsOut << "Slots per square,";
        settingsOut << "Fitness target,";
        settingsOut << "Environmental change rate,";
        settingsOut << "Minimum species size,";
        settingsOut << "Environment mode,";
        settingsOut << "Speices mode,";
        settingsOut << "Genome size,";
        settingsOut << "Interaction attempts per organism per iteration,";
        settingsOut << "Cropping rate," << "\n";

        settingsOut << simSettings->gridX << ",";
        settingsOut << simSettings->gridY << ",";
        settingsOut << cellsettings.settleTolerance << ",";
        settingsOut << cellsettings.startAge << ",";
        settingsOut << cellsettings.dispersal << ",";
        settingsOut << cellsettings.food << ",";
        settingsOut << cellsettings.breedCost << ",";
        settingsOut << cellsettings.mutate << ",";
        settingsOut << cellsettings.pathogenMutate << ",";
        settingsOut << cellsettings.pathogenFrequency << ",";
        settingsOut << simSettings->maxDifference << ",";
        settingsOut << cellsettings.breedThreshold << ",";
        settingsOut << cellsettings.slotsPerSquare << ",";
        settingsOut << cellsettings.target << ",";
        settingsOut << simSettings->environmentChangeRate << ",";
        settingsOut << simSettings->minSpeciesSize << ",";
        settingsOut << simSettings->environmentMode << ",";
        settingsOut << speciesMode << ",";
        settingsOut << simSettings->genomeSize << ",";
        settingsOut << cellsettings.interactions << ",";
        settingsOut << cellsettings.croppingFrequency << "\n";

        settingsOut << "Bools\n";
        settingsOut << "Recalculate fitness,";
        settingsOut << "No selection,";
        settingsOut << "Toroidal environment,";
        settingsOut << "Interpolate environment,";
        settingsOut << "Nonspatial setling,";
        settingsOut << "Enforce max diff to breed,";
        settingsOut << "Only breed within species,";
        settingsOut << "Pathogens enabled,";
        settingsOut << "Variable mutate,";
        settingsOut << "Exclude species without descendants,";
        settingsOut << "Interactions using blocks,";
        settingsOut << "Interactions using XOR,";
        settingsOut << "Interactions change fitness,";
        settingsOut << "Interactions change energy,";
        settingsOut << "Multiple breed lists,";
        settingsOut << "Random reseed before genetic,";
        settingsOut << "Breeding,";
        settingsOut << "Pathogen mode, ";
        settingsOut << "Species burnin, " << "\n";

        settingsOut << simSettings->recalculateFitness << ",";
        settingsOut << cellsettings.noSelection << ",";
        settingsOut << simSettings->toroidal << ",";
        settingsOut << simSettings->environmentInterpolate << ",";
        settingsOut << simSettings->nonspatial << ",";
        settingsOut << cellsettings.breedDifference << ",";
        settingsOut << cellsettings.breedSpecies << ",";
        settingsOut << cellsettings.pathOn << ",";
        settingsOut << cellsettings.variableMutate << ",";
        settingsOut << allowExcludeWithDescendants << ",";
        settingsOut << cellsettings.interactBlocks << ",";
        settingsOut << cellsettings.interactFitness << ",";
        settingsOut << cellsettings.interactEnergy << ",";
        settingsOut << cellsettings.multiBreedList << ",";
        settingsOut << simSettings->randomReseedBeforeGenetic << ",";

        if (cellsettings.obligateSexual) settingsOut << "obligate sexual" << ",";
        else if (cellsettings.facultativeSexual) settingsOut << "facultative sexual" << ",";
        else if (cellsettings.asexual) settingsOut << "asexual" << ",";
        else settingsOut << "variable" << ",";

        if (simulationManager->simulationSettings->pathogenMode == PATH_MODE_DRIFT) settingsOut << "Drift,";
        else settingsOut << "Evolve,";

        settingsOut << simSettings->speciesBurnIn << "\n";

        settingsOut << "Systems\n";
        for (auto &s : simulationManager->systemsList) settingsOut << s->returnName() << ",";
        settingsOut << "\n";
        for (auto &s : simulationManager->systemsList) settingsOut << s->returnGenomeWordInUseString() << ",";
        settingsOut << "\n";

        if (cellsettings.interactBlocks)
        {
            settingsOut << "\nInteractions Matrix,";
            settingsOut << "{";
            for (int i1 = 0; i1 < 4; i1++)
            {
                settingsOut << "{";
                for (int i2 = 0; i2 < 4; i2++)
                {
                    settingsOut << simSettings->a_priori_interaction[i1][i2];
                    if (i2 < 3)
                        settingsOut << ":";
                }
                settingsOut << "}";
                if (i1 < 3)
                    settingsOut << ":";
            }
            settingsOut << "}";
        }
    }

    return settings;
}

void LogSimulation::setHeaderTextFromGUI(QString text)
{
    headerText = text;
}

void LogSimulation::setIterationTextFromGUI(QString text)
{
    logIterationText = text;
}

void LogSimulation::setSpeciestTextFromGUI(QString text)
{
    logSpeciesText = text;
}

QString LogSimulation::writeHeaderFromLogText(QString text)
{
    //RJG - Replace tags using list
    QHashIterator<QString, QString> i(replacementText);
    while (i.hasNext())
    {
        i.next();
        text.replace(i.key(), i.value());
    }
    text = validateString(text);
    return text;
}

QString LogSimulation::validateString(QString text)
{
    //Now check for non-converted tags and colour
    QRegularExpression re("\\*(.*?)\\*", QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatchIterator j = re.globalMatch(text);
    while (j.hasNext())
    {
        QRegularExpressionMatch match = j.next();
        QString tag = match.captured(0);
        //Don't want the settings to be replaced for header as this will presumably want to be written at the time the header is written and replaced there
        if (tag == "*printSettings*" || tag == "*printTime*" || tag == "**") continue;
        QString newTag = QString("<font color=\"Red\">" + tag + "</font color>");
        text.replace(tag, newTag);
    }
    return text;
}

QString LogSimulation::validateLog(QString text, int logType)
{
    QString returnText = text;
    if (logType == LOG_ITERATION) text = processLogTextGeneral(text);
    if (logType == LOG_SPECIES) text = processLogTextSpecies(text);
    //if (logType == LOG_HEADER) <-- do nothing here, assume the header is already processed, and anything other than *printSettings* is bogus

    //Get rid of any red already...
    returnText.replace("color:#ff0000;", "");

    //RJG - Need to do this here so as to shunt new tags through to the return text but not actually change the tags to the associated text
    QRegularExpression re("\\*(.*?)\\*", QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatchIterator j = re.globalMatch(text);
    while (j.hasNext())
    {
        QRegularExpressionMatch match = j.next();
        QString tag = match.captured(0);
        //Don't want the settings to be replaced for header as this will presumably want to be written at the time the header is written and replaced there
        if (tag == "*printSettings*" || tag == "*printTime*" || tag == "**") continue;
        QString newTag = QString("<font color=\"Red\">" + tag + "</font color>");
        returnText.replace(tag, newTag);
    }

    return returnText;
}

/********** Logging functions **********/
//At end of run in run for/batch mode, or on click when a run is going, this allows user to output the final log, along with the tree for the run.
void LogSimulation::writeRunData(QString globalSavePath, int batchRuns)
{

    QString FinalLoggingFile(globalSavePath);
    if (!FinalLoggingFile.endsWith(QDir::separator())) FinalLoggingFile.append(QDir::separator());
    FinalLoggingFile.append(QString(PRODUCTNAME) + "_end_run_log");
    if (batchRuns > -1) FinalLoggingFile.append(QString("_run_%1").arg(batchRuns, 4, 10, QChar('0')));
    FinalLoggingFile.append(".txt");
    QFile outputfile(FinalLoggingFile);
    outputfile.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&outputfile);

    out << "End run log ";
    QDateTime t(QDateTime::currentDateTime());
    out << t.toString(Qt::ISODate) << "\n\n===================\n\n" << printSettings() <<
        "\n\n===================\n";
    out << "\nThis log features the tree from a finished run, in Newick format, and then data for all the species that have existed with more individuals than minimum species size. The exact data provided depends on the phylogeny tracking mode selected in the GUI.\n";
    out << "\n\n===================\n\n";
    out << "Tree:\n\n" << mainWindow->handleAnalysisTool(ANALYSIS_TOOL_CODE_MAKE_NEWICK);
    out << "\n\nSpecies data:\n\n";
    out << mainWindow->handleAnalysisTool(ANALYSIS_TOOL_CODE_WRITE_DATA);
    outputfile.close();
}

//RJG - per species level outputs
QString LogSimulation::processLogTextSpecies(QString text)
{
    //RJG - we may want to have some of these general tags in our species-level log - process this first
    text = processLogTextGeneral(text);
    //RJG - for this log output to a new string, as will need repeats
    QString outString;
    QTextStream out(&outString);


    for (int i = 0; i < oldSpeciesList->count(); i++)
    {
        //----RJG: Unable to exclude species without descendants, for obvious reasons.
        if (quint64(oldSpeciesList->at(i).size) < simulationManager->simulationSettings->minSpeciesSize) continue;

        QString textLocal(text);
        textLocal.replace("*speciesID*",  QString::number(oldSpeciesList->at(i).ID));
        textLocal.replace("*originTime*",  QString::number(oldSpeciesList->at(i).originTime));
        textLocal.replace("*speciesParent*",  QString::number(oldSpeciesList->at(i).parent));
        textLocal.replace("*speciesSize*",  QString::number(oldSpeciesList->at(i).size));
        textLocal.replace("*speciesMeanFitness*", QString::number(oldSpeciesList->at(i).fitness));
        textLocal.replace("*speciesMeanEnvironmentalFitness*", QString::number(oldSpeciesList->at(i).envFitness));
        textLocal.replace("*speciesMeanRunningEnergy*", QString::number(oldSpeciesList->at(i).totalEnergy));
        textLocal.replace("*speciesMeanRunningStolenEnergy*", QString::number(oldSpeciesList->at(i).totalStolenEnergy));
        textLocal.replace("*speciesTrophicLevel*", QString::number(oldSpeciesList->at(i).trophicLevel));
        textLocal.replace("*speciesGenomeDiversity*", QString::number(oldSpeciesList->at(i).genomeDiversity));

        //RJG - for now, let's output whole genome (guess whether it's this or the species system words is worth discussion
        //MDS - could be a new system for this! For now, I moved returnGenomeString to be a static of System
        if (textLocal.contains("*speciesModalGenome*"))
        {
            QString genomeString(System::returnGenomeString(oldSpeciesList->at(i).type, simulationManager->simulationSettings->genomeSize));
            textLocal.replace("*speciesModalGenome*", genomeString);
        }

        if (textLocal.contains("*completeSpeciesData*"))
        {
            if (speciesMode != SPECIES_MODE_PHYLOGENY_AND_METRICS) textLocal.replace("*completeSpeciesData*", "Wrong species mode to record complete species data");
            else
            {
                LogSpeciesDataItem thisDataItem = (oldSpeciesList->at(i).complexLogData);
                QString complexData (thisDataItem.sharedCSVoutput());
                textLocal.replace("*completeSpeciesData*", complexData);
            }
        }

        // and now... the PE gene frequency stuff
        if (textLocal.contains(("*originalGeneFrequencies*")))
            textLocal.replace("*originalGeneFrequencies*", GetFrequenciesLogString(oldSpeciesList->at(i), true));

        if (textLocal.contains(("*currentGeneFrequencies*")))
            textLocal.replace("*currentGeneFrequencies*", GetFrequenciesLogString(oldSpeciesList->at(i), false));

        textLocal.replace("*Ca*", QString::number(oldSpeciesList->at(i).Ca));
        textLocal.replace("*Cr*", QString::number(oldSpeciesList->at(i).Cr));
        textLocal.replace("*NCa*", QString::number(oldSpeciesList->at(i).NCa));
        textLocal.replace("*NCr*", QString::number(oldSpeciesList->at(i).NCr));

        out << textLocal;
    }


    return outString;
}

QString LogSimulation::GetFrequenciesLogString(Species species, bool original)
{
    QStringList stringList;
    for (int i = 0; i < simulationManager->simulationSettings->genomeSize; i++)
        for (int j = 0; j < 32; j++)
        {
            if (original)
                stringList.append(QString::number(species.frequenciesAtOrigination[i][j]));
            else
                stringList.append(QString::number(species.frequenciesLastIteration[i][j]));
        }

    return stringList.join(",");
}

//RJG - grid level outputs
QString LogSimulation::processLogTextGeneral(QString text)
{
    text.replace("*iteration*", QString::number(simulationManager->iteration));
    //RJG - espcae sequence for star
    text.replace("**", "*");
    text.replace("*speciesCount*", QString::number(oldSpeciesList->count()));
    text.replace("*printSettings*", printSettings());
    QDateTime t(QDateTime::currentDateTime());
    text.replace("*printTime*", t.toString(Qt::ISODate));
    text.replace("*dumpGenomes*", writeDisparityLog());
    text.replace("*dumpSpecies*", writeGridSpecies());

    if (text.contains("*gridSpeciesRange*"))
    {
        QList <int> ranges;
        for (auto s : *oldSpeciesList)
        {
            int cells = 0;
            for (int i = 0; i < simulationManager->simulationSettings->gridX; i++)
                for (int j = 0; j < simulationManager->simulationSettings->gridY; j++)
                {
                    bool present = false;
                    for  (int k = 0; k < simulationManager->cellSettingsMaster->slotsPerSquare; k++)
                        if (critters[i][j][k].age > 0)
                            if (critters[i][j][k].speciesID == s.ID)
                            {
                                present = true;
                                break;
                            }
                    if (present) cells++;
                }
            ranges.append(cells);
        }
        int total = 0;
        for (int i : ranges) total += i;
        double mean = static_cast<double>(total) / static_cast<double>(ranges.count());
        text.replace("*gridSpeciesRange*", QString::number(mean));
    }

    //RJG - grid level stats
    if (text.contains("*gridNumberAlive*") || text.contains("*gridBreedEntries*") || text.contains("*gridBreedFails*") || text.contains("*gridMeanFitness*") || text.contains("*gridTrophicHistograms*")
            || text.contains("*gridBreedSuccess*") || text.contains("*gridGeneration*"))
    {
        int gridNumberAlive = 0, gridTotalFitness = 0, gridBreedEntries = 0, gridBreedFails = 0, gridBreedSuccess = 0, gridGeneration = 0;
        int trophicL01 = 0, trophic0102 = 0, trophic0203 = 0, trophic0304 = 0, trophic0405 = 0, trophic0506 = 0, trophic0607 = 0, trophic0708 = 0, trophic0809 = 0, trophic0910 = 0, trophic1011 = 0,
            trophic1112 = 0, trophic1213 = 0, trophic1314 = 0, trophic1415 = 0, trophic1516 = 0, trophic1617 = 0, trophic1718 = 0, trophic1819 = 0, trophic1920 = 0, trophic2021 = 0, trophic2122 = 0,
            trophic2223 = 0, trophic2324 = 0, trophic2425 = 0, trophic2526 = 0, trophic2627 = 0, trophic2728 = 0, trophic2829 = 0, trophic2930 = 0, trophicG30 = 0;
        for (int i = 0; i < simulationManager->simulationSettings->gridX; i++)
            for (int j = 0; j < simulationManager->simulationSettings->gridY; j++)
            {
                gridTotalFitness += totalFitness[i][j];
                //----RJG: Manually count breed stuff for grid
                gridBreedEntries += breedAttempts[i][j];
                gridBreedSuccess += breedSuccess[i][j];
                gridBreedFails += breedFails[i][j];
                gridGeneration += breedGeneration[i][j];

                //----RJG: Manually count number alive thanks to maxUsed descendants
                for  (int k = 0; k < simulationManager->cellSettingsMaster->slotsPerSquare; k++)
                    if (critters[i][j][k].age > 0)
                    {
                        gridNumberAlive++;
                        if (critters[i][j][k].trophicLevel < 0.1) trophicL01++;
                        else if (critters[i][j][k].trophicLevel < 0.2) trophic0102++;
                        else if (critters[i][j][k].trophicLevel < 0.3) trophic0203++;
                        else if (critters[i][j][k].trophicLevel < 0.4) trophic0304++;
                        else if (critters[i][j][k].trophicLevel < 0.5) trophic0405++;
                        else if (critters[i][j][k].trophicLevel < 0.6) trophic0506++;
                        else if (critters[i][j][k].trophicLevel < 0.7) trophic0607++;
                        else if (critters[i][j][k].trophicLevel < 0.8) trophic0708++;
                        else if (critters[i][j][k].trophicLevel < 0.9) trophic0809++;
                        else if (critters[i][j][k].trophicLevel < 1.0) trophic0910++;
                        else if (critters[i][j][k].trophicLevel < 1.1) trophic1011++;
                        else if (critters[i][j][k].trophicLevel < 1.2) trophic1112++;
                        else if (critters[i][j][k].trophicLevel < 1.3) trophic1213++;
                        else if (critters[i][j][k].trophicLevel < 1.4) trophic1314++;
                        else if (critters[i][j][k].trophicLevel < 1.5) trophic1415++;
                        else if (critters[i][j][k].trophicLevel < 1.6) trophic1516++;
                        else if (critters[i][j][k].trophicLevel < 1.7) trophic1617++;
                        else if (critters[i][j][k].trophicLevel < 1.8) trophic1718++;
                        else if (critters[i][j][k].trophicLevel < 1.9) trophic1819++;
                        else if (critters[i][j][k].trophicLevel < 2.0) trophic1920++;
                        else if (critters[i][j][k].trophicLevel < 2.1) trophic2021++;
                        else if (critters[i][j][k].trophicLevel < 2.2) trophic2122++;
                        else if (critters[i][j][k].trophicLevel < 2.3) trophic2223++;
                        else if (critters[i][j][k].trophicLevel < 2.4) trophic2324++;
                        else if (critters[i][j][k].trophicLevel < 2.5) trophic2425++;
                        else if (critters[i][j][k].trophicLevel < 2.6) trophic2526++;
                        else if (critters[i][j][k].trophicLevel < 2.7) trophic2627++;
                        else if (critters[i][j][k].trophicLevel < 2.8) trophic2728++;
                        else if (critters[i][j][k].trophicLevel < 2.9) trophic2829++;
                        else if (critters[i][j][k].trophicLevel < 3.0) trophic2930++;
                        else trophicG30++;
                    }
            }
        double gridMeanFitness = static_cast<double>(gridTotalFitness) / static_cast<double>(gridNumberAlive);
        text.replace("*gridNumberAlive*", QString::number(gridNumberAlive));
        text.replace("*gridBreedEntries*", QString::number(gridBreedEntries));
        text.replace("*gridBreedSuccess*", QString::number(gridBreedSuccess));
        //Note the below assumes one breed per generation, and user should check this. Is in docs.
        if (gridBreedSuccess > 0) text.replace("*gridGeneration*", QString::number(static_cast<double>(gridGeneration) / static_cast<double>(gridBreedSuccess)));
        else text.replace("*gridGeneration*", QString("Breed success is zero - can't calculate this."));
        text.replace("*gridBreedFails*", QString::number(gridBreedFails));
        text.replace("*gridMeanFitness*", QString::number(gridMeanFitness));
        text.replace("*gridTrophicHistograms*", (QString::number(trophicL01) + "," + QString::number(trophic0102) + "," + QString::number(trophic0203) + "," + QString::number(
                                                     trophic0304) + "," + QString::number(trophic0405) + "," + QString::number(trophic0506) + "," + QString::number(trophic0607) + "," + QString::number(trophic0708) + "," + QString::number(
                                                     trophic0809) + "," + QString::number(trophic0910) + "," + QString::number(trophic1011) + "," + QString::number(trophic1112) + "," + QString::number(trophic1213) + "," + QString::number(
                                                     trophic1314) + "," + QString::number(trophic1415) + "," + QString::number(trophic1516) + "," + QString::number(trophic1617) + "," + QString::number(trophic1718) + "," + QString::number(
                                                     trophic1819) + "," + QString::number(trophic1920) + "," + QString::number(trophic2021) + "," + QString::number(trophic2122) + "," + QString::number(trophic2223) + "," + QString::number(
                                                     trophic2324) + "," + QString::number(trophic2425) + "," + QString::number(trophic2526) + "," + QString::number(trophic2627) + "," + QString::number(trophic2728) + "," + QString::number(
                                                     trophic2829) + "," + QString::number(trophic2930) + "," + QString::number(trophicG30)));
    }
    return text;

}

void LogSimulation::writeLog(QString globalSavePath, int batchRuns, int logType, int iteration)
{
    QString loggingFile = globalSavePath;
    if (!loggingFile.endsWith(QDir::separator())) loggingFile.append(QDir::separator());
    if (logType == LOG_CUSTOM) loggingFile.append(QString(PRODUCTNAME) + "_log");
    if (logType == LOG_FITNESS) loggingFile.append(QString(PRODUCTNAME) + "_fitness");
    if (logType == LOG_RECOMBINATION) loggingFile.append(QString(PRODUCTNAME) + "_recombination");
    if (logType == LOG_DISPARITY) loggingFile.append(QString(PRODUCTNAME) + "_disparity");
    if (logType == LOG_SPECIATION) loggingFile.append(QString(PRODUCTNAME) + "_speciation");
    if (logType == LOG_MUTATION) loggingFile.append(QString(PRODUCTNAME) + "_mutation");
    if (logType == LOG_DUMP_INDIVIDUALS) loggingFile.append(QString(PRODUCTNAME) + "_individuals_data");

    if (batchRuns > -1) loggingFile.append(QString("_run_%1").arg(batchRuns, 4, 10, QChar('0')));
    if (iteration > -1) loggingFile.append(QString("_iteration_%1").arg(iteration, 4, 10, QChar('0')));
    loggingFile.append(".txt");

    QFile outputFile(loggingFile);

    if (!outputFile.exists())
    {
        outputFile.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream out(&outputFile);

        if (logType == LOG_CUSTOM) out << writeCustomLogHeader();
        if (logType == LOG_FITNESS)out << writeFitnessLogHeader();
        if (logType == LOG_RECOMBINATION)out << writeRecombinationLogHeader();
        if (logType == LOG_DISPARITY) out << writeDisparityLogHeader();
        if (logType == LOG_SPECIATION) out << writeSpeciationLogHeader();
        if (logType == LOG_MUTATION) out << writeMutationLogHeader();
        if (logType == LOG_DUMP_INDIVIDUALS) out <<  writeDisparityLogHeader(true);
    }

    outputFile.open(QIODevice::Append | QIODevice::Text);
    QTextStream out(&outputFile);

    if (logType == LOG_CUSTOM) out << writeCustomLog();
    if (logType == LOG_FITNESS)out << writeFitnessLog();
    if (logType == LOG_RECOMBINATION)out << writeRecombinationLog();
    if (logType == LOG_DISPARITY) out << writeDisparityLog();
    if (logType == LOG_SPECIATION) out << writeSpeciationLog();
    if (logType == LOG_MUTATION) out << writeMutationLog();
    if (logType == LOG_DUMP_INDIVIDUALS) out << writeDisparityLog(true);

    outputFile.close();

}

QString LogSimulation::writeCustomLogHeader()
{

    QString headerTextProccessed = processLogTextSpecies(headerText);
    return headerTextProccessed;
};

QString LogSimulation::writeFitnessLogHeader()
{
    QString logString;
    QTextStream out(&logString);

    // Info on simulation setup
    out << printSettings() << "\n";

    //Different versions of output, for reuse as needed
    //out<<"Each iteration lists, for each pixel: mean fitness, entries on breed list";
    //out<<"Each line lists iteration, then the grid's: total critter number, total fitness, total entries on breed list";
    //out<<"Each iteration lists, for each pixel (top left to bottom right): total fitness, number of critters,entries on breed list\n\n";
    if (csvOutput)
        out << "Each iteration lists, for the grid:\nNumber of living critters,Total fitness of all living critters,Number of entries on the breed list";
    else out << "Each iteration lists, for the grid: the number of living critters, then the total fitness of all those critters, and then the number of entries on the breed list.";
    out << "\n";

    return logString;
};

QString LogSimulation::writeRecombinationLogHeader()
{
    QString logString;
    QTextStream out(&logString);

    // Info on simulation setup
    out << printSettings() << "\n";
    out << simulationManager->variableBreedSystem->printDistribution();
    out << "\nNote on log: this only calculates proportions when variable breeding is selected for speed and also currently will only count total breed attempts when the fitness log is also running.\n";

    if (csvOutput) out << "Iteration,Asexual_breeds,Sexual_breeds,Total_breed_attempts,Total_breed_fails,Total_Alive,Percent_sexual\n";
    else out << "Iteration\tAsexual_breeds\tSexual_breeds\tTotal_breed_attempts\tTotal_breed_fails\tTotal_Alive\tPercent_sexual\n";

    if (!simulationManager->cellSettingsMaster->variableBreed)out << "Variable breed not enabled. Will not log.\n\n";

    return logString;
};

QString LogSimulation::writeDisparityLogHeader(bool fullDetails)
{
    QString logString;
    QTextStream out(&logString);
    // Info on simulation setup
    out << printSettings() << "\n";
    out << "This log dumps the genomes of all living individuals.\n\nGenome,X coord,Y coord";
    if (fullDetails)
        out << ",age,environmental fitness,fitness,stolen energy,lifetime energy,species ID,environment R value,environment G value,environment B value";
    out << "\n";

    return logString;
};

QString LogSimulation::writeSpeciationLogHeader()
{
    QString logString;
    QTextStream out(&logString);

    // Info on simulation setup
    out << printSettings() << "\n";
    out << "Iteration number,Number of species,Shared cells,";
    for (int i = 1; i < 6; i++) out << "Species " << i << " ID,Species " << i << " individuals,Species " << i << " cells total,Species " << i << "cells sole species,";
    out << "\n";
    return logString;
};

QString LogSimulation::writeMutationLogHeader()
{
    QString logString;
    QTextStream out(&logString);

    // Info on simulation setup
    out << printSettings() << "\n";
    out << "\nNote on log: this outputs a histogram for the grid showing the number of ones in the non-coding genome which in turn dictates the probability of a mutation occurring.\n"
        "Probabilities are as follows:\n";

    if (csvOutput)  out << "Number of ones,Probability of mutation occurring on breed/duplication,Percentage probability\n";
    else  out << "Number of ones\tProbability of mutation occurring on breed/duplication\n";

    for (int i = 0; i < 32; i++)
    {
        quint32 n = simulationManager->variableMutateSystem->returnCumulativeDistributionAtN(i);
        double percentProb =  ((4294967296. - static_cast<double>(n)) / 4294967296.) * 100.;
        if (csvOutput) out << i << "," << (4294967296 - n) << "/4294967296," << percentProb << "\n";
        else out << i << "\t" << (4294967296 - n) << "/4294967296, or " << percentProb << "%\n";

    }

    if (csvOutput)
    {
        out << "Output\nIteration:,Ones in genome\n";
        for (int i = 0; i < 33; i++) out << i << ",";
    }
    else
    {
        out << "\n\nOutput - iteration:\tOnes in genome:\nIt:\t";
        for (int i = 0; i < 33; i++) out << i << "\t";
    }
    out << "\n";

    return logString;
};

QString LogSimulation::writeCustomLog()
{
    QString out = "";
    if (logIterationText.length() > 2)out.append(processLogTextGeneral(logIterationText));
    if (logSpeciesText.length() > 2)out.append(processLogTextSpecies(logSpeciesText));
    return out;
};

QString LogSimulation::writeFitnessLog()
{
    QString logString;
    QTextStream out(&logString);

    //These are useful in various different formulations of this log
    /*int gridNumberAlive = 0;
    int gridTotalFitness = 0;
    int gridBreedEntries = 0;*/

    for (int i = 0; i < simulationManager->simulationSettings->gridX; i++)
    {
        for (int j = 0; j < simulationManager->simulationSettings->gridY; j++)
        {
            //----RJG: Total fitness per grid square.
            //out<<totalFitness[i][j];

            //----RJG: Number alive per square - output with +1 due to c numbering, zero is one critter, etc.
            //out<<maxUsed[i][j]+1;
            // ---- RJG: Note, however, there is an descendants that when critters die they remain in cell list for iteration
            // ---- RJG: Easiest to account for this by removing those which are dead from alive count, or recounting - rather than dealing with death system
            // int numberalive=0;

            //----RJG: In case mean is ever required:
            //float mean=0;
            // mean = static_cast<float>(totalFitness[i][j])/static_cast<float>(maxUsed[i][j])+1;

            //----RJG: Manually calculate total fitness for grid
            //gridTotalFitness += totalFitness[i][j];

            int critters_alive = 0;

            //----RJG: Manually count number alive thanks to maxUsed descendants
            for (int k = 0; k < simulationManager->cellSettingsMaster->slotsPerSquare; k++)
                if (critters[i][j][k].age > 0)
                {
                    //gridNumberAlive++;
                    critters_alive++;
                }

            //total fitness, number of critters,entries on breed list";
            out << totalFitness[i][j] << " " << critters_alive << " " << breedAttempts[i][j];

            //----RJG: Manually count breed attempts for grid
            //gridBreedEntries += breedAttempts[i][j];

            out << "\n";
        }
    }
    return logString;
}

QString LogSimulation::writeRecombinationLog()
{
    if (!simulationManager->cellSettingsMaster->variableBreed || simulationManager->iteration < 40) return QString("");

    QString logString;
    QTextStream out(&logString);

    out << simulationManager->iteration;
    if (csvOutput) out << ",";
    else out << "\t";

    //RJG count breeding. There may be a better way to do this, but keeping as is for now as not too slow
    int cntAsex = 0;
    int cntSex = 0;
    int totalBreedAttempts = 0;
    int totalBreedFails = 0;

    for (int i = 0; i < simulationManager->simulationSettings->gridX; i++)
        for (int j = 0; j < simulationManager->simulationSettings->gridY; j++)
        {
            for (int c = 0; c < simulationManager->cellSettingsMaster->slotsPerSquare; c++)
                if (critters[i][j][c].age > 0)
                {
                    if (critters[i][j][c].returnRecombination() < 0) cntAsex++;
                    else if (critters[i][j][c].returnRecombination() > 0) cntSex++;
                }
            totalBreedAttempts += breedAttempts[i][j];
            totalBreedFails += breedFails[i][j];
        }

    double percent = -1.;
    if (cntSex > 0) percent = (static_cast<double>(cntSex) / (static_cast<double>(cntAsex) + static_cast<double>(cntSex))) * 100.;
    if (csvOutput) out << cntAsex << "," << cntSex << "," << totalBreedAttempts << "," << totalBreedFails << "," << simulationManager->aliveCount << "," << percent << "\n";
    else out << cntAsex << "\t" << cntSex << "\t" << totalBreedAttempts << "\t" << totalBreedFails << "\t" << simulationManager->aliveCount << "\t" << percent << "\n";

    return logString;
};

QString LogSimulation::writeDisparityLog(bool fullDetails)
{
    QString logString;
    QTextStream out(&logString);

    for (int i = 0; i < simulationManager->simulationSettings->gridX; i++)
        for (int j = 0; j < simulationManager->simulationSettings->gridY; j++)
            for (int k = 0; k < simulationManager->cellSettingsMaster->slotsPerSquare; k++)
                if (critters[i][j][k].age > 0)
                {
                    out << System::returnGenomeString(critters[i][j][k].genomeWords, simulationManager->simulationSettings->genomeSize);
                    out << "," << i << "," << j;
                    if (fullDetails)
                    {
                        quint8 *RGB = simulationManager->env->returnRGB(i, j);
                        out << "," << critters[i][j][k].age << ",";
                        out << critters[i][j][k].environmentalFitness << ",";
                        out << critters[i][j][k].fitness << ",";
                        out << critters[i][j][k].stolenEnergy << ",";
                        out << critters[i][j][k].lifetimeEnergy << ",";
                        out << critters[i][j][k].speciesID << ",";
                        out << RGB[0] << "," <<  RGB[1] << "," <<  RGB[2];
                    }
                    out << "\n";
                }

    return logString;
}

QString LogSimulation::writeGridSpecies()
{
    QString logString;
    QTextStream out(&logString);

    for (int i = 0; i < simulationManager->simulationSettings->gridX; i++)
        for (int j = 0; j < simulationManager->simulationSettings->gridY; j++)
        {
            out << "x" << i << ",y" << j;
            for (int k = 0; k < simulationManager->cellSettingsMaster->slotsPerSquare; k++)
                if (critters[i][j][k].age > 0) out << "," << critters[i][j][k].speciesID;
            out << "\n";
        }
    return logString;
}

QString LogSimulation::writeSpeciationLog()
{
    QString logString;
    QTextStream out(&logString);

    if (speciesMode != SPECIES_MODE_PHYLOGENY_AND_METRICS) out << "Won't log - on wrong species mode. Please select phylogeny and metrics.\n";
    else out << mainWindow->analyser->speciationLogText;

    return logString;
}

QString LogSimulation::writeMutationLog()
{
    QString logString;
    QTextStream out(&logString);

    if (!simulationManager->cellSettingsMaster->variableMutate)
    {
        out << "Variable mutate not enabled. Will not log.\n\n";
        return logString;
    }

    if (csvOutput) out << simulationManager->iteration << ",";
    else out << simulationManager->iteration << ":\t";

    //RJG counts for log - note it goes up to one more than max because need option for zero ones, plus all ones
    QVector <quint32> variableMutateOnes;
    for (int i = 0; i < (simulationManager->variableMutateSystem->returnUseGenomeWordsCount() * 32) + 1; i++)variableMutateOnes.append(0);

    for (int i = 0; i < simulationManager->simulationSettings->gridX; i++)
        for (int j = 0; j < simulationManager->simulationSettings->gridY; j++)
            for (int c = 0; c < simulationManager->cellSettingsMaster->slotsPerSquare; c++)
                if (critters[i][j][c].age > 0) variableMutateOnes[simulationManager->variableMutateSystem->returnBitcount(critters[i][j][c].genomeWords)]++;

    for (int i = 0; i < 33; i++)
    {
        out << variableMutateOnes[i];
        if (csvOutput) out << ",";
        else out << "\t";
    }
    out << "\n";
    return logString;
};

void LogSimulation::writeLogXML(QString globalSavePath)
{
    QString xmlFileString(globalSavePath);
    if (!xmlFileString.endsWith("REvoSim_output/"))xmlFileString.append("REvoSim_output/");
    xmlFileString.append(QString(PRODUCTNAME) + "_log_template.xml");
    QFile xmlFile(xmlFileString);
    if (!xmlFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        mainWindow->setStatusBarText("Error opening XML file to write to.");
        return;
    }

    QXmlStreamWriter xmlFileOut(&xmlFile);
    xmlFileOut.setAutoFormatting(true);
    xmlFileOut.setAutoFormattingIndent(-2);

    xmlFileOut.writeStartDocument();

    xmlFileOut.writeStartElement("revosim_log");

    //Strings
    xmlFileOut.writeStartElement("headerText");
    xmlFileOut.writeCharacters(headerText);
    xmlFileOut.writeEndElement();

    xmlFileOut.writeStartElement("logIterationText");
    xmlFileOut.writeCharacters(logIterationText);
    xmlFileOut.writeEndElement();

    xmlFileOut.writeStartElement("logSpeciesText");
    xmlFileOut.writeCharacters(logSpeciesText);
    xmlFileOut.writeEndElement();

    xmlFileOut.writeEndElement();
    xmlFileOut.writeEndDocument();
    xmlFile.close();

    mainWindow->setStatusBarText("Log XML file saved");
}

void LogSimulation::loadLogXML(QString fileName)
{
    QFile xmlFile(fileName);
    if (!xmlFile.open(QIODevice::ReadOnly))
    {
        qInfo() << "Error opening log XML file. Log may be borked.";
        return;
    }

    QXmlStreamReader xmlFileIn(&xmlFile);

    while (!xmlFileIn.atEnd() && !xmlFileIn.hasError())
    {
        // Read next element.
        QXmlStreamReader::TokenType token = xmlFileIn.readNext();

        //If token is just StartDocument, we'll go to next
        if (token == QXmlStreamReader::StartDocument)
            continue;
        if (token == QXmlStreamReader::StartElement)
        {

            //Strings
            if (xmlFileIn.name().toString() == "headerText")headerText = xmlFileIn.readElementText();
            if (xmlFileIn.name().toString() == "logIterationText")logIterationText = xmlFileIn.readElementText();
            if (xmlFileIn.name().toString() == "logSpeciesText")logSpeciesText = xmlFileIn.readElementText();

        }
    }

    // Error
    if (xmlFileIn.hasError()) qInfo() << "There was an error reading in the log XML file. It may be that not all log elements were loaded.";
    else qInfo() << "Loaded log file successfully, set loggingoutput text as requested. Logging is on.";

    xmlFile.close();
}
