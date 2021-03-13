# pyvtd
This repository aims to provide an easily maintainable Python interface for the interaction with the Virtual Test Drive Simulator.

For this a `C++` based networking interface converts received packages into messages and shares them in form of SWIG wrappers to the Python interface.

This allows both convenient development in a pure Python environment while providing the ability to implement real time sensitive code directly in `C++`.


## Overview
- [Installation](#installation)
    - Initial setup instructions for the Python module and preparation of the build process
- [`C++` interface compilation](#cpp-interface)
    - How to build the Python interface
- [C++ interface creation]()
    - pending instructions
- [Python interface creation]()
    - pending instructions
- [FAQ](./docs/extended/FAQ.md)
    - A list of helpful common problems

## Installation
This repository provides two installation methods.
Namely, the static installation of python libraries into the system path with `setup.py` and the installation with exposing the Python library to the `PYTHONPATH`.
However, currently only the latter is supported, which allows to update source code in the local directory without repeated re-installation of the Python library.

For the latter development oriented installation execute in the root directory of this repository
```bash
./init.sh
```

_Note: This exposes the current working directory to your `.profile` and will extend your PYTHONPATH. In order for this to take effect the interactive session has to be refreshed. This can be done the easiest by simply rebooting._

## Cpp Interface
Under `./resources/` find the script files `{make,regen,setup,clean}.sh` further described in the subsequent sections.

Before using the automatic generation framework, you need to add the following files receivable from Vires (see `resources/.gitignore`):
```bash
include/RDBHandler.cc
include/RDBHandler.hh
include/scpIcd.h
include/viRDBIcd.h
```

_Note: It is necessary to execute the `./init.sh` to expose necessary environment variables._

### Usage
In order to build the SWIG python `base` module execute

```bash
./make.sh
```

This implicitly tests the module, for manual testing you can also execute after successful compilation and linkage (`make.sh`) the following command.

```bash
python2 -c import vtd.generated.base
```

You can clean any build artifacts (including generated python interface) using
```bash
./clean.sh
```

and for repetitive development cycles you can simply execute both `clean.sh` and `make.sh` in one command with
```bash
./regen.sh
```

### Tests
After completed successful build process you can try executing the test Python scripts under `./tests`.

## Contributions
If you encounter challenges or you have general questions about the repository or you would like to see more detailed instructions for specfic sections, don't hesitate to create a pull request.


For code contributions from forks, use the `Google C++ Style Guide` for C++ based contributions and the common PEP for Python contributions.
These, are however barely encouragements and the repository is partly based on code sections which adhered to different coding styles.


## LICENSE
All rights are reserved. If you are interested in licensing details or having an open source license please let me know.
