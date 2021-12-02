#ifndef Prooflogger_h
#define Prooflogger_h

#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>

#include "Vec.h"
#include "Heap.h"
#include "Alg.h"

#include "SolverTypes.h"


//=================================================================================================
// Prooflogger -- the main class:


class Prooflogger {
public:

    // Formula number of clauses
    //
    int formula_length = 0;

    // Constraint counter
    //
    int constraint_counter = 0;

    // Proof file
    std::ofstream proof_file;
    const char *proof_file_name = "maxsat_proof.proof";
    void open_proof                 ()                 {proof_file.open(proof_file_name);};
    void close_proof                ()                 {proof_file.close();};
    void set_proof_name             (const char* name) {proof_file_name = name;};

    void write_proof_header    (int nClauses);
    void write_comment         (const char* comment);
    void derived_empty_clause  ();
    const char* literal_symbol (Lit lit); 
    void write_learnt_clause   (vec<Lit>& clause);
    void write_sub_red         (vec<Lit>& definition, bool ass);
    void write_constraint      (vec<Lit>& clause);
    void write_contradiction   ();
    void write_delete          (int number);

    // OPB file
    std::ofstream OPB_file;
    std::stringstream constraints;
    const char *OPB_file_name = "maxsat_problem.opb";
    void open_OPB                 ()                   {OPB_file.open(OPB_file_name);};
    void close_OPB                ()                   {OPB_file << constraints.rdbuf(); OPB_file.close();};
    void set_OPB_name             (const char* name)   {OPB_file_name = name;};

    void write_OPB_header         (int nbvar, int nbclause);
    void write_minimise           (int start_var, int num);
    void write_OPB_constraint     (vec<Lit>& constraint, int weight);
};

//=================================================================================================
#endif