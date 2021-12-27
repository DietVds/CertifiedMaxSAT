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
// Prooflogger -- reverse polish notation class:

class ReversePolishNotation {
public:
    virtual std::string apply(int constraint_id_at_start_of_printing){};
};

class Operand : public ReversePolishNotation {
public:
    Operand(int value);

    // Value
    //
    int value;

    std::string apply(int constraint_id_at_start_of_printing) override;
};

class Operation : public ReversePolishNotation {
public:
    Operation(ReversePolishNotation* a, ReversePolishNotation* b, const char* operant);

    // Operands
    //
    ReversePolishNotation* a;
    ReversePolishNotation* b;

    // Operant
    //
    std::string operant;

    // Apply
    //
    std::string apply(int constraint_id_at_start_of_printing) override;
};


//=================================================================================================
// Prooflogger -- the main class:


class Prooflogger {
public:

    // Formula information
    //
    int formula_length = 0;
    int n_variables = 0;

    // Constraint counter
    //
    int constraint_counter = 0;
    int last_bound_constraint_id;

    // Cardinality derivation
    //
    int cardinality_constraint_counter = 0;
    vec<ReversePolishNotation*> cardinality_derivation;

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
    const char *proof_file_name         = "maxsat_proof.pbp";
    void set_proof_name                 (const char* name) {proof_file_name = name;};
    void write_proof_file               ();
    void write_cardinality_derivation   ();
    void write_proof_header             (int nbclause);
    void write_comment                  (const char* comment);
    void write_contradiction            ();
    void write_empty_clause             ();
    bool is_aux_var                     (int var); 
    void write_literal                  (Lit literal);
    void write_literal_assignment       (lbool assignment, int var);
    void write_witness                  (Lit literal);
    void write_clause                   (vec<Lit>& clause);
    void write_learnt_clause            (vec<Lit>& clause);
    void write_linkingVar_clause        (vec<Lit>& clause);
    void write_bound_update             (vec<lbool>& model); 
    void write_unit_sub_red             (vec<Lit>& definition);
    void write_C2_sum                   (vec<int>& constraint_ids, int third, int sigma, int from, int to);
    int write_C_sub_red                 (vec<Lit>& definition, int sigma, int from, int to);

    // OPB file
    std::ofstream OPB_file;
    std::stringstream constraints;
    const char *OPB_file_name           = "maxsat_problem.opb";
    void open_OPB_file                  ()                   {OPB_file.open(OPB_file_name);};
    void write_OPB_file                 ()                   {OPB_file << constraints.rdbuf(); OPB_file.close();};
    void set_OPB_name                   (const char* name)   {OPB_file_name = name;};

    void write_OPB_header               (int nbvar, int nbclause);
    void write_minimise                 (int start_var, int num);
    void write_OPB_constraint           (vec<Lit>& constraint, int weight);
};

//=================================================================================================
#endif