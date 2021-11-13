
#from pysat.solvers import Solver
from pysat.solvers import Lingeling as Solver
from pysat.formula import IDPool
from pysat.examples.hitman import Hitman

#INPUT
F1= [[5, 2], [-4,5], [-5], [3,-4] ,[-5,1 ], [-2,5], [-3,4]] #6 max

F2= [[6, 2], [-6,2 ], [-2,1], [-1], [-6,8], [-8,6],[2,4], [-4,5],[7,5],[-7,5],[-5,3],[-3]]#10 max

F3= [[9,1],[5,2],[7, 2],[-2,9], [-6,2], [-2,1],[6,2],[9,4],[-9],[-8,9], [-1], [-6,8], [-8,6],[2,4], [-4,5],[7,5],[-7,5],[-5,3],[8,1],[-3],[4,-9]]#18 max

F= F3

#FUNCTIONS
def max_sat_MHS(
    hard, 
    soft, 
    vpool= IDPool(),
):
    """
    :param hard: a list of clauses.
    :param soft: a list of clauses.
    :param vpool: an IDPool for assumptions.

    :return: the maximal set of satisfiable caluses.
    """

    vpool.occupy(min([abs(l) for c in soft for l in c]), max([abs(l) for c in soft for l in c])) #block the range of variable names, to avoid conflict with the (about to be) generated assumptions
    get_assumption = lambda x:vpool.id('assumption{}'.format(x))

    #add assumptions
    for i in range(len(soft)):
        clause = soft[i]
        # Add an assumption to each SOFT clause. HARD clauses do not get an assumption. 
        clause.append(-get_assumption(i))


    #prepare the oracles
    sat_oracle= Solver(bootstrap_with=hard+soft, with_proof=True)
    mhs_oracle= Hitman()

    #main algorithm loop
    mhs= []
    while True:
        #mhs are the clauses we disable: 
        #all other clauses are active
        assumptions =[get_assumption(i) if get_assumption(i) not in mhs else -get_assumption(i) for i in range(len(soft)) ]  #-get_assumption(i) disables clause i, get_assumption(i) tracks it

        if sat_oracle.solve(assumptions=assumptions):
            return [soft[i][:-1] for i in range(len(soft)) if -get_assumption(i) not in assumptions] 
        else:
            mhs_oracle.hit(sat_oracle.get_core())
            mhs= mhs_oracle.get()
        print("proof:",sat_oracle.get_proof())

       
#MAIN_CALL
max_sat_set= max_sat_MHS([],F)
print(max_sat_set)
print("count ", len(max_sat_set))