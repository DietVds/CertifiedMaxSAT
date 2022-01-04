#include "Prooflogger.h"

//=================================================================================================
// Prooflogger -- VeriPB operation classes:

// Initialisers
CPOperand::CPOperand(int value) : value(value){};
RUP::RUP(vec<Lit>& in_clause) {
    for(int i = 0; i < in_clause.size(); i++) {
        clause.push(in_clause[i]);
    }
};
CP1::CP1(VeriPBOperation* a, const char* operant) : a(a), operant(operant){};
CP2::CP2(VeriPBOperation* a, VeriPBOperation* b, const char* operant) : a(a), b(b), operant(operant){};

std::string CP1::apply(int constraint_id_at_start_of_printing) {
    return a->apply(constraint_id_at_start_of_printing)  
                                   + " " + this->operant;
}

std::string CP2::apply(int constraint_id_at_start_of_printing) {
    return a->apply(constraint_id_at_start_of_printing) + " " 
                                   + b->apply(constraint_id_at_start_of_printing) 
                                   + " " + this->operant ;
}

std::string CPOperand::apply(int constraint_id_at_start_of_printing){
    return std::to_string(this->value < 0 ? abs(this->value) + constraint_id_at_start_of_printing : this->value);
}


//=================================================================================================
// Proof file

void Prooflogger::write_tree_derivation() {
    int constraint_counter_at_start_of_derivations = constraint_counter;
    for(int i = 0; i < tree_derivation.size(); i++){

        // Write RPN
        if(dynamic_cast<RUP*>(tree_derivation[i]) != nullptr) {
            RUP* learned_clause = dynamic_cast<RUP*>(tree_derivation[i]);
            write_learnt_clause(learned_clause->clause);
        } else {
            proof << "p " << tree_derivation[i]->apply(constraint_counter_at_start_of_derivations ) << "\n";
            constraint_counter++;
        }

        // Tree recursively free the tree derivation
        delete_tree_derivation(tree_derivation[i]);
    }
}

void Prooflogger::delete_tree_derivation(VeriPBOperation* node) {
    if(dynamic_cast<CP1*>(node) != nullptr) {
        delete_tree_derivation(dynamic_cast<CP1*>(node)->a);
    } else if(dynamic_cast<CP2*>(node) != nullptr) {
        delete_tree_derivation(dynamic_cast<CP2*>(node)->a);
        delete_tree_derivation(dynamic_cast<CP2*>(node)->b);
    } 
    delete(node);
}

void Prooflogger::write_proof_header(int nbclause) {
    proof << "pseudo-Boolean proof version 1.0\n";
    proof << "f " << nbclause << "\n";
}

void Prooflogger::write_comment(const char* comment) {
    proof<< "* " << comment << "\n";
}

void Prooflogger::write_contradiction() {
    proof << "c " << constraint_counter << "\n";
}

void Prooflogger::write_empty_clause() {
    proof<< "u >= 1;\n";
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
    proof << weight_and_sign << name << " ";
}

void Prooflogger::write_literal_assignment(lbool assignment, int var) {

    // Sign
    std::string sign = assignment == l_True? "" : "~";

    // Variable symbol
    std::string symbol = var_name(var);

    // Write
    proof << sign << symbol << " ";
}

void Prooflogger::write_witness(Lit literal) {
    std::string name = var_name(var(literal));
    proof<< name << " -> " << std::to_string(sign(literal) == 0);
}

void Prooflogger::write_clause(vec<Lit>& clause) {
    for (int i = 0; i < clause.size(); i++) write_literal(clause[i]);
}

void Prooflogger::write_learnt_clause(vec<Lit>& clause) {
    proof << "u ";
    write_clause(clause);
    proof << " >= 1;\n";
    constraint_counter++;
}

void Prooflogger::write_linkingVar_clause(vec<Lit>& clause) {
    int variable = var(clause[0]);
    int constraint_id = C2_store[variable];
    if(constraint_id != 0) {
        proof << "p " << constraint_id << " " << last_bound_constraint_id << " + s\n";
        constraint_counter++;
    }
    write_learnt_clause(clause);
}

void Prooflogger::write_bound_update(vec<lbool>& model) {
    proof<< "o ";
    for(int i = 0; i < model.size(); i++) write_literal_assignment(model[i], i);
    proof << "\n";

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
    proof << "\n";
    constraint_counter++;
}

void Prooflogger::write_P1_sub_red_cardinality(int var, int sigma, int from, int to) {
    int weight = (to-from+1)-(sigma - 1);
    proof << "red ";
    for(int i = from; i < to+1; i++) {
        write_literal(~Lit(i));
    }
    proof << weight << " " << var_name(var) << " >= " << weight << "; ";
    write_witness(Lit(var));
    proof << "\n";
    constraint_counter++;
    C1_store[var] = constraint_counter;
    C1_weight_store[var] = weight;
}

