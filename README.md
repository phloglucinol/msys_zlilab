Msys is a library for inspecting and manipulating chemical structures of
the sort used in molecular simulations.  Its main features include:

 * Efficient representation of chemical systems and forcefields;

 * A powerful atom selection language;

 * Command line tools for scripting common tasks;

 * C++ and Python interfaces.

Msys currently supports Python 3.6+.  

Building Msys with python 3.10 and numpy 2.2
--------------------------------------------

See in INSTALL.md

1. OpenMM env install
```sh
conda create --name openmm-plumed python=3.10
conda install -c conda-forge openmm
conda activate openmm-plumed
```

2. Installation of dependencies on Ubuntu
```sh
sudo apt install libboost-dev libboost-all-dev
sudo apt install scons
sudo apt install sqlite3 libsqlite3-dev
sudo apt install pybind11-dev
cd _YOUR_MSYS_INSTALL_PATH_/
git clone https://github.com/phloglucinol/msys_zlilab.git
```

3. Compile the msys
```
cd msys
scons -j4
```

4. Download the InChI and  Lpsolve Support
```sh
cd msys/external/lpsolve
wget https://downloads.sourceforge.net/project/lpsolve/lpsolve/5.5.2.5/lp_solve_5.5.2.5_source.tar.gz
tar -zxvf lp_solve_5.5.2.5_source.tar.gz

cd msys/external/inchi
# Download the INCHI-1-SRC.zip in the https://github.com/IUPAC-InChI/InChI/releases
unzip INCHI-1-SRC.zip
```

5. Compile the python wrapper of msys
```sh
scons -j8 PYTHONVER=310 -D MSYS_WITH_INCHI=1 -D MSYS_WITH_LPSOLVE=1
```

- Add PYTHONPATH：
```sh
echo 'export PYTHONPATH=_PATH_INSTALL/msys/build/lib/python:$PYTHONPATH' >> ~/.bashrc
```


Old Building Msys
-----------------

To build Msys, you'll need at a minimum the following:

 * A recent gcc compiler supporting C++11; gcc 5.2 or later should work.

 * Boost libraries (https://www.boost.org) version 1.67 or later.  Earlier versions may also work, but a change in the naming convention for the boost python libraries means you'll have an easier time if you can use 1.67.  Besides the boost headers, msys uses the following boost libraries: filesystem, system, and iostreams.  In addition, you'll almost certainly want to compile the msys python bindings, which use the pybind11 library (https://pybind11.readthedocs.io/en/stable/).

 * The numpy (http://www.numpy.org) headers for each python version you want to support.

 * Scons (https://scons.org), a build tool, available through `pip install`.

 * sconsutils, a set of extensions to Scons.  A copy has been placed in /external; add it to your PYTHONPATH before
 building with:

    export PYTHONPATH=external:$PYTHONPATH


Msys can make use of several optional libraries when they are explicitly
enabled during compilation.  These libraries are:

 * lpsolve (https://sourceforge.net/projects/lpsolve/), needed for bond order and formal charge assigment.

 * The InChI library (https://www.inchi-trust.org/downloads/) version 1.05.

Once you've obtained the necessary dependencies, build msys by running the scons tool at the command line:

    scons -j4

This should build the msys libraries in a `build` subdirectory, placing scripts in `build/bin` and python modules in `build/lib/python`.  If anything fails at this point, check that you have a recent enough compiler, and that the boost headers are installed in a location that is normally searched by the compiler (e.g. /usr/include or /usr/local/include).

At this point, the scripts in `build/bin` aren't really usable since no python bindings have been built.  In order to build the python bindings for msys, run the scons tool again, specifying which python version or versions you want to build for.  For example, to build for Python 3.7, assuming `python3.7` is in your shell's PATH, run::

    scons -j4 PYTHONVER=37

Assuming that works, you should be able to use all the scripts and programs in `build/bin`.  Try it with

    ./build/bin/dms-info tests/files/two.mae

You should get a summary of the structure and forcefield information contained in the file.

The scripts and python extensions you've just built are usable from the root of the msys source directory, but need to be installed before they can be used from other locations.  To install what you've built, specify `install` on the scons command line and add a `PREFIX` argument::

    scons -j4 PYTHONVER=37 install PREFIX=$HOME/local
    export PATH=$HOME/local/bin:$PATH
    export PYTHONPATH=$HOME/local/lib/python:$PYTHONPATH

You should now be able to run the msys scripts and use the python interface.


Building with InChI Support
---------------------------

In order to build msys with support for generation of InChI strings, download version 1.05 of
the InChI source files from here: https://www.inchi-trust.org/downloads/.  The name of the file
will be `INCHI-1-SRC.zip`.  Unzip the file in `external/inchi/`, which should create a directory
named `INCHI-1-SRC` in that location.  If you now build msys with the `MSYS_WITH_INCHI` option,
msys will build the inchi library from source files and link against it:

    scons -j4 PYTHONVER=37 -D MSYS_WITH_INCHI=1

Assuming the build was successful, you can generate an InChI string in msys using the python interface:

    PYTHONPATH=build/lib/python:$PYTHONPATH python

    >>> import msys
    >>> mol = msys.Load("tests/files/stereo.sdf")
    >>> print(msys.InChI(mol).string)
    InChI=1/2C2H2F2/c2*3-1-2-4/h2*1-2H/b2-1+;2-1-


Building with Lpsolve Support
-----------------------------

Msys uses lpsolve in a module that determines chemically reasonable bond orders and formal charges
in systems for which this information is absent, but all atoms are present.  The bond order assigner
is reachable from python through the `msys.AssignBondOrderAndFormalCharge` function, as well as
through the `dms-select` command line tool.

To build msys with lpsolve support, download `lp_solve_5.5.2.5_source.tar.gz` from here:

    https://sourceforge.net/projects/lpsolve/files/lpsolve/5.5.2.5/

Then unpack it into the `external/lpsolve` directory, creating
subdirectory named `lp_solve_5.5` containing the `lpsolve` source files.  Building msys with the
`MSYS_WITH_LPSOLVE` option should now result in the lpsolve library being built from source and
linked into msys, and bond order assignment should be enabled:

    scons -j4 PYTHONVER=37 -D MSYS_WITH_INCHI=1 -D MSYS_WITH_LPSOLVE=1

To test that bond order assignment is available, run the following command:

    ./build/bin/dms-select tests/files/jandor-bad.sdf -o assign.sdf -A

The last few lines of `assign.sdf` should specify formal charges on two atoms:

    ...
    M  CHG  1  30   1
    M  CHG  1  31  -1
    ...



