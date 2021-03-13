#! /usr/bin/env bash
echo "Warning: deleting both build and generated python interface"
echo "Confirm with [y] to continue:"
#read response
#! [[ "$response" == "y" ]] && exit
echo "Deleting..."
rm -drf build/*
rm -drf generated/*
echo "--> [x] Completed."
echo "Press any key to continue."
#read placeholder

