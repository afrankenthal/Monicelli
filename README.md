# Monicelli
Fork of the Fermilab's test beam alignment and calibration software (non-github).

### Requirements for installation:

To install Monicelli and Chewie, these are the current dependencies:

- ROOT 6.06.06
- Qt/QtCreator 5.7.1
- Boost default
- XercesC 3.1.3

Check out [setup-6.sh](./setup-6.sh) for where I have these packages on lnx231. Qt 5.7.1 was already installed, as well as Boost. I custom-
installed ROOT and XercesC, but your mileage may vary.

### Installing Monicelli

Once all the dependencies have been installed, git clone this repository and adjust the environment variables in [setup-6.sh](./setup-6.sh)
to match your set-up. After that, source the file and issue qmake, followed by make. This should build Monicelli successfully.

### Monicelli and Chewie tutorials

You can find a tutorial session and slides on how to use Monicelli and Chewie here:

https://indico.cern.ch/event/705870/

### Aligning PROC/PSI46 25x100 sensors:

Due to the odd layout of the 25x100 sensor/chip bump-bonding, a special procedure is required for the alignment step in each run.
In the code [here](src/fileEater.cpp#L623), to run Monicelli on a given 25x100 run for the first time, those lines need to be un-commented.
This is necessary in order to get the best possible alignment, using only small pixels. Using large pixels will reduce our alignment
capability. After you've gone through Monicelli a first time and saved a .geo file with the right alignment and calibration, then comment
these lines out again and re-run Monicelli to produce all the tracks and residuals including the large-area pixels. Make sure to input the 
.geo file just produced. Those pixels are required in Chewie to calculate the efficiency correctly.

