# ThermoFun

A code for calculating the standard state thermodynamic properties at a given temperature and pressure.

## Build and Run Tests on Linux

## Prepare building tools

* Make sure you have g++, cmake and git installed. If not, install them (on Ubuntu Linux):

```
#!bash
sudo apt-get install g++ cmake git
```

## Prepare folder structure and download ThermoFun source code

* In a terminal, at the home directory level e.g. ```<user>@ubuntu:~$``` copy-paste and run the following code:

```
#!bash
mkdir gitTHERMOFUN && cd gitTHERMOFUN && \
mkdir build && cd build && \
mkdir release && mkdir debug && cd .. && \
git clone https://bitbucket.org/gems4/thermofun.git && cd thermofun
```

* In the terminal you should be in ```~/gitTHERMOFUN/thermofun$``` and the directory structure should now look like this:

```
~/gitTHERMOFUN
    /build
        /debug
        /release
    /thermofun
```

## Install ArangoDB local

* For using ThermoFun with a local arangodb client. If only the remote db.thermohub.net database will be used, the installation of a local arangodb client is not necessary. Proceed to the next section Install Dependencies
* In a terminal copy-paste and run to flowing code: (for possibly newer versions of arangodb check [click here](https://www.arangodb.com/download-major/ubuntu/). In the arangodb packedge configuration we recommend to leave ```root``` password empty, and click enter for the following questions, using default selections. For Backup database files before upgrading select "Yes".

```
#!bash
sudo apt-get install curl && \
curl -OL https://download.arangodb.com/arangodb33/xUbuntu_17.04/Release.key && \
sudo apt-key add - < Release.key && \
echo 'deb https://download.arangodb.com/arangodb33/xUbuntu_17.04/ /' | sudo tee /etc/apt/sources.list.d/arangodb.list && \
sudo apt-get install apt-transport-https && \
sudo apt-get update && \
sudo apt-get install arangodb3=3.3.19
```

## Build and install ThermoFun library 

Buil and ThermoFun library for use in C++ or python thirdparty codes.

### Install Dependencies

Installing dependencies needed to build ThermoFun on (k)ubuntu linux 16.04 or 18.04, in a terminal ```~/gitTHERMOFUN/thermofun$``` execute the following: 

```
#!bash
sudo ./install-dependencies.sh
```

### Compiling the C++ library

In the terminal ```~/gitTHERMOFUN/thermofun$```, execute the following commands:

```
#!bash
cd ../build/release && \
cmake ../../thermofun && \
make
``` 

To take advantage of parallel compilation use ```make -j3```. 3 representing the number of threads. 

For a global installation of the compiled libraries in your system, execute:

```
#!bash
sudo make install 
```

This will install Thermofun library and header files in the default installation directory of your system (e.g, ```/usr/local/``` ).

For a local installation, you can specify a directory path for the installed files as follows:

```
#!bash
cmake ../../thermofun -DCMAKE_INSTALL_PREFIX=/home/username/local/
```
then execute:

```
sudo make install 
```

To compile ThermoFun library in debug mode change directory to ```~/gitTHERMOFUN/build/debug``` and:

```
#!bash
cmake ../../thermofun/ -DCMAKE_BUILD_TYPE=Debug
```
then execute:

```
sudo make install 
```

### Build and run ThermoFun GUI Demo

To be able to build and run the ThemroFun GUI (graphical user interface) application demo, Qt needs to be installed.

* Download and install Qt 5.11.0 (https://www1.qt.io/download/) in your home directory ```~/Qt```. In the "Select components to install" menu select: Qt 5.11.0 with Desktop gcc 64-bit, Qt Charts, and Qt WebEngine

### Install Dependencies

Installing dependencies needed to build ThermoFun on (k)ubuntu linux 16.04 or 18.04, in the terminal go in ```~/gitTHERMOFUN/thermofun$``` and execute the following: 

```
#!bash
sudo ./install-dependencies-gui.sh $HOME/Qt/5.11.0/gcc_64
```

This step will download, configure, build, and install all dependencies: `lua5.3-dev`, `libboost-all-dev`, `libcurl4-openssl-dev`, `libboost-test-dev`, `automake`, `flex`, `bison`, `libssl-dev`, `pugixml`, `yaml-cpp`,  `thrift`, `velocypack`, `jsonio`, `jsonimpex`, `jsonui`. The script will check if the dependencies are not already present at the defalut instalation path ```/usr/local/``` and will only install them if not found. 
To reinstall dependencies open `install-dependencies.sh` and/or `install-dependencies-gui.sh` files with a text editor. At the beginning of the script file commands for removing dependency library files are present but they are commented out with `#` symbol. Remove `#` for each dependency you wish to be reinstalled. 

### Compiling the ThermoFun GUI demo

In the terminal at ```~/gitTHERMOFUN/thermofun$```, execute the following commands:

```
#!bash
cd .. && mkdir build-gui \
cd build-gui && mkdir release && cd release && \
cmake ../../thermofun/fungui -DBUILD_FUNGUI=ON -DBUILD_DEMO=ON -DCMAKE_PREFIX_PATH=$HOME/Qt/5.11.0/gcc_64 && \
make
``` 

The build script will also copy into the build folder the necessary /Resources folder. In the Resources folder a file named "ThermoFun-config.json" is present and contains the arangodb database connection preferences. 

To run the ThermoFun GUI demo in the terminal at ```~/gitTHERMOFUN/build-gui$``` execute:

```
./guidemo.sh
```

* For building using Qt Creator, use the ThermoFunDemoGUI.pro project file found in  ```~/gitTHERMOFUN/thermofun/fungui```.


### Simple API example

* Using a json database file

```
#!c++
int main()
{
   // Create the interface object using a database file in JSON
   Interface interface("aq17.json");

   // Optional: set the solvent symbol used for calculating properties of aqueous species
   interface.setSolventSymbol("H2O@");

   // Optional: change default units
   interface.setPropertiesUnits({"temperature", "pressure"},{"degC","bar"});

   // Optional: change default digits
   interface.setPropertiesDigits({"gibbs_energy","entropy", "volume", "enthalpy", "temperature", "pressure"}, {0, 1, 2, 0, 0, 0});

   // Retrieve the entropy of H2O
   double H2Oentropy = interface.thermoPropertiesSubstance( 300, 2000, "H2O@", "entropy").toDouble();

   // Retrieve the derivative of G with respect to T
   double H2OdGdT = interface.thermoPropertiesSubstance( 300, 2000, "H2O", "entropy").toThermoScalar().ddt;

   // Write results to a comma separate files for a list of T-P pairs, substances, and properties
   interface.thermoPropertiesSubstance({{25, 1},{40, 1},{70, 100},{90, 100},{100, 100}}, // list of T-P pairs
                                        {"Al+3", "OH-", "SiO2@"},                        // list of substance symbols
                                        {"gibbs_energy","entropy", "volume", "enthalpy"} // list of properties
                                      ).toCSV("results.csv");                            // output
}
```

* Using the database client and retrieving a ThermoDataSet from the remote database

```
#!c++
int main()
{
   // Initialize a database client object
   DatabaseClient dbc;

   // Retrieve list of records given a ThermoDataSet symbol
   auto records = dbc.recordsFromThermoDataSet("PSINagra07"); 

   // Create a ThermoFun database using the records list
   Database db = databaseFromRecordList(dbc, records);

   // Initialize an interface object using the database
   ThermoFun::Interface interface (db);

   // Optional: set the solvent symbol used for calculating properties of aqueous species
   interface.setSolventSymbol("H2O@");

   // Optional set calculation and output preferences
   ThermoFun::OutputSettings op;
   op.isFixed = true;
   op.outSolventProp       = true;
   op.calcReactFromSubst   = false;
   op.calcSubstFromReact   = false;
   interface.setOutputSettings(op);

   // Optional set units and significant digits
   interface.setPropertiesUnits({"temperature", "pressure"},{"degC","bar"});
   interface.setPropertiesDigits({"reaction_gibbs_energy","reaction_entropy", "reaction_volume",
                                  "reaction_enthalpy","logKr", "temperature", "pressure"}, {0, 4, 4, 4, 4, 0, 0});

   interface.thermoPropertiesReaction({{25,1}}, {"AmSO4+", "MgSiO3@"}, {"reaction_gibbs_energy", "reaction_entropy",
                                      "reaction_volume", "reaction_enthalpy", "logKr"}).toCSV("results.csv");

   interface.thermoPropertiesReaction({0,20,50,75},{0,0,0,0},{"AmSO4+", "MgSiO3@"}, {"reaction_gibbs_energy", "reaction_entropy",
                                      "reaction_volume", "reaction_enthalpy", "logKr"}).toCSV("results.csv");
}

```