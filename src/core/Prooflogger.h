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

    // Constraint counter
    //
    int constraint_counter = 0;

    // Proof file
    std::ofstream proof_file;
    const char *proof_file_name = "maxsat_proof.proof";
    void open                 ();
    void close                ();
    void set_name             (const char* name);
    
    // Printing functions
    //
    void write_header         (int nClauses);
    void write_comment        (const char* comment);
    void derived_empty_clause ();
    void write_learnt_clause  (vec<Lit>& clause);
    void write_sub_red        (vec<Lit>& definition, bool ass, int start_x = 1);
    void write_constraint     (vec<Lit>& clause);
    void write_contradiction  ();
    void write_delete         (int number);

};

//=================================================================================================
#endif