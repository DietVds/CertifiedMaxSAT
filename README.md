# A proof-logged MaxSAT solver

This project was made in the context of the Discrete Modelling, Optimization and Search (DMOS) course, given by Prof. Dr. Bart Bogaerts at the [VUB](https://www.vub.be/).

Underneath you can find our contact information:
| Name | Student id | Email address |
| :--- | :--- |:--- |
| Wolf De Wulf | 0546395 | [wolf.de.wulf@vub.be](mailto:wolf.de.wulf@vub.be) |
| Dieter Vandesande | 00000 | [dieter.vandesande@vub.be](mailto:dieter.vandesande@vub.be) |

## Installation

### 1. Setting up the virtual environment

```console
python -m venv venv
```

### 2. Activating the virtual environment

```console
source venv/bin/activate
```

To deactivate the virtual environment, use:

```console
deactivate
```

### 3. Installing the required packages

Make sure the virtual environment is active!

```console
pip3 install -r requirements.txt
```

### 4. Installing veripb

Make sure the virtual environment is active!

```console
./setup_veripb.sh
```

## Usage

```console
python main.py -h
```

## Input format

The solver expects [WDIMACS](http://www.maxhs.org/docs/wdimacs.html) input.

## Output format
