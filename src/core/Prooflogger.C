#include "Prooflogger.h"

//=================================================================================================
// Proof file

void Prooflogger::write_proof_header(int nbclause) {
    proof << "pseudo-Boolean proof version 1.0\n";
    proof << "f " << nbclause << "\n" ;
}

void Prooflogger::write_comment(const char* comment) {
    proof<< "* " << comment << "\n" ;
}

void Prooflogger::write_contradiction() {
    proof << "c " << constraint_counter << "\n" ;
}

void Prooflogger::write_empty_clause() {
    proof<< "u >= 1;\n" ;
    constraint_counter++;
    write_contradiction();
}

bool Prooflogger::is_aux_var(int var) {
    return var + 1 > n_variables;
}

std::string Prooflogger::var_name(int var) {
    std::string name;
    if(meaningful_names && meaningful_name_UB.find(var) != meaningful_name_UB.end()) {
        int lb = meaningful_name_LB[var];
        int ub = meaningful_name_UB[var];
        int n = meaningful_name_n[var];
        name = "v" + std::to_string(n) + "_x" + std::to_string(lb) + "_x" + std::to_string(ub); 
    } else if(is_aux_var(var)) {
        name = "y";
        name += std::to_string(var+1);
    } else {
        name = "x";
        name += std::to_string(var+1);
    } 
    return name;
}

void Prooflogger::write_literal(Lit literal) {

    // Weight and sign
    std::string weight_and_sign = "1 ";
    weight_and_sign += (sign(literal) == 1 ? "~" : "");

    // Variable symbol
    std::string name = var_name(var(literal));

    // Write
    proof << weight_and_sign << name << " " ;
}

void Prooflogger::write_literal_assignment(lbool assignment, int var) {

    // Sign
    std::string sign = assignment == l_True? "" : "~";

    // Variable symbol
    std::string symbol = var_name(var);

    // Write
    proof << sign << symbol << " " ;
}

void Prooflogger::write_witness(Lit literal) {
    std::string name = var_name(var(literal));
    proof<< name << " -> " << std::to_string(sign(literal) == 0) ;
}

void Prooflogger::write_clause(vec<Lit>& clause) {
    for (int i = 0; i < clause.size(); i++) write_literal(clause[i]);
}

void Prooflogger::write_clause(Clause& clause) {
    for(int i = 0; i < clause.size(); i++){
        write_literal(clause[i]);
    } 
}

void Prooflogger::write_learnt_clause(vec<Lit>& clause) {
    proof << "u ";
    write_clause(clause);
    proof << " >= 1;\n" ;
    constraint_counter++;
}

void Prooflogger::delete_learnt_clause(Clause& clause) {
    proof << "del find ";
    write_clause(clause);
    proof << " >= 1;\n" ;
}

void Prooflogger::write_linkingVar_clause(vec<Lit>& clause) {
    int variable = var(clause[0]);
    int constraint_id = C2_store[variable];
    if(constraint_id != 0) {
        proof << "p " << constraint_id << " " << last_bound_constraint_id << " + s\n" ;
        constraint_counter++;
    }
    write_learnt_clause(clause);
}

void Prooflogger::write_bound_update(vec<lbool>& model) {
    proof<< "o ";
    for(int i = 0; i < model.size(); i++) write_literal_assignment(model[i], i);
    proof << "\n" ;

    // Veripb automatically adds an improvement constraint so counter needs to be incremented
    last_bound_constraint_id = ++constraint_counter;
}

void Prooflogger::write_unit_sub_red(vec<Lit>& definition, int sigma, int from, int to) {

    if(meaningful_names) {

        // If variable does not already have a meaningful name
        if(meaningful_name_LB.find(var(definition[0])) == meaningful_name_LB.end()) {
            meaningful_name_LB[var(definition[0])] = from+1;
            meaningful_name_UB[var(definition[0])] = to+1;
            meaningful_name_n[var(definition[0])] = sigma;
        }
    }

    proof << "red ";
    write_clause(definition);
    proof << ">= 1; ";
    write_witness(definition[0]);
    proof << "\n" ;
    constraint_counter++;
}

void Prooflogger::write_P1_sub_red_cardinality(int var, int sigma, int from, int to) {

    if(meaningful_names) {

        // If variable does not already have a meaningful name
        if(meaningful_name_LB.find(var) == meaningful_name_LB.end()) {
            meaningful_name_LB[var] = from+1;
            meaningful_name_UB[var] = to+1;
            meaningful_name_n[var] = sigma;
        }
    }

    int weight = (to-from+1)-(sigma - 1);
    proof << "red ";
    for(int i = from; i < to+1; i++) {
        write_literal(~Lit(i));
    }
    proof << weight << " " << var_name(var) << " >= " << weight << "; ";
    write_witness(Lit(var));
    proof << "\n" ;
    constraint_counter++;
    C1_store[var] = constraint_counter;
}

