.. _requirements:

Compiling, Installation, and Requirements
==========================================

Installation
------------

Pre-compiled binary releases and packaged installers can be downloaded from the REvoSim GitHub repository. For Windows users we provide both a portable binary release (.zip) - which just needs extracting to a convenient location - and a self contained installer. For Mac we provide a zip containing the REvoSim program that can be downloaded from the [REvoSim GitHub repository](https://github.com/palaeoware/revosim/releases). To install the software, drag and drop the required .app folder(s) into the Applications folder. You may be required to the approve the software in security and privacy settings before it will launch. For Linux users, the instructions below will allow the software to be built using a limited number of lines of bash. Please contact palaeoware@gmail.com if you encounter any issues or would like an app image.

System requirements
-------------------

REvoSim has no minimum requirements as such, and will run on most standard systems (Windows/Linux/Mac); it has not been tested, however, on versions of Windows older than Windows 10, Ubuntu 16.04, and macOS High Sierra. Performance will benefit from high processor speed and increased number of processor cores, with large amounts (>4GB) of available RAM recommended for large simulations. Graphics card performance is not relevant as GPUs are not currently used in the program's calculation pipeline. A fast hard drive (e.g. SSD) is recommend when intensive logging is enabled; as slow I/O response time can affect the iteration cycle speed.

We recommend a minimum of 1GB RAM and a 1.8 GHz or faster, ideally multicore processor. We also recommend a minimum screen resolution of 1280x720 if using the software without the genome comparison docker (and 1920x1080 if this is enabled).

Compiling from Source
----------------------

If you wish to use REvoSim it is generally easiest to use the resources linked above. However, if you wish to develop features, the software can be compiled as follows.

**Windows 64-bit**

*QT Creator + QT > v5.11 using MSYS2 (64-bit) and MinGW (64-bit)*. We recommend you install and use MSYS2 (64-bit) a Windows package manager, based on modern Cygwin (POSIX compatibility layer) and MinGW-w64, that allows easy installation of QT v5.x 64-bit.

#. Download and run the latest version of `MSYS2 <https://www.msys2.org/>`_ for 64-bit Windows. This will be name "mysys2-x86_64-..." for the 64-bit installer.
#. Follow the install instructions. We have used the default install location of "C:\mysys64\" and it is here that includes required in the .pro files point. If you install MSYS2 to another location the .pro files will need to be updated to your install location.
#. Once installed open up MSYS2 shell and run the pacman update command: pacman -Syu Note that as this will almost certainly update pacman itself you may have to close down and restart the MYSYS2 shell before re-running the command to finish.
#. Once MSYS2 and pacman are fully updated run the following command to install QT 5.x and its dependencies: pacman -S mingw-w64-x86_64-qt-creator mingw-w64-x86_64-qt5
#. Optional - if you intend on debugging the software in QT and wish to use GDB then run the following to install the matching GBD debugger: pacman -S mingw-w64-x86_64-gdb
#. At this stage you should have the following under the MSYS2 install location:
    * {install location}/mingw64 (Main ming64 folder)
    * {install location}/mingw64/bin/qmake.exe (QMake for QT version)
    * {install location}/mingw64/bin/g++.exe (C++ complier)
    * {install location}/mingw64/bin/gcc.exe (C complier)
    * {install location}/mingw64/bin/gdb.exe (Debugger | OPTIONAL)
#. You should now be able to find the required libraries under "{install location}/mingw64/bin" and the required header (.h) files for QT v5.x.
#. Open the .pro file in QT Creator, and then use the information above to setup a new 64-bit ming64 kit. Follow standard QT Creator debug/release procedure.

**Ubuntu 18.04/20.04/22.04 64-bit - QT Creator + QT >v5.11 using GCC (64-bit)**

*To compile from command line.*

1. Install GCC and Qt using system packages:

.. code-block:: console

  sudo apt-get install build-essential libgl1-mesa-dev
  sudo apt install qt5-default

2. Download source code and navigate to folder, or alternatively clone using Git:

.. code-block:: console

  git clone https://github.com/palaeoware/revosim.git
  cd revosim

3. Within REvoSim folder create makefile:

.. code-block:: console

  qmake ./revosim.pro

4. Build by running the make command:

.. code-block:: console

  make

5. Navigate to bin folder (e.g. revosim/bin) and launch software by double clicking on file.

On older operating systems, the OS Qt distribution will result in a large number of compile warnings. None impact on the functioning of the software.

*Using Qt creator.*

1. Install Q5.X on your system by running the installer from Qt: https://www.qt.io/download Further instructions are available here: https://wiki.qt.io/Install_Qt_5_on_Ubuntu
2. Download source code, launch Qt Creator, and open the .pro file. Configure build and follow standard debug/release procedure.

**MacOS**

*QT Creator + QT >v5.11*

The above (Linux, using Qt Creator) approach should also work for MacOS builds. This will require xcode to be installed, which you can do using the app store, followed by QtCreator, which can be achieved through the Qt online installer. To build the software, download source code, launch Qt Creator, and open the .pro file. Configure build and follow standard debug/release procedure.
