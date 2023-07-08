# lerImagens

Program to identify and track particles in a narrow fluidized bed, along with calculating granular temperature, velocity profile, bed height, moment of solids, mixing.

## Instalation

On CMakeLists.txt fix the opencv path for your computer.

Run
```bash
cmake .
```
to configure the compiler.

Run
```bash
make
```
to compile the program.

## Usage

Before using, it's necessary to create 2 folders in the main folder:
* Files
* PostFiles
```bash
mkdir Files
mkdir PostFiles
```
They will be used to store generated files

The thresholds.txt have the numerical values that control the behavior of the program, so change it as necessary to have the correct settings for your data.

Usage is descripted in with the -h option