void Prooflogger::write_P2_sub_red_cardinality(int var, int sigma, int from, int to) {

    if(meaningful_names) {

        // If variable does not already have a meaningful name
        if(meaningful_name_LB.find(var) == meaningful_name_LB.end()) {
            meaningful_name_LB[var] = from+1;
            meaningful_name_UB[var] = to+1;
            meaningful_name_n[var] = sigma;
        }
    }

    int weight = sigma;
    proof << "red ";
    for(int i = from; i < to+1; i++) {
        write_literal(Lit(i));
    }
    proof << weight << " ~" << var_name(var) << " >= " << weight << "; ";
    write_witness(~Lit(var));
    proof << "\n" ;
    constraint_counter++;
    C2_store[var] = constraint_counter;
}

void Prooflogger::write_C1(vec<Lit>& definition, int sigma, int from, int to) {
    int first = var(definition[0]);
    int second = var(definition[1]);
    int third = var(definition[2]);

    // Write derivation of parts
    bool resolved_one = false;
    if(C2_store.find(first) != C2_store.end()) {
        resolved_one = true;
        proof << "p " << C1_store[third] << " " << C2_store[first] << " +\n" ;
        constraint_counter++;
    }
    if(C2_store.find(second) != C2_store.end()) {
        int to_add_to = resolved_one? constraint_counter : C1_store[third];
        proof << "p " << to_add_to << " " << C2_store[second] << " +\n" ;
        constraint_counter++;
        resolved_one = true;
    }
    if(resolved_one) {
        proof << "p " << constraint_counter << " s\n" ;
        constraint_counter++;
    }

    // Derivation is done so clause can be written as RUP
    write_learnt_clause(definition);
}

void Prooflogger::write_C2(vec<Lit>& definition, int sigma, int from, int to) {
    int first = var(definition[0]);
    int second = var(definition[1]);
    int third = var(definition[2]);

    // Write derivation of parts
    bool resolved_one = false;
    if(C1_store.find(first) != C1_store.end()) {
        resolved_one = true;
        proof << "p " << C2_store[third] << " " << C1_store[first] << " +\n" ;
        constraint_counter++;
    }
    if(C1_store.find(second) != C1_store.end()) {
        int to_add_to = resolved_one? constraint_counter : C2_store[third];
        proof << "p " << to_add_to << " " << C1_store[second] << " +\n" ;
        constraint_counter++;
        resolved_one = true;
    }
    if(resolved_one) {
        proof << "p " << constraint_counter << " 2 d s\n" ;
        constraint_counter++;
    }

    // Derivation is done so clause can be written as RUP
    write_learnt_clause(definition);
}

void Prooflogger::genCardinalDefinitions(int from, int to, vec<Lit>& lits, vec<Lit>& linkingVar) {
  int inputSize = to - from + 1;
  linkingVar.clear();

  vec<Lit> linkingAlpha;
  vec<Lit> linkingBeta;

  Var varZero = variable_counter++;
  Var varLast = variable_counter++;

  // First
  lits.clear(); lits.push(Lit(varZero));
  write_unit_sub_red(lits, 0, from, to);

  // Last
  lits.clear(); lits.push(~Lit(varLast));
  write_unit_sub_red(lits, inputSize+1, from, to);


  if (inputSize > 2) {
    int middle = inputSize/2;
    genCardinalDefinitions(from, from+middle, lits, linkingAlpha);
    genCardinalDefinitions(from+middle+1, to, lits, linkingBeta);
  } else if (inputSize == 2) {
    genCardinalDefinitions(from, from, lits, linkingAlpha);
    genCardinalDefinitions(to, to, lits, linkingBeta);
  }
  if (inputSize == 1) {
    linkingVar.push(Lit(varZero));
    linkingVar.push(Lit(from));
    linkingVar.push(Lit(varLast));
  } else { // inputSize >= 2

    write_comment("- Node clauses:");
    linkingVar.push(Lit(varZero));
    for (int i = 0; i < inputSize; i++) linkingVar.push(Lit(variable_counter++));
    linkingVar.push(Lit(varLast));

    for (int sigma = 0; sigma <= inputSize; sigma++) {

        // Verify if PB cardinality definition exists
        if(C1_store.find(var(linkingVar[sigma])) == C1_store.end()) {

            // Write a substitution redundancy PB cardinality definition
            write_P1_sub_red_cardinality(var(linkingVar[sigma]), sigma, from, to);
        }

        // Verify if PB cardinality definition exists
        if(C2_store.find(var(~linkingVar[sigma+1])) == C2_store.end()) {

            // Write a substitution redundancy PB cardinality definition
            write_P2_sub_red_cardinality(var(~linkingVar[sigma+1]), sigma+1, from, to);
        }
    }
    write_comment("-------------------------------------------");
  }
  linkingAlpha.clear();
  linkingBeta.clear();
}