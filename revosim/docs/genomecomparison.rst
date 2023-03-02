.. _genomecomparison:

Genome comparison dock
======================

The Genome Comparison Dock allows genomes to be inspected and compared within RevoSim, and can be launched from the main menu or the toolbar. By default it appears landscape on the bottom of the main window. Its landscape orientation is intended to allow whole genomes to be viewed without scrolling on most operating systems, however we note that on operating systems with low resolution displays, the REvoSim GUI may struggle to accommodate the docker in its default position: the environment and population views may be forced to be small, or the genome comparison docker may not have the vertical extent to show more than one genome. If this is the case, the docker can be floated, or alternatively docked to the right or left of the main window.

.. raw:: html

	<div class="figure align-center">
	<video width="695" height="485" loop autoplay controls muted>
    <source src="_static/genomeComparisonDock.mp4" type="video/mp4">
    Your browser does not support the video tag.
    </video>
	<p class="caption">Genome comparison dock in use (created with v2.0.0).</p>
	</div>

A genome can be added to the comparison dock by right clicking on the population scene. By default, this will add the genomes of all of the digital organisms living within that pixel to the comparison dock, rendering them as zeros and ones, as well as displaying, from left to right in the row, the colour of that organism's environment, that organism's species ID, the colour assigned to that species ID, that organism's environmental fitness (EF) in its environment, that organism's current fitness (F) (after interactions), the lifetime total energy accumulated by that organism (Le), and the total energy acquired by that organism through direct energy theft in interactions (Se). A colour is also displayed for each genome word, to highlight differences between genome words among organisms. Alternatively, as shown in the video above, when Modal genome only is selected on the right of the dock, this adds just the model genome for any clicked pixel. By default, auto compare is also enabled: this compares and highlights the differences between each entry on the genome list and that before it.

The genomes can be deleted individually from the table by selecting a genome and using the Delete button. Alternatively all genomes in the table can be removed from Genome Comparison Dock the using the Reset All button.

The first ten genomes from the Genome Comparison Dock are pulled through to the reseed dialogue (shown in figure below): this allows one to be selected as the starting genome for a new run. This allows multiple repeats of simulations using the same genome, which is known to be capable of surviving within a given environment using the masks loaded at launch (note that these will not persist between sessions unless through loading a saved simulation).

.. figure:: _static/reseedDialog.png
    :align: center

    Reseed dialog showing first ten genomes as selected in the Genome Comparison Dock (created with v2.0.0).
