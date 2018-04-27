Custom Random Numbers
=====================

REvoSim by default comes preloaded with a pre-generated table of 65,536 random numbers 0–255 for randomization during the simulation, both for speed and to avoid potential biases from pseudo-random number generators. 10Mb of quantum-generated random numbers from http://www.randomnumbers.info/ are packaged into the executable; these can be replaced with any other random number file preferred by the user.

To load a custom file of random numbers use the ‘Commands > Load Random Numbers…’ command from the main menu to open a file selection dialog.

[image]

The random number file should be encoded/formatted in/as… [to be added]

Once the desired file is selected press the ‘Open’ button to start importing the new random numbers. Next you will be asked to select the byte offset that REVOSIM will use to read the file from (Figure 11). Note that REVOSIM will always read 65536 bytes; and will throw an error message if it cannot.

[image]

On success a pop-up message reading “New random numbers read successfully” will appear.

[image]

