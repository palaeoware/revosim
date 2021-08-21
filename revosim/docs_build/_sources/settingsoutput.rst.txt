.. _outputs:

Configuring your Outputs
========================

REvoSim has multiple output options that allow runs to be recorded and analysed as required for any given study. These are controlled from the Output tab of the settings dock. Options are as follows:

:Output save path: This is the folder into which all outputs from REvoSim (logs, images, any other files) are saved. For consistency these are all placed within a newly created folder called *REvoSim_output*. Text files are placed within the root of this folder, and images are placed within their own folder within *REvoSim_output*.

:Refresh/polling rate: This is the frequency (in iterations) that the simulation is polled. Polling includes running the full clustering analysis intergral to the thorough species-identification algorithm (see RevoSim paper), writing the in-simulation log, writing any requested image files to disk, and updating the GUI (nless dsisabled). Because the species identification system has a significant computational overhad, if species tracking is on, frequent polling will significantly slow the simulation.

:Logging Population/Environment: Any of the simulation visualisations during a run can be saved as an image (.png stacks labelled by iteration number). The tick boxes in this part of the tab dictate which images are saved.

:Write Log Files: When this option is checked a log file is written during the course of every run. See :ref:`logging` for more details of REvoSim logs.

:Log file formatted as CSV: When this option is checked the log file is converted to comma-separated value format, with one line per species entry. This log-format is less easily read by a human, but may be easier to import into software for analysis. If 'Write Log Files' is unchecked, this option is disabled (greyed out).

:Gene Frequencies in Log: When this option is checked extra data is added to the log file, detailing the frequencies of each gene for each species, the changes in gene frequencies, and the total numbers of bit changes in the modal genome. If 'Write Log Files' is unchecked, this option is disabled (greyed out).

:Automatically create detailed log on batch runs: This option outputs the detailed log at the end of each run when REvoSim is operating in batch mode. See :ref:`logging` for more details of REvoSim logs.

:Write data for current run: This option outputs the detailed log for the currently running simulation at the point at which the button is pressed. See :ref:`logging` for more details of REvoSim logs.

:Exclude species without descendents: Under most settings a significant number of small, short-lived species may appear regularly within a REvoSim run. Given the significant amount of data REvoSim can generate, and the fact that these short lived species will be unimportant for many studies (potentially masking important obvservations), this option rationalises REvoSim detailed logs by only including species with descendents in the end run log and tree.

:Minimum species size: It is also possible to filter the species data in the log files so that only species above a certain number of individuals are included in the logs. This spin box dictates what that minimum cut-off is.

:Don't update GUI: This option allows runs to proceed without updating the GUI (although this prevents images being saved through a run). This allow REvoSim to run marginally faster, and may be of utility for very long runs.
