#include "Prooflogger.h"
#include<string>

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
        // The used constraints are deleted before the next MiniSAT-call
        constraint_ids_to_delete.push_front(constraint_counter);
        constraint_ids_to_delete.push_front(constraint_id);
        C2_store.erase(constraint_id);
    }    
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

void Prooflogger::delete_P(const vec<Lit>& reification_literals, std::map<int,int>& constraint_store){
    for(int i = 0; i < reification_literals.size(); i++){
        int variable = var(reification_literals[i]);
        int constraint_id = constraint_store[variable];

        if(constraint_id != 0) // Only remove constraint if it is one that can be deleted.
                                // We did not create the P1/P2 constraint definitions for the trivial v_0 and v_(vars(n)+1) variables.
            constraint_ids_to_delete.push_front(constraint_id);
            // proof << "del id " << constraint_id << "\n";

        constraint_store.erase(variable);
    }
}

void Prooflogger::delete_P1(const vec<Lit>& reification_literals){
    delete_P(reification_literals, C1_store);
}

void Prooflogger::delete_P2(const vec<Lit>& reification_literals){
    delete_P(reification_literals, C2_store);
}

void Prooflogger::delete_cardinality_defs(const vec<Lit>& reification_literals){
    delete_P1(reification_literals);
    delete_P2(reification_literals);
}

void Prooflogger::write_deletes(){
    while(! constraint_ids_to_delete.empty()){
        int constraint_id = constraint_ids_to_delete.front();

        proof << "del id " << constraint_id << "\n";
        constraint_ids_to_delete.pop_front();
    }
}


void Prooflogger::write_C1(vec<Lit>& definition, int sigma, int from, int to) {
    int first = var(definition[0]);
    int second = var(definition[1]);
    int third = var(definition[2]);

    // Write derivation of parts
    bool resolved_one = false;
    std::stringstream ss;
    ss << "p " << C1_store[third];
    if(C2_store.find(first) != C2_store.end()) {
        resolved_one = true;
        ss << " " << C2_store[first] << " + " ;
    }
    if(C2_store.find(second) != C2_store.end()) {
        ss << " " << C2_store[second] << " +" ;
        resolved_one = true;
    }
    if(resolved_one) {
        ss << " s\n" ;
        proof << ss.str();
        constraint_counter++;
        constraint_ids_to_delete.push_front(constraint_counter);
    }
}



void Prooflogger::write_C2(vec<Lit>& definition, int sigma, int from, int to) {
    int first = var(definition[0]);
    int second = var(definition[1]);
    int third = var(definition[2]);

    // Write derivation of parts
    bool resolved_one = false;
    std::stringstream ss;
    ss << "p " << C2_store[third];
    if(C1_store.find(first) != C1_store.end()) {
        resolved_one = true;
        ss  << " " << C1_store[first] << " + " ;
    }
    if(C1_store.find(second) != C1_store.end()) {
        ss << " " << C1_store[second] << " + " ;
        resolved_one = true;
    }
    if(resolved_one) {
        ss << " s\n" ;
        proof << ss.str();
        constraint_counter++;
        constraint_ids_to_delete.push_front(constraint_counter);
    }
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
  C1_store[varZero] = constraint_counter;

  // Last
  lits.clear(); lits.push(~Lit(varLast));
  write_unit_sub_red(lits, inputSize+1, from, to);
  C1_store[varLast] = constraint_counter;

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

void Prooflogger::write_clause_as_comment(vec<Lit>& clause) {
    proof << "* ";
    write_clause(clause);
    proof << ">= 1;\n" ;
}

void Prooflogger::write_clause_as_comment(Clause& clause){
    proof << "* ";
    write_clause(clause);
    proof << " >= 1;\n" ;
}