.. _countpeaks:

Count peaks
===========

A key element of REvoSim is its fitness algorithm, described in depth in the REvoSim paper. The countpeaks command in the REvoSim main menu is included to provide a simple quantification of the fitness landscape given the current masks. These change when the software is restarted, and thus in any given run there will be shifts in the fitness lanscape.

To provide quantification, the count peaks command cycles through every possible 32-bit number, calculating its fitness for the current masks, selected fitness target, and user-defined RGB values (these are requested from the user on initiation of the count peaks command). When this is complete, the software outputs these in a text file written to the output folder (as defined in the Output tab of the Settings dock) that lists fitness *vs* genome count. An example is shown below.

::

  REvoSim Peak Counting 2018-12-17T16:56:57
  ===================

  Below is a histogram showing the different fitnesses for all potential 32-bit organisms in REvoSim under the user-defined RGB levels.

  ===================

  Fitness counts for red=128, green=128, blue=128

  0,3400873943
  1,261711486
  2,206660322
  3,154146136
  4,108257580
  5,71295660
  6,43807340
  7,24949080
  8,13052205
  9,6196135
  10,2627235
  11,976050
  12,310646
  13,82512
  14,18036
  15,2930

The above result is for a typical run, which provides a distribution within those organisms capable of surviving as follows, for red=128, green=128, blue=128:

.. figure:: _static/fitnessSettleTolerance15.png
    :align: center
