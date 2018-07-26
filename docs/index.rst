.. REvoSim documentation master file, created by
   sphinx-quickstart on Thu Apr  5 14:07:27 2018.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

Welcome to REvoSim's User Manual
================================

The [R]apid [Evo]lutionary [Sim]ulator program.

REvoSim is an individual-based model, using a simplified first-principles evolutionary model to facilitate high computational efficiency, enabling the simulation of large populations incorporating space, over geological time, using modest computer hardware. It can simulate populations of 10\ :sup:`5` \–10\ :sup:`7` digital organisms over geological timescales, and incorporates spatial and temporal environmental variation, recombinant reproduction, mutation and dispersal. Speeds attainable depend on the computer hardware in use, the size of the populations simulated, and details of the experimental setup (most especially on whether species tracking and fitness recalculation are activated). With a typical 2018 desktop computer, speeds of between 500,000 and 1,000,000 iterations per hour can be achieved for populations of around 250,000.

REvoSim has been in development since 2008, and has been released with the intention that it can be used as a multipurpose platform for the study of many evolutionary phenomena (both macro- and microevolution). While it was designed with macroevolutionary studies in mind, it is also applicable to microevolutionary problems. As such it is complementary to the many other approaches of studying evolution on a range of different timescales, and will be continually developed by the core team to expand its capabilities.

.. figure:: _static/palaeoware_logo_square.png
    :align: center

t:@palaeoware

w:https://github.com/palaeoware.

Relevant references
-------------------

Garwood, R.J., Spencer A.R.T. and Sutton, M.D., 2018. REvoSim: Organism-level simulation of macro- and microevolution.


Table of Contents
=================
.. toctree::
   :maxdepth: 3
   :numbered:
   
   introduction
   requirements
   thebasics
   runfirstsim
   runforandbatch
   settingsorganisms
   settingssiumlation
   loggingsim
   advanced
