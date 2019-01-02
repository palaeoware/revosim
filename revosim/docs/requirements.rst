.. _requirements:

Compiling, Installation, and Requirements
==========================================

Compiling from Source
----------------------

**Windows 64-bit**

*QT Creator + QT v5.x using MSYS2 (64-bit) and MinGW (64-bit)*
We recommend you install and use MSYS2 (64-bit) a Windows package manager, based on modern Cygwin (POSIX compatibility layer) and MinGW-w64, that allows easy installation of QT v5.x 64-bit.

#. Download and run the latest version of `MSYS2 <https://www.msys2.org/>`_ for 64-bit Windows. This will be name "mysys2-x86_64-..." for the 64-bit installer.
#. Follow the install instructions. We have used the default install location of "C:\mysys64\" and it is here that includes required in the .pro files point. If you install MSYS2 to another location the .pro files will need to be updated to your install location.
#. Once installed open up MSYS2 shell and run the pacman update command: pacman -Syu Note that as this will almost certainly update pacman itself you may have to close down and restart the MYSYS2 shell before re-running the command to finish.
#. Once MSYS2 and pacman are fully updated run the following command to install QT 5.x and its dependencies: pacman -S mingw-w64-x86_64-qt-creator mingw-w64-x86_64-qt5
#. Optional - if you intend on debugging the software in QT and wish to use GDB then run the following to install the matching GBD debugger: pacman -S mingw-w64-x86_64-gdb
#. At this stage you should have the following under the MYSYS2 install location:
    * {install location}/mingw64 (Main ming64 folder)
    * {install location}/mingw64/bin/qmake.exe (QMake for QT version)
    * {install location}/mingw64/bin/g++.exe (C++ complier)
    * {install location}/mingw64/bin/gcc.exe (C complier)
    * {install location}/mingw64/bin/gdb.exe (Debugger | OPTIONAL)
#. You should now be able to find the required libraries under "{install location}/mingw64/bin" and the required header (.h) files for QT v5.x.
#. Open the .pro file in QT Creator, and then use the information above to setup a new 64-bit ming64 kit. Follow standard QT Creator debug/release procedure.

**Ubuntu 18.04 64-bit**
*QT Creator + QT v5.x and using GCC (64-bit)*

#. The simplest way to install Q5.X on your system is to download and run the installer from `Qt <https://www.qt.io/download>`_. Further instructions are available `from the Qt website <https://wiki.qt.io/Install_Qt_5_on_Ubuntu>`_
#. Open the .pro file in QT Creator, set up an appropriate kit using the installed version of QT, and follow the standard debug/release procedure.

**MacOS**
*QT Creator + QT v5.x*

The above (Linux) approach should also work for MacOS builds.

Installation
------------

From the REvoSim GitHub repository pre-compiled binary releases and packaged installers can be downloaded. For Windows users we provide both a portable binary release (.zip) - which just needs extracting to a convinent location - and a self contained installer.

Requirements
------------

REvoSim has no minimum requirements as such, and will run on most standard systems (Windows/Linux/Mac); it however has not been tested on versions of Windows older than Windows 10, Ubuntu 16.04, and macOS High Sierra. Performance will benefit from high processor speed and increased number of processor cores, with large amounts (>4GB) of available RAM recommended for large simulations. Graphics card performance is not relevant as GPUs are not currently used in the program's calculation pipeline. A fast hard drive (e.g. SSD) is recommend when intensive logging is enabled; as slow I/O response time can affect the iteration cycle speed.

We recommend a minimum of 1GB RAM and a 1.8 GHz or faster, ideally multicore processor. We also recommend a minimum screen resolution of 1280x720 if using the software without the genome comparison docker (and 1920x1080 if this is enabled).
