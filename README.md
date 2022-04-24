# A Certified MaxSAT Solver

Our contact information:
| Name | Email address |
| :--- | :--- |
| Dieter Vandesande | [dieter.vandesande@vub.be](mailto:dieter.vandesande@vub.be) |
| Wolf De Wulf | [wolf.de.wulf@vub.be](mailto:wolf.de.wulf@vub.be) |
| Bart Bogaerts | [bart.bogaerts@vub.be](mailto:bart.bogaerts@vub.be) |

## Patches

The [patches](patches) folder contains two patches, underneath we give a description for them.

### MiniSAT fix

The [qmaxsat_fix.patch](patches/qmaxsat_fix.patch) patch applies a fix to the Qmaxsat 0.1 source code such that it works with recent C++ compilers.

### Prooflogging

The [prooflogging.patch](patches/prooflogging.patch) patch extends the (fixed) Qmaxsat 0.1 source code with prooflogging.

## Usage

```console
./qmaxsat --help
```

## Citation

D. Vandesande, W. De Wulf, B. Bogaerts. A Certified MaxSAT Solver. (to be submitted at LPNMR2022)
