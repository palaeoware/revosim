.. REvoSim documentation master file, created by
   sphinx-quickstart on Thu Apr  5 14:07:27 2018.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

Welcome to REvoSim's User Manual
================================

The [R]apid [Evo]lutionary [Sim]ulator program.

REvoSim uses a simplified first-principles evolutionary model to facilitate high computational efficiency, enabling the simulation of large populations incorporating space, over geological time, using modest computer hardware. Speeds attainable depend on the computer hardware in use, the size of the populations simulated, and details of the experimental setup (most especially on whether species tracking and fitness recalculation are activated). With a typical 2018 desktop computer, speeds of between 500,000 and 1,000,000 iterations (years) per hour can be achieved for populations of around 250,000.

.. figure:: _static/palaeoware_logo_square.png
    :align: center

t:@palaeoware

w:https://github.com/palaeoware.

Relevant references
-------------------

Garwood, R.J., Spencer A.R.T. and Sutton, M.D., 2018. REvoSim: Organism-level simulation of macro- and microevolution


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
