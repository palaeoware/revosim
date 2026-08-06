#This command will build the documentation on a system that has python3, sphinx, and the correct theme installed. 
#On Linux that erquires these commands (assuming Python3 is already installed):
#sudo apt-get install python3-sphinx
#sudo apt install python3-pip
#sudo pip3 install sphinx_rtd_theme
echo "Now building revosim doc"
sphinx-build ./ ../docs_build

