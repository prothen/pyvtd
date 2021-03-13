#! /usr/bin/env bash
#
# Automate build for simple swig interface
#
# Author: Philipp Rothenhäusler, Stockholm 2020
#

[ -z ${PYVTD_DIR} ] && echo "Initialise repository with ./init.sh (in root of repository)"  && read placeholder && exit

if [ ! -f .gitignore ]; then
    echo "No .gitignore detected, generating..."
    touch .gitignore
    echo "build" >> .gitignore
    echo "generated" >> .gitignore
    echo "!build/.gitkeep" >> .gitignore
    echo "!generated/.gitkeep" >> .gitignore
    echo "--> [x] completed generation of .gitignore."
fi
echo "Checking for folder structure..."
[ ! -d build ] && mkdir build && touch build/.gitkeep
[ ! -d generated ] && mkdir generated && touch generated/.gitkeep
echo "--> [x] Completed folder structure checks."

CW="$(readlink -m "$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd )")"
[ ! -d ${CW}/build ] && mkdir ${CW}/build
# --------------- Generate swig python interface
echo "Building swig interface..."
swig -python -outdir generated -o build/swig_wrapper.cpp \
    -I${CW}/include \
    -I${CW}/src \
    -c++ ${CW}/interface.i
echo "--> [x] completed swig interface."
# Generate swig cpp interface with includes
cd build
g++ -c  -fpic swig_wrapper.cpp \
    -I/usr/include/python2.7 \
    -I${CW}/include \
    -I${CW}/src
# Generate object for RDBHandler
g++ -c -fpic ${CW}/include/RDBHandler.cc -I${CW}/include
# Generate object for tcp_interface
#g++ -c -fpic ${CW}/src/tcp_interface.cpp -I${CW}/include
# Generate object for rdb_interface
g++ -c -fpic  \
    ${CW}/src/{tcp,rdb,scp}_interface.cpp \
    -I${CW}/build \
    -I${CW}/src \
    -I${CW}/include
# Generate object for scp_interface
#g++ -c -fpic ${CW}/src/scp_interface.cpp -I${CW}/include
# Link them all
# TODO: _base is hardcoded on line 1 in interface.i
g++ -fpic -v \
    swig_wrapper.o \
    RDBHandler.o \
    {tcp,rdb,scp}_interface.o \
    -shared -o ${CW}/generated/_base.so
echo "Copying into python package root..."
echo "--> (Copy to ${PYVTD_DIR})"
cd ..
cp -rf generated/* ${PYVTD_DIR}/vtd/generated
echo "--> [x] completed copying."
echo "Skip python tests for now. Please see manual instructions in README.md."
#echo "Testing the generated interface..."
#cd ${PYVTD_DIR}/vtd/generated
#python2 -c "import vtd.generated.base;">result 2>&1
#if [ -z "$(cat ./result)" ]; then
#    echo "--> [x] Successful. "
#else
#    echo "$(cat ./result)"
#    echo "--> [ ] Unsuccessful. (see above print) "
#fi
#rm ./result
echo "Press any button to continue"
read placeholder
exit

# --------------- Build shared libraries
# TODO: either glob all include and src files or manually required to invoke subsequent commands
# see following examples
echo "Completed auto generated wrapper. Compiling now."
cd build && g++ -c -fpic rdb_receiver_interface_wrapper.cpp -I/usr/include/python2.7 -I${CW}/include -I${CW}/rdb_cpp_interface
echo "Generated wrapper cpp source, generate dependent shared libraries now."
g++ -c -fpic ${CW}/include/RDBHandler.cc -I${CW}/include
echo "Generated RDBHandler.o"
g++ -c -fpic ${CW}/rdb_cpp_interface/rdb_receive.cpp -I${CW}/src -I${CW}/include
echo "Compiled custom interface, generate dependent shared libraries now."
g++ rdb_receiver_interface_wrapper.o rdb_receive.o RDBHandler.o -shared -o ${CW}/rdb_python_interface/_rdb_receiver_interface.so
echo "Generated targeted shared object."
# --------------- RDB_CONSOLE GENERATION
