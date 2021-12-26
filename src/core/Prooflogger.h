#ifndef Prooflogger_h
#define Prooflogger_h

#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#include <map>
#include <string>
#include <regex>

#include "Vec.h"
#include "Heap.h"
#include "Alg.h"

#include "SolverTypes.h"


//=================================================================================================
// Prooflogger -- the main class:


class Prooflogger {
public:

    // Formula number of clauses and number of variables
    //
    int formula_length = 0;
    int n_variables = 0;

    // Constraint counter
    //
    int constraint_counter = 0;

    // Simplified constraint store
    // 
    bool simplify = false;
    std::map<int, int> unit_store;

    // Meaningful variable names
    bool meaningful_names = false;
    std::map<int, int> meaningful_name_LB;
    std::map<int, int> meaningful_name_UB;
    std::map<int, int> meaningful_name_n;

    // Constraint and coeff store
    //
    std::map<int, int> constraint_store;
    std::map<int, int> coeff_store;

    // Proof file
    std::stringstream proof;
    const char *proof_file_name = "maxsat_proof.pbp";
    void set_proof_name             (const char* name) {proof_file_name = name;};
    void write_proof_file           ();

    void write_proof_header         (int nbclause);
    void write_comment              (const char* comment);
    void write_empty_clause         ();
    const char* literal_symbol      (int var); 
    void write_learnt_clause        (vec<Lit>& clause);
    void write_C2_v1_sum            (vec<int>& constraint_ids, int key);
    int write_C_sub_red             (vec<Lit>& definition, int sigma, int from, int to);
    void write_unit_sub_red         (vec<Lit>& definition);
    void write_bound_update         (vec<lbool>& model); 
    void write_contradiction        ();
    void store                      (Lit literal);

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