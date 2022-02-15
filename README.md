# A Certified MaxSAT Solver

Underneath you can find our contact information:
| Name | Student id | Email address |
| :--- | :--- |:--- |
| Wolf De Wulf | 0546395 | [wolf.de.wulf@vub.be](mailto:wolf.de.wulf@vub.be) |
| Dieter Vandesande | 0565683 | [dieter.vandesande@vub.be](mailto:dieter.vandesande@vub.be) |

## Patches

The [patches](patches) folder contains a number of patches, in what follows a description is given for each of them.

### MiniSAT fix

The [qmaxsat_fix.patch](patches/qmaxsat_fix.patch) patch applies a fix to the Qmaxsat 0.1 source code such that it works with recent C++ compilers.

### Prooflogging

The [prooflogging.patch](patches/prooflogging.patch) patch extends the (fixed) Qmaxsat 0.1 source code with prooflogging.

### VeriPB WCNF extension

The [veripb_wcnf.patch](patches/veripb_wcnf.patch) patch extends the VeriPB source code with the ability to read from `.wcnf` files.  
Use the `-wcnf` flag to indicate that the input file is in the `.wcnf` format.

## Usage

```console
./qmaxsat --help
```

## Citation

D. Vandesande, W. De Wulf, B. Bogaerts. A Certified MaxSAT Solver. (to be submitted at SAT2022)
