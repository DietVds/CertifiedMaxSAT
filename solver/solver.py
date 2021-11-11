import argparse
import sys
from pysat.formula import WCNF
from pysat.solvers import Solver
from pysat.formula import IDPool
from pysat.examples.hitman import Hitman


def solve(
    solver_args,
    **kwargs
):

    """
    Open the input file and read it in
    """
    try:
        if solver_args['inputfile'] is None:
            input_string = sys.stdin.read()
        else:
            with open(solver_args['inputfile']) as inputfile: input_string = inputfile.read()
    
        input = WCNF(from_string=input_string)
    except Exception as e:
        print("Something went wrong when parsing the input file, message: " + str(e))
        return


    """
    MHS MaxSat implementation (from WPO)
    """
    soft = input.soft
    hard = input.hard
    weights = input.wght
    vpool = IDPool()
    vpool.occupy(min([abs(l) for c in soft for l in c]), max([abs(l) for c in soft for l in c])) # block the range of variable names, to avoid conflict with the (about to be) generated assumptions
    get_assumption = lambda x:vpool.id('assumption{}'.format(x))

    # Add assumptions
    for i in range(len(soft)):
        clause = soft[i]
        # Add an assumption to each SOFT clause. HARD clauses do not get an assumption. 
        clause.append(-get_assumption(i))


    # Prepare the oracles
    sat_oracle= Solver(bootstrap_with=hard+soft)
    mhs_oracle= Hitman()

    # Main algorithm loop
    mhs= []
    while True:
        # mhs are the clauses we disable: 
        # all other clauses are active
        assumptions =[get_assumption(i) if get_assumption(i) not in mhs else -get_assumption(i) for i in range(len(soft)) ]  # -get_assumption(i) disables clause i, get_assumption(i) tracks it

        if sat_oracle.solve(assumptions=assumptions):

            # Found SAT solution
            solution = [i for i in range(len(soft)) if -get_assumption(i) not in assumptions] 

            # Calculate cost
            cost = sum([weights[i] if i in solution else 0])

            # Get assignment
            ass = sat_oracle.get_model()

            break
        else:
            mhs_oracle.hit(sat_oracle.get_core())
            mhs= mhs_oracle.get() 


    """
    Print output
    """
    solution_string = f"o {cost}\ns OPTIMUM FOUND\nv {' '.join([str(var) for var in ass])}\n"
    try:
        if solver_args['outputfile'] is None:
            sys.stdout.write(str(solution_string))
        else:
            with open(solver_args['outputfile'], "w") as outputfile: 
                outputfile.write(str(solution_string))
    except Exception as e:
        print("Something went wrong when writing the solution, message: " + str(e))
        print("Printing solution to stdout:")
        sys.stdout.write(str(solution_string))
        return


def run_solver(
    solver_params
):

    parser = argparse.ArgumentParser()

    # Input and output files
    parser.add_argument('--inputfile', '-i', type=str, default=None, help="Input WDIMACS encoding file")
    parser.add_argument('--outputfile', '-o', type=str, default=None, help="Output file")
    parser.add_argument('--prooflog', '-p', type=str, default="proof.txt", help="Proof log output file")

    # Parse
    solver_args = parser.parse_args(sys.argv[1:])

    # Add to params
    solver_params['solver_args'] = vars(solver_args)

    solve(**solver_params)