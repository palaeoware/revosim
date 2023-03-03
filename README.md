# REvoSim and EnviroGen
## [R]apid [Evo]lutionary [Sin]ulator and [Enviro]nment [Gen]rator


Coded by:
 - Mark Sutton (m.sutton@imperial.ac.uk)
 - Russell J. Garwood (russell.garwood@gmail.com)
 - Alan R.T. Spencer (alan.spencer@imperial.ac.uk)

<p align="center">
  <img width="250" height="250" src="./resources/palaeoware_logo_square.png">
</p>

______

## Software references:
Garwood, R.J., Spencer A.R.T. and Sutton, M.D., 2019. REvoSim: Organism-level simulation of macro- and microevolution. Palaeontology 62(3),339-355. [https://doi.org/10.1111/pala.12420](https://doi.org/10.1111/pala.12420)

Furness, E.N., Garwood, R.J. and Sutton, M.D., 2023. REvoSim v3.0.0: A fast evolutionary simulation tool with ecological processes. JOSS. Submitted.

## Other references:
Furness E.N., Garwood R.J., Mannion P.D. & Sutton M.D. 2021. Evolutionary simulations clarify and reconcile biodiversity-disturbance models. Proceedings of the Royal Society B 288(1949):20210240. doi: 10.1098/rspb.2021.0240

Furness, E.N., Garwood, R.J., Mannion, P. D. & Sutton, M.D. 2021. Productivity, niche availability, species richness and extinction risk: Untangling relationships using individual-based simulations. Ecology and Evolution 11(13): 8923-8940. doi: 10.1002/ece3.7730

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

REvoSim and utility code may be compiled using QT from the source code or installed from the binary releases on our [GitHub](https://github.com/palaeoware).

### Compiling from source

#### Windows 64-bit - QT Creator + QT v5.x using MSYS2 (64-bit) and MinGW (64-bit)
We recommend you install and use MSYS2 (64-bit) a Windows package manager, based on modern Cygwin (POSIX compatibility layer) and MinGW-w64, that allows easy installation of QT v5.x 64-bit.

1. Download and run the latest version of MSYS2 https://www.msys2.org/ for 64-bit Windows. This will be name "mysys2-x86_64-..." for the 64-bit installer.
2. Follow the install instructions. We have used the default install location of "C:\mysys64\" and it is here that includes required in the .pro files point. If you install MSYS2 to another location the .pro files will need to be updated to your install location.
3. Once installed open up MSYS2 shell and run the pacman update command: pacman -Syu Note that as this will almost certainly update pacman itself you may have to close down and restart the MYSYS2 shell before re-running the command to finish.
4. Once MSYS2 and pacman are fully updated run the following command to install QT 5.x and its dependencies: pacman -S mingw-w64-x86_64-qt-creator mingw-w64-x86_64-qt5
5. (OPTIONAL) If you intend on debugging the software in QT and wish to use GDB then run the following to install the matching GBD debugger: pacman -S mingw-w64-x86_64-gdb
6. At this stage you should have the following under the MYSYS2 install location:
- {install location}/mingw64 (Main ming64 folder)
- {install location}/mingw64/bin/qmake.exe (QMake for QT version)
- {install location}/mingw64/bin/g++.exe (C++ complier)
- {install location}/mingw64/bin/gcc.exe (C complier)
- {install location}/mingw64/bin/gdb.exe (Debugger | OPTIONAL)
7. You should now be able to find the required libraries under "{install location}/mingw64/bin" and the required header (.h) files for QT v5.x under "{install location}/mingw64/include".
8. Download the source code, and use the information above to setup a new 64-bit ming64 kit under QT creator and follow standard QT Creator debug/release procedure.

#### Ubuntu 18.04/20.04/22.04 64-bit - QT Creator + QT v5.x using GCC (64-bit)

To compile from command line.
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

Using Qt creator.
1. Install Q5.X on your system by running the installer from Qt: https://www.qt.io/download Further instructions are available here: https://wiki.qt.io/Install_Qt_5_on_Ubuntu
2. Download source code, launch Qt Creator, and open the .pro file. Configure build and follow standard debug/release procedure.

_____

## 4. Minimum Requirements

REvoSim and utility code will run on any modern desktop/laptop: we recommend a minimum of 1GB RAM and a 1.8 GHz or faster, ideally multicore processor. We also recommend a minimum screen resolution of 1280x720 if using the software without the genome comparison docker (and 1920x1080 if this is enabled).

_____

## 5. Documentation

All documentation for REvoSim is online at [http://revosim.readthedocs.io/](http://revosim.readthedocs.io/). This is a version controlled user manual automatically parsed from the .rst found under the docs folders in our software folders. EnviroGen documentation is managed in the same way, and can be found at [http://envirogen.readthedocs.io/]((http://revosim.readthedocs.io/).
_____

## 6. Contributing to the code

Contribution to the code in this repository is welcome. Please read the [Repository Contribution](https://github.com/palaeoware/repoconventions)  document for a guide to our coding style and repository structure before submitting any code updates.

_____


## 7. Bug reporting and feature requests

Bug reporting and feature requests should be made through the [GitHub Issues](../../issues) page for this repository. We will aim to respond to all issues and feature requests in a timely manner.

_____


t:@palaeoware

e:palaeoware@gmail.com

w:https://github.com/palaeoware
