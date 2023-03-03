# REVoSim and EnviroGen - Inno Setup
## [R]apid [EVO]lutionary [SIM]ulator and [Enviro]nment [Gen]rator


Coded by:
 - Mark Sutton (m.sutton@imperial.ac.uk)
 - Russell J. Garwood (russell.garwood@gmail.com)
 - Alan R.T. Spencer (alan.spencer@imperial.ac.uk)

______

## Relevant references:
Garwood, R.J., Spencer A.R.T. and Sutton, M.D., 2019. REvoSim: Organism-level simulation of macro- and microevolution. Palaeontology 62(3),339-355. [https://doi.org/10.1111/pala.12420](https://doi.org/10.1111/pala.12420)
_____

CONTENTS:

1. Creating Windows Installer Program - Inno Setup

_____

## 1. Creating Windows Installer Program - Inno Setup

Inno Setup is a free installer for Windows programs by Jordan Russell and Martijn Laan. First introduced in 1997, Inno Setup today rivals and even surpasses many commercial installers in feature set and stability.

The program can be downloaded from: [http://jrsoftware.org/isinfo.php](http://jrsoftware.org/isinfo.php)

The file REVOSIMinstaller.iss contained in this folder should be modified and used to create the Windows Installation Binary. It expects all support .DLLs and the .EXE to be placed in a ./bin folder. The Inno Setup will create the new install binary in a folder called ./build

The REVOSIMinstaller.iss should be altered with the latest version number etc... before being run.

_____


t:@palaeoware

e:palaeoware@gmail.com

w:https://github.com/palaeoware
