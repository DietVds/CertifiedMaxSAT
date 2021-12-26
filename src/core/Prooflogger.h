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
// Prooflogger -- polish notation class:

class Expression {
public:
};

class Single : public Expression {
public:
    Single(int constraint_id);

    // Constraint id
    //
    int constraint_id;
};

class Operation : public Expression {
public:
    Operation(Expression* a, Expression* b, const char* operant);

    // Operands
    //
    Expression* a;
    Expression* b;

    // Operant
    //
    std::string operant;

    // Apply
    //
    std::string apply();
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

    // Tree derivation
    //
    int tree_constraint_counter = 0;
    vec<Expression*> tree_derivation;

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
    void write_tree_derivation      ();
    void write_proof_header         (int nbclause);
    void write_comment              (const char* comment);
    void write_contradiction        ();
    void write_empty_clause         ();
    bool is_aux_var                 (int var); 
    void write_literal              (Lit literal);
    void write_literal_assignment   (lbool assignment, int var);
    void write_witness              (Lit literal);
    void write_clause               (vec<Lit>& clause);
    void write_learnt_clause        (vec<Lit>& clause);
    void write_linkingVar_clause    (vec<Lit>& clause);
    void write_bound_update         (vec<lbool>& model); 
    void write_unit_sub_red         (vec<Lit>& definition);
    void write_C2_sum               (vec<int>& constraint_ids, int third, int from, int to);
    int write_C_sub_red             (vec<Lit>& definition, int sigma, int from, int to);

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