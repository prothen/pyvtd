#!/usr/bin/env bash
#
# Initialise python source code to pythonpath and make it accessible as package.
#
# Author: Philipp Rothenhäusler, Stockholm 2020
#

CWD="$(readlink -m "$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd )")"
TBA=()
TBA_pyvtd_ROOT_DIR="export PYVTD_DIR=${CWD}"
echo -e "Expose PYVTD_DIR to environment\n\t $TBA_pyvtd_ROOT_DIR"
TBA+=("$TBA_pyvtd_ROOT_DIR")

TBA_pyvtd_PYTHON_DIR="export PYTHONPATH=${CWD}:\$PYTHONPATH"
echo -e "Expose to PYTHONPATH\n\t $TBA_pyvtd_PYTHON_DIR"
TBA+=("$TBA_pyvtd_PYTHON_DIR")

for i in "${TBA[@]}"
do
    if grep -Fxq "$i" ~/.profile; then echo -e "\nSkip entry: \n $i";
    else echo "$i" >> ~/.profile; echo -e  "\nAdded entry: \n $i"; fi
done

echo -e "\n--> [x] Completed. (Source ~/.profile or restart your session to activate.)"
echo "Press any key to continue"
read placeholder
