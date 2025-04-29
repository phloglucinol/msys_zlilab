OpenMM env install
```
conda create --name openmm-plumed python=3.10
conda install -c conda-forge openmm
conda activate openmm-plumed
```

For Ubuntu 
```sh
sudo apt install libboost-dev libboost-all-dev
sudo apt install scons
sudo apt install sqlite3 libsqlite3-dev
sudo apt install pybind11-dev
cd _PATH_INSTALL/
git clone https://github.com/DEShawResearch/msys.git
```

Compile the msys
```
cd msys
scons -j4
``` 

Download the InChI and  Lpsolve Support
```
cd msys/external/lpsolve
wget https://downloads.sourceforge.net/project/lpsolve/lpsolve/5.5.2.5/lp_solve_5.5.2.5_source.tar.gz
tar -zxvf lp_solve_5.5.2.5_source.tar.gz

cd msys/external/inchi
# Download the INCHI-1-SRC.zip in the https://github.com/IUPAC-InChI/InChI/releases
unzip INCHI-1-SRC.zip
```

Compile the python wrapper of msys
```
scons -j8 PYTHONVER=310 -D MSYS_WITH_INCHI=1 -D MSYS_WITH_LPSOLVE=1
```

Add PYTHONPATH：
```
echo 'export PYTHONPATH=_PATH_INSTALL/msys/build/lib/python:$PYTHONPATH' >> ~/.bashrc
```

