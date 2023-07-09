# lerImagens

Program to identify and track particles in a narrow fluidized bed, along with calculating granular temperature, velocity profile, bed height, moment of solids, mixing.

## Instalation

On CMakeLists.txt fix the opencv path for your computer.

Run to configure the compiler.
```bash
cmake .
```

Run to compile the program. 
```bash
make
```

Before using, it's necessary to create 2 folders in the main folder:
* Files
* PostFiles
```bash
mkdir Files
mkdir PostFiles
```
They will be used to store generated files

## Usage

The thresholds.txt have the numerical values that control the behavior of the program, so change it as necessary to have the correct settings for your data.

Usage is described in with the option:
```bash
./lerImagem -h
```

### Pre-processing

Pre-process of the images can be done with the option
```bash
./lerImagem -m pre -b preMode -p "Images/Path/pattern.tif" -c "Background/Images/pattern.tif"
```

Where the mode should be choosen from the list
- all : Runs all the methods
- build_mask : combine the image from "Background/Images/pattern.tif" to a mask of the background
- apply_mask : subtract the mask built from every frame of "Images/Path/pattern.tif"
- rotate : rotate images to the correct angle for processing

### Processing

Processing should be done with:
```bash
./lerImagem -m process -r all -t n -p "Images/Path/pattern.tif"
```

Where 'n' is the type of processing:
- 0 : monodisperse (default)
- 1 : bidisperse

### Simulation

Processing should be done with:
```bash
./lerImagem -m simulation -r all -p "Simulation/Path/pattern.tif"
```

Indicating the Glob pattern for the simulation files, example: "/dados/dump_liggghts_run.*"
