import argparse
import sys


def solve(
    solver_args,
    **kwargs
):
    raise NotImplementedError

def run_solver(
    solver_params
):

    parser = argparse.ArgumentParser()

    # Input and output files
    parser.add_argument('--inputfile', '-i', type=str, default=sys.stdin, help="Input DIMACS encoding file")
    parser.add_argument('--outputfile', '-o', type=str, default=sys.stout, help="Output file")
    parser.add_argument('--prooflog', '-p', type=str, default="proof.txt", help="Proof log output file")

    # Parse
    solver_args = parser.parse_args(sys.argv[1:])

    # Add to params
    solver_params[solver_args] = vars(solver_args)

    solve(**solver_args)