#ifndef Prooflogger_h
#define Prooflogger_h

#include <cstdio>
#include <fstream>
#include <iostream>

#include "Vec.h"
#include "Heap.h"
#include "Alg.h"

#include "SolverTypes.h"


//=================================================================================================
// Prooflogger -- the main class:


class Prooflogger {
public:

    // Constrain counter
    //
    int constraint_counter = 0;

    // Proof file
    std::ofstream proof_file;
    const char *proof_file_name = "maxsat_proof.proof";
    void open    ();
    void close   ();
    void set_name(const char* name);
    
    // Printing functions
    //
    void write_header       (int nClauses);
    void derived_empty_clause ();
    void write_learnt_clause(vec<Lit>& clause);
    void write_constraint   (vec<Lit>& clause);
    void write_contradiction();

};

//=================================================================================================
#endif