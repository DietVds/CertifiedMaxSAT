# A Certified MaxSAT Solver

Underneath you can find our contact information:
| Name | Student id | Email address |
| :--- | :--- |:--- |
| Wolf De Wulf | 0546395 | [wolf.de.wulf@vub.be](mailto:wolf.de.wulf@vub.be) |
| Dieter Vandesande | 0565683 | [dieter.vandesande@vub.be](mailto:dieter.vandesande@vub.be) |

## Patches
The [patches](patches) folder contains a number of patches, in what follows a description is given for each of them:

### 

## Building scripts
The [scripts](scripts) folder contains a number of buildings scripts, in what follows a description is given for each of them:

### Building the certified solver

Run the `build.sh` script to extract the MiniSat and QMaxSAT archives, combine them, apply the fix a prooflogging patches to them and build the final solver:

```console
./scripts/build.sh
```

### Building veripb

Run the `build_src.sh` script to extract the veripb archive and build/install the proof verifier:

```console
./scripts/build_veripb.sh
```

### Building the source code

Run the `build_src.sh` script to extract the MiniSat and QMaxSAT archives, combine them and apply the patches to them:

```console
./scripts/build_src.sh PATH_TO_SRC
```

## Usage

```console
./qmaxsat --help
```
