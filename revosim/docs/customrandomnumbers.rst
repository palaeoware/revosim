.. Below allows arrows to be included - needs to be above page label in newer versions of sphinx
.. include:: <isonum.txt>

.. _customrandomnumbers:

Custom Random Numbers
=====================

As of version 3.0.0, REvoSim employs QT's random number library QRandomGenerator to create its random numbers. Specifically, it uses a QRandomGenerator::global() that is securely seeded. More information about this library can be found at in the Qt documentation: `QRandomGenerator class <https://doc.qt.io/qt-5/qrandomgenerator.html>`_. Earlier versions of REvoSim employed a pre-generated table of 65,536 random numbers 0-255 sourced from 10Mb of quantum-generated random numbers from `randomnumbers.info <http://www.randomnumbers.info>`_ that were packaged into the executable. REvoSim still offers the option to employ this approach via the option to load a custom random numbers file if preferred by the user. 

To load a custom file of random numbers use the 'Commands |rarr| Load Random Numbers…' command from the main menu to open a file selection dialogue.

.. figure:: _static/randomNumberLoad.png
    :align: center

    Custom Random Number file open dialogue.

The random number file should be encoded as a random binary string, and should be a minimum of 65536 bytes. Once the desired file is selected press the 'Open' button to import the new random numbers. REvoSim will then ask for a byte offset to read the file from (thus allowing runs to be repeated with the same random numbers, if desired).

.. figure:: _static/randomNumberByteOffset.png
    :align: center

    Custom Random Number byte offset form.

Note that REVOSIM will always read 65536 bytes; and will throw an error message if it cannot.

.. figure:: _static/randomNumberError.png
    :align: center

    Custom Random Number error on load message.

On success a pop-up message reading “New random numbers read successfully” will appear.