void Prooflogger::write_P2_sub_red_cardinality(int var, int sigma, int from, int to) {
    int weight = sigma;
    proof << "red ";
    for(int i = from; i < to+1; i++) {
        write_literal(Lit(i));
    }
    proof << weight << " ~" << var_name(var) << " >= " << weight << "; ";
    write_witness(~Lit(var));
    proof << "\n";
    constraint_counter++;
    C2_store[var] = constraint_counter;
    C2_weight_store[var] = weight;
}

void Prooflogger::write_C1(vec<Lit>& definition, int sigma, int from, int to) {
    int first = var(definition[0]);
    int second = var(definition[1]);
    int third = var(definition[2]);

    if(meaningful_names) {

        // If variable does not already have a meaningful name
        if(meaningful_name_LB.find(third) == meaningful_name_LB.end()) {
            meaningful_name_LB[third] = from+1;
            meaningful_name_UB[third] = to+1;
            meaningful_name_n[third] = sigma;
        }
    }

    // Verify if PB cardinality definition exists
    if(C1_store.find(third) == C1_store.end()) {

        // Write a substitution redundancy PB cardinality definition
        write_P1_sub_red_cardinality(third, sigma, from, to);
    }

    // Write derivation of parts
    int total_weight = C1_weight_store[third];
    bool resolved_one = false;
    if(C2_store.find(first) != C2_store.end()) {
        resolved_one = true;
        tree_derivation.push(new CP2(new CPOperand(C1_store[third]), new CPOperand(C2_store[first]), "+"));
        tree_constraint_counter++;
        total_weight += C2_weight_store[first];
    }
    if(C2_store.find(second) != C2_store.end()) {
        int to_add_to = resolved_one? -tree_constraint_counter : C1_store[third];
        resolved_one = true;
        tree_derivation.push(new CP2(new CPOperand(to_add_to), new CPOperand(C2_store[second]), "+"));
        tree_constraint_counter++;
        total_weight += C2_weight_store[second];
    }
    if(resolved_one) {
        tree_derivation.push(new CP1(new CPOperand(-tree_constraint_counter), "s"));
        tree_constraint_counter++;
    }

    // Derivation is done so clause can be written as RUP
    tree_derivation.push(new RUP(definition));
    tree_constraint_counter++;
}

void Prooflogger::write_C2(vec<Lit>& definition, int sigma, int from, int to) {
    int first = var(definition[0]);
    int second = var(definition[1]);
    int third = var(definition[2]);

    if(meaningful_names) {

        // If variable does not already have a meaningful name
        if(meaningful_name_LB.find(third) == meaningful_name_LB.end()) {
            meaningful_name_LB[third] = from+1;
            meaningful_name_UB[third] = to+1;
            meaningful_name_n[third] = sigma;
        }
    }

    // Verify if PB cardinality definition exists
    if(C2_store.find(third) == C2_store.end()) {

        // Write a substitution redundancy PB cardinality definition
        write_P2_sub_red_cardinality(third, sigma, from, to);
    }

    // Write derivation of parts
    int total_weight = C2_weight_store[third];
    bool resolved_one = false;
    if(C1_store.find(first) != C1_store.end()) {
        resolved_one = true;
        tree_derivation.push(new CP2(new CPOperand(C2_store[third]), new CPOperand(C1_store[first]), "+"));
        tree_constraint_counter++;
        total_weight += C1_weight_store[first];
    }
    if(C1_store.find(second) != C1_store.end()) {
        int to_add_to = resolved_one? -tree_constraint_counter : C2_store[third];
        resolved_one = true;
        tree_derivation.push(new CP2(new CPOperand(to_add_to), new CPOperand(C1_store[second]), "+"));
        tree_constraint_counter++;
        total_weight += C1_weight_store[second];
    }
    if(resolved_one) {
        tree_derivation.push(new CP1(new CP2(new CPOperand(-tree_constraint_counter), new CPOperand(2), "d"), "s"));
        tree_constraint_counter++;
    }

    // Derivation is done so clause can be written as RUP
    tree_derivation.push(new RUP(definition));
    tree_constraint_counter++;
}


//=================================================================================================
// OPB file

void Prooflogger::write_OPB_header(int nbvar, int nbsoft, int nbclause) {
    formula_length = nbclause;
    n_variables = nbvar+nbsoft;
    OPB_file << "* #variable= " << nbvar+nbsoft << " #constraint= " << nbclause << "\n";
    OPB_file << "*\n* This MaxSAT instance was automatically generated.\n*\n";
}

void Prooflogger::write_minimise(int start_var, int num) {
    if(num > 0) {
        OPB_file << "min: ";
        for(int i = start_var+1; i < start_var+num+1; i++) {
            OPB_file << "1 x" << i << " ";
        } 
        OPB_file << ";\n";
    }
}

void Prooflogger::write_OPB_constraint(vec<Lit>& constraint) {
    for (int i = 0; i < constraint.size(); i++) {
        if (sign(constraint[i]) == 1)
            constraints << "1 ~x" << var(constraint[i]) + 1 << " ";
        else
            constraints << "1 x" << var(constraint[i]) + 1 << " ";
    }
    constraints << ">= 1;\n";
}