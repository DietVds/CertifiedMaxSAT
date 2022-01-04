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
// Prooflogger -- VeriPB operation classes:

class VeriPBOperation {
public:
    virtual std::string apply(int constraint_id_at_start_of_printing){};
};

class CPOperand : public VeriPBOperation {
public:
    CPOperand(int value);

    // Value
    //
    int value;

    std::string apply(int constraint_id_at_start_of_printing) override;
};

class RUP : public VeriPBOperation {
public:
    RUP(vec<Lit>& clause);

    // Clause
    //
    vec<Lit> clause;
};

class CP1 : public VeriPBOperation {
public:
    CP1(VeriPBOperation* a, const char* operant);

    // Operands
    //
    VeriPBOperation* a;

    // Operant
    //
    std::string operant;

    // Apply
    //
    std::string apply(int constraint_id_at_start_of_printing) override;
};

class CP2 : public VeriPBOperation {
public:
    CP2(VeriPBOperation* a, VeriPBOperation* b, const char* operant);

    // Operands
    //
    VeriPBOperation* a;
    VeriPBOperation* b;

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

    // Tree derivation
    //
    int tree_constraint_counter = 0;
    vec<VeriPBOperation*> tree_derivation;

    // Meaningful variable names
    bool meaningful_names = false;
    std::map<int, int> meaningful_name_LB;
    std::map<int, int> meaningful_name_UB;
    std::map<int, int> meaningful_name_n;

    // Constraint and weight stores
    //
    std::map<int, int> C1_store;
    std::map<int, int> C1_weight_store;
    std::map<int, int> C2_store;
    std::map<int, int> C2_weight_store;

    // Proof file
    std::ofstream proof;
    const char *proof_file_name         = "maxsat_proof.pbp";
    void set_proof_name                 (const char* name) {proof_file_name = name;};
    void open_proof_file                (){proof.open(proof_file_name);};
    void close_proof_file               (){proof.close();};
    void write_tree_derivation          ();
    void delete_tree_derivation         (VeriPBOperation* node); 
    void write_proof_header             (int nbclause);
    void write_comment                  (const char* comment);
    void write_contradiction            ();
    void write_empty_clause             ();
    bool is_aux_var                     (int var); 
    std::string var_name                (int var); 
    void write_literal                  (Lit literal);
    void write_literal_assignment       (lbool assignment, int var);
    void write_witness                  (Lit literal);
    void write_clause                   (vec<Lit>& clause);
    void write_learnt_clause            (vec<Lit>& clause);
    void write_linkingVar_clause        (vec<Lit>& clause);
    void write_bound_update             (vec<lbool>& model); 
    void write_unit_sub_red             (vec<Lit>& definition, int sigma, int from, int to);
    void write_C1                       (vec<Lit>& definition, int sigma, int from, int to);
    void write_P1_sub_red_cardinality   (int var, int sigma, int from, int to);
    void write_C2                       (vec<Lit>& definition, int sigma, int from, int to);
    void write_P2_sub_red_cardinality   (int var, int sigma, int from, int to);

    // OPB file
    std::ofstream OPB_file;
    std::stringstream constraints;
    const char *OPB_file_name           = "maxsat_problem.opb";
    void open_OPB_file                  ()                   {OPB_file.open(OPB_file_name);};
    void write_OPB_file                 ()                   {OPB_file << constraints.rdbuf(); OPB_file.close();};
    void set_OPB_name                   (const char* name)   {OPB_file_name = name;};

    void write_OPB_header               (int nbvar, int nbsoft, int nbclause);
    void write_minimise                 (int start_var, int num);
    void write_OPB_constraint           (vec<Lit>& constraint);
};

//=================================================================================================
#endif