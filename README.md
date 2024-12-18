# REvoSim and EnviroGen
## [R]apid [Evo]lutionary [Sim]ulator and [Enviro]nment [Gen]erator

Coded by:
 - Mark D. Sutton (m.sutton@imperial.ac.uk)
 - Russell J. Garwood (russell.garwood@gmail.com)
 - Alan R.T. Spencer (alan.spencer@imperial.ac.uk)
 - Euan N. Furness (e.furness19@imperial.ac.uk)

<p align="center">
  <img width="250" height="250" src="./resources/palaeoware_logo_square.png">
</p>

______

## Software references:
Garwood, R.J., Spencer A.R.T. and Sutton, M.D., 2019. REvoSim: Organism-level simulation of macro- and microevolution. Palaeontology 62(3),339-355. [https://doi.org/10.1111/pala.12420](https://doi.org/10.1111/pala.12420)

Furness, E.N., Garwood, R.J. and Sutton, M.D., 2023. REvoSim v3.0.0: A fast evolutionary simulation tool with ecological processes. JOSS  8(89), 5284. [https://doi.org/10.21105/joss.05284](https://doi.org/10.21105/joss.05284)

## Other references:
Furness E.N., Garwood R.J., Mannion P.D. & Sutton M.D. 2021. Evolutionary simulations clarify and reconcile biodiversity-disturbance models. Proceedings of the Royal Society B 288(1949):20210240. [https://doi.org/10.1098/rspb.2021.0240](https://doi.org/10.1098/rspb.2021.0240)

Furness, E.N., Garwood, R.J., Mannion, P. D. & Sutton, M.D. 2021. Productivity, niche availability, species richness and extinction risk: Untangling relationships using individual-based simulations. Ecology and Evolution 11(13): 8923-8940. [https://doi.org/10.1002/ece3.7730](https://doi.org/10.1002/ece3.7730)

Furness, E.N. & Sutton, M.D. 2024. The biological cropping hypothesis over evolutionary time: an experimental test. Theoretical Ecology, 17: 121-129. [https://doi.org/10.1007/s12080-024-00579-3](https://doi.org/10.1007/s12080-024-00579-3)
_____

CONTENTS:

1. Copyright and Licence
2. Introduction
3. Installation
4. Minimum Requirements
5. Documentation
6. Contributing to the code
7. Bug reporting and feature requests

_____

## 1. Copyright and Licence

All REvoSim and utility code (EnviroGen) is released under the GNU General Public License. See LICENSE.md files in the programme directories.

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version. This program is distributed in the hope that it will be useful, but **without any warranty**.

_____

## 2. Introduction

REvoSim is an evolutionary model that aims to bridge the epistemological gap between micro- and macroevolution by providing an organism-level simulation of sufficient computational efficiency that population sizes of 10<sup>5</sup>-10<sup>6</sup> individuals can be studied over geological timescales. The model also incorporates concepts of spatial relationships, spatial and temporal environmental variation, and recombinant reproduction. Isolation of gene-pools (i.e. speciation) is not enforced, but emerges naturally from simulations under appropriate conditions. We intend REvoSim as a multipurpose platform for the study of many evolutionary phenomena; while it was designed with macroevolutionary studies in mind, it is also applicable to microevolutionary problems.

EnviroGen is an environment generator for the REvoSim software, which allows highly customised image stacks to be created using a number of different algorithms.

_____

## 3. Installation

If you wish to use REvoSim and utility packages for research or any other purpose without modifying the software, they can be installed from the binary releases on our [GitHub](https://github.com/palaeoware/revosim/releases/). If you wish to modify the software, you will need to compile them using Qt from the source code as described below.

### Compiling from source

#### Windows 64-bit - Qt Creator + Qt v5.x using MSYS2 (64-bit) and MinGW (64-bit)
We recommend you install and use MSYS2 (64-bit) a Windows package manager, based on modern Cygwin (POSIX compatibility layer) and MinGW-w64, that allows easy installation of Qt v5.x 64-bit.

1. Download and run the latest version of MSYS2 https://www.msys2.org/ for 64-bit Windows. This will be name "mysys2-x86_64-..." for the 64-bit installer.
2. Follow the install instructions. We have used the default install location of "C:\mysys64\" and it is here that includes required in the .pro files point. If you install MSYS2 to another location the .pro files will need to be updated to your install location.
3. Once installed open up MSYS2 shell and run the pacman update command: pacman -Syu Note that as this will almost certainly update pacman itself you may have to close down and restart the MYSYS2 shell before re-running the command to finish.
4. Once MSYS2 and pacman are fully updated run the following command to install Qt 5.x and its dependencies: pacman -S mingw-w64-x86_64-qt-creator mingw-w64-x86_64-qt5
5. (OPTIONAL) If you intend on debugging the software in Qt and wish to use GDB then run the following to install the matching GBD debugger: pacman -S mingw-w64-x86_64-gdb
6. At this stage you should have the following under the MYSYS2 install location:
- {install location}/mingw64 (Main ming64 folder)
- {install location}/mingw64/bin/qmake.exe (QMake for Qt version)
- {install location}/mingw64/bin/g++.exe (C++ complier)
- {install location}/mingw64/bin/gcc.exe (C complier)
- {install location}/mingw64/bin/gdb.exe (Debugger | OPTIONAL)
7. You should now be able to find the required libraries under "{install location}/mingw64/bin" and the required header (.h) files for Qt v5.x under "{install location}/mingw64/include".
8. Download the source code to your desired diectory.
9. Launch Qt creator, select Tools > Options > Kits. If a kit is not available, add a new 64-bit ming64 kit by entering the information listed under point 6 in the appropriate parts of the dailogue.
10. Open revosim.pro, select the newly created kit on launch. Build options (e.g. debug/release, which package) can be selected towards the bottom of the left bar within Qt Creator.

#### Ubuntu 18.04/20.04/22.04 64-bit - Qt v5.x using GCC (64-bit)

To compile from command line. If required, update package lists:

`sudo apt-get update`

Then:

1. Install GCC and Qt using system packages:

`sudo apt-get install build-essential libgl1-mesa-dev`

On Ubuntu <21.04

`sudo apt install qt5-default`

On Ubuntu >= 21.04

`sudo apt-get install qtbase5-dev qtchooser qt5-qmake qtbase5-dev-tools`

2. Download source code and navigate to folder, or alternatively clone using Git:

`git clone https://github.com/palaeoware/revosim.git`

`cd revosim`

3. Within REvoSim folder create makefile:

`qmake ./revosim.pro`

4. Build by running the make command:

`make`

5. Navigate to bin folder (e.g. revosim/revosim/bin) and launch software by double clicking on file.

Note: If you experience a make error that includes e.g. -I../../../anaconda3/include/qt see [issue #9](https://github.com/palaeoware/revosim/issues/9).

Using Qt creator.
1. Install Q5.X on your system by running the installer from Qt: https://www.qt.io/download Further instructions are available here: https://wiki.qt.io/Install_Qt_5_on_Ubuntu
2. Download source code, launch Qt Creator, and open the .pro file. Configure build and follow standard debug/release procedure.

#### MacOS 13 Ventura - QT Creator + QT v5.x using CLANG

These instructions are likely to vary between architectures and different OS versions. Procedure for M1 Apple Silicon Macbook running MacOS 13 Ventura as follows. 
1. Ensure at least ~40Gb of free space for XCode and Qt. 
2. Install XCode, either via Apple Store or direct from Apple developer webpage. This should be done before installing Qt.
3. Install QT using the online installer tool, choosing version 5.15.2.
4. Download source code and navigate to folder, or alternatively clone using Git.
5. Open project in Qt Creator, and ensure Qbuild kit is using Apple Clang x86_64 compiler for C++.
6. Build using Qt Creator

_____

## 4. Minimum Requirements

REvoSim and utility code will run on any modern desktop/laptop: we recommend a minimum of 1GB RAM and a 1.8 GHz or faster, ideally multicore processor. We also recommend a minimum screen resolution of 1280x720 if using the software without the genome comparison docker (and 1920x1080 if this is enabled).

_____

## 5. Documentation

All documentation for REvoSim is online at [http://revosim.readthedocs.io/](http://revosim.readthedocs.io/). This is a version controlled user manual automatically parsed from the .rst found under the docs folders in our software folders. EnviroGen documentation is managed in the same way, and can be found at [http://envirogen.readthedocs.io/](http://envirogen.readthedocs.io/).
_____

## 6. Contributing to the code

Contribution to the code in this repository is welcome. Please read the [Repository Contribution](https://github.com/palaeoware/repoconventions) document for information on our licencing, code of conduct, coding style, and repository structure before submitting any code updates.

_____


## 7. Bug reporting and feature requests

Bug reporting and feature requests should be made through the [GitHub Issues](../../issues) page for this repository. We will aim to respond to all issues and feature requests in a timely manner.

_____


e:palaeoware@gmail.com

w:https://github.com/palaeoware
