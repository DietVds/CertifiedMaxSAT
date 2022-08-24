#ifndef Prooflogger_h
#define Prooflogger_h

#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#include <map>
#include <forward_list>
#include <string>
#include <regex>

#include "Vec.h"
#include "Heap.h"
#include "Alg.h"

#include "SolverTypes.h"

//=================================================================================================
// Prooflogger 


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
    int variable_counter = 0;

    // Meaningful variable names
    bool meaningful_names = false;
    std::map<int, int> meaningful_name_LB;
    std::map<int, int> meaningful_name_UB;
    std::map<int, int> meaningful_name_n;

    // Constraint stores
    //TODO STRANGE NAMES: THEY ARE RELATED TO THE P-CONSTRAINTS, NOT THE C-CONSTRAINTS
    std::map<int, int> P1_store;
    std::map<int, int> P2_store;

    std::forward_list<int> constraint_ids_to_delete;

    // Proof file
    std::ofstream proof;
    const char *proof_file_name         = "maxsat_proof.pbp";
    void set_proof_name                 (const char* name) {proof_file_name = name;};
    void open_proof_file                (){proof.open(proof_file_name);};
    void close_proof_file               (){proof.close();};
    void write_tree_derivation          ();
    void write_proof_header             (int nbclause);
    void write_comment                  (const char* comment);
    void write_contradiction            ();
    void write_empty_clause             ();
    void store_meaningful_name          (int var, int from, int to, int n);
    bool is_aux_var                     (int var); 
    std::string var_name                (int var); 
    void write_literal                  (Lit literal);
    void write_literal_assignment       (lbool assignment, int var);
    void write_witness                  (Lit literal);
    void write_clause                   (vec<Lit>& clause);
    void write_clause                   (Clause& clause);
    void write_learnt_clause            (vec<Lit>& clause);
    void overwrite_learnt_clause        (vec<Lit>& clause);
    void implies_rule                   (int constraint_id, vec<Lit>& c);
    void check_last_constraint          (vec<Lit>& c);    
    void delete_learnt_clause           (Clause& clause);
    void delete_constraint              (int constraint_id);
    void write_linkingVar_clause        (vec<Lit>& clause);
    void write_bound_update             (vec<lbool>& model); 
    void write_sub_red                  (vec<Lit>& clause);
    void write_C1                       (vec<Lit>& definition, int sigma, int from, int to);
    void write_P1_sub_red_cardinality   (int var, int sigma, int from, int to);
    void delete_P1                      (const vec<Lit>& reification_literals);
    void write_C2                       (vec<Lit>& definition, int sigma, int from, int to);
    void write_P2_sub_red_cardinality   (int var, int sigma, int from, int to);
    void delete_P2                      (const vec<Lit>& reification_literals);
    void genCardinalDefinitions         (int from, int to, vec<Lit>& linkingVar);
    void add_P1_as_clause               (Var var);
    void add_P2_as_clause               (Var var);
    void delete_P                       (const vec<Lit>& reification_literals, std::map<int,int>& constraint_store);
    void delete_cardinality_defs        (const vec<Lit>& reification_literals);
    void write_deletes                  ();

    void write_clause_as_comment        (vec<Lit>& clause);
    void write_clause_as_comment        (Clause& clause);
};

//=================================================================================================
#endif