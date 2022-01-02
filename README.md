# A certified MaxSAT solver

This project was made in the context of the Discrete Modelling, Optimization and Search (DMOS) course, given by Prof. Dr. Bart Bogaerts at the [VUB](https://www.vub.be/).

Underneath you can find our contact information:
| Name | Student id | Email address |
| :--- | :--- |:--- |
| Wolf De Wulf | 0546395 | [wolf.de.wulf@vub.be](mailto:wolf.de.wulf@vub.be) |
| Dieter Vandesande | 0565683 | [dieter.vandesande@vub.be](mailto:dieter.vandesande@vub.be) |

## Building the solver

Run the `build.sh` script to extract the MiniSat and QMaxSAT archives, combine them, apply the patches to them and build the final solver:

```console
./scripts/build.sh
```

## Building veripb

Run the `build_src.sh` script to extract the veripb archive and build/install the proof verifier:

```console
./scripts/build_veripb.sh
```

## Building the source code

Run the `build_src.sh` script to extract the MiniSat and QMaxSAT archives, combine them and apply the patches to them:

```console
./scripts/build_src.sh PATH_TO_SRC
```

## Creating a patch file for the current src folder

Run the `build_patch.sh` script to create a patch file that compares the current `src` to a fresh `src` built by `build_src.sh`:

```console
./scripts/build_patch.sh PATCH_FILE_NAME
```

## Usage

```console
./qmaxsat --help
```
