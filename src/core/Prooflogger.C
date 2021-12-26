#include "Prooflogger.h"

//=================================================================================================
// Expression initialisers:

Single::Single(int constraint_id) : constraint_id(constraint_id){};
Operation::Operation(Expression* a, Expression* b, const char* operant) : a(a), b(b), operant(operant){};

//=================================================================================================
// Proof file

void Prooflogger::write_proof_file() {

    if(meaningful_names) {

        // Loop over all variables that have a meaningful name
        std::string proof_string;
        std::string simplified_name;
        std::string to_replace;
        int lb,ub,n;
        for(std::map<int,int>::iterator it = meaningful_name_LB.begin(); it != meaningful_name_LB.end(); it++) {
            proof_string = proof.str();

            lb = meaningful_name_LB[it->first];
            ub = meaningful_name_UB[it->first];
            n = meaningful_name_n[it->first];

            simplified_name = "v" + std::to_string(n) + "_x" + std::to_string(lb) + "_x" + std::to_string(ub); 
            to_replace = "y" + std::to_string(it->first+1); 

            // Replace them with their meaningful name
            proof_string = std::regex_replace(proof_string, std::regex(to_replace), simplified_name);

            // Save as proof
            proof.str(proof_string);
        }
    }

    // Write proof to proof file
    std::ofstream proof_file(proof_file_name);
    proof_file << proof.rdbuf();

    // Close
    proof_file.close();
}

void Prooflogger::write_tree_derivation() {
    // Loop over tree_derivation expressions
    // tree_derivation.apply();
    int constraint_counter_at_start_of_derivations = constraint_counter;
    for(int i = 0; i < tree_derivation.size(); i++){
        proof << "p " << tree_derivation[i]->apply(constraint_counter_at_start_of_derivations ) << "\n";
        constraint_counter++;
    }

    std::map<int, int>::iterator it = constraint_store.begin();

    while(it != constraint_store.end()){
        int var = it->first;
        int constraint_id = it->second;

        if(constraint_id < 0){
            constraint_store[var] = abs(constraint_id) + constraint_counter_at_start_of_derivations;
        }
        it++;
    }
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
    return var + 1 > formula_length + n_variables;
}

void Prooflogger::write_literal(Lit literal) {

    // Weight and sign
    std::string weight_and_sign = "1 ";
    weight_and_sign += (sign(literal) == 1 ? "~" : "");

    // Variable symbol
    std::string symbol = is_aux_var(var(literal))? "y" : "x"; 
    symbol += std::to_string(var(literal)+1);

    // Write
    proof << weight_and_sign << symbol << " ";
}

void Prooflogger::write_literal_assignment(lbool assignment, int var) {

    // Sign
    std::string sign = assignment == l_False ? "~" : "";

    // Variable symbol
    std::string symbol = "x";
    symbol += std::to_string(var+1);

    // Write
    proof << sign << symbol << " ";
}

void Prooflogger::write_witness(Lit literal) {
    proof<< " y" << var(literal)+1 << " -> " << std::to_string(sign(literal) == 0);
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
    int constraint_id = constraint_store[variable];
    int coeff = coeff_store.find(constraint_id) != coeff_store.end() ? coeff_store[constraint_id] : 1;
    proof << "p " << constraint_id << " " << coeff - 1 << " d " << last_bound_constraint_id << " +\n";
    constraint_counter++;
    write_learnt_clause(clause);
}

void Prooflogger::write_bound_update(vec<lbool>& model) {
    proof<< "o ";
    for(int i = 0; i < model.size(); i++) write_literal_assignment(model[i], i);
    proof << "\n";

    // Veripb automatically adds an improvement constraint so counter needs to be incremented
    last_bound_constraint_id = ++constraint_counter;
}

void Prooflogger::write_unit_sub_red(vec<Lit>& definition) {
    if(simplify) {
        unit_store[var(definition[0])] = sign(definition[0]);
        return;
    } else {
        proof << "red ";
        write_clause(definition);
        proof << " >= 1;";
        write_witness(definition[0]);
        proof << "\n";
        constraint_counter++;
    }
}

void Prooflogger::write_C2_sum(vec<int>& constraint_ids, int third, int from, int to) {
    /*
        This method writes the summation of all the C2 constraints for a certain ~vX_xFROM_xTO
        --> 'third' is the variable number of vN_xFROM_xTO
    */
    if(constraint_ids.size() > 1) {

        // Need to keep track of coeff for vN_xFROM_xTO
        int total_coeff = 0;

        // Write first two manually
        Expression* expression = new Operation(new Single(constraint_ids[0]), new Single(constraint_ids[1]), "+");
        total_coeff += coeff_store.find(constraint_ids[0]) != coeff_store.end() ? coeff_store[constraint_ids[0]] : 1; 
        total_coeff += coeff_store.find(constraint_ids[1]) != coeff_store.end() ? coeff_store[constraint_ids[1]] : 1; 

        // Loop for others
        for(int i = 2; i < constraint_ids.size(); i++) {
            expression = new Operation(expression, new Single(constraint_ids[i]), "+");
            total_coeff += coeff_store.find(constraint_ids[i]) != coeff_store.end() ? coeff_store[constraint_ids[i]] : 1;
        }
        tree_derivation.push(expression);
        tree_constraint_counter++;

        // Store resulting constraint id
        constraint_store[third] = -tree_constraint_counter;

        // Store sum of coeffs 
        coeff_store[-tree_constraint_counter] = total_coeff;
    }
}

int Prooflogger::write_C_sub_red(vec<Lit>& definition, int sigma, int from, int to) {
    /*
        This method writes the substitution redundancy line for C1 and C2 constraints
        --> it keeps track of meaning full names
        --> if simplify is set to true it automatically skips trivially true constraints and unit variables
        --> through summing with previous C2's it resolves variables such that veripb gets the actual cardinality constraints
    */

    if(simplify) {
        // First verify if clause should be written at all
        for (int i = 0; i < definition.size(); i++) {
            // If the sign is the same as the one in the store then the clause is trivially true
            if(unit_store.find(var(definition[i])) != unit_store.end() && unit_store[var(definition[i])] == sign(definition[i])) return -1;
        }
    }

    if(meaningful_names) {
        int first = var(definition[0]);
        int second = var(definition[1]); 
        int third = var(definition[2]);

        // If variable does not already have a meaningful name
        if(meaningful_name_LB.find(third) == meaningful_name_LB.end()) {
            meaningful_name_LB[third] = from+1;
            meaningful_name_UB[third] = to+1;
            meaningful_name_n[third] = sigma;
        }
    }

    // Write sub red line
    std::string variable;
    proof<< "red ";
    for (int i = 0; i < definition.size(); i++) {
        
        // If the literal is now found in the store then its sign was different, hence it shouldn't be written 
        if(!simplify || unit_store.find(var(definition[i])) == unit_store.end()) {
            write_literal(definition[i]);
        }
    }
    proof << " >= 1;";
    write_witness(definition[2]);
    proof << "\n";
    constraint_counter++;

    // Only resolve C2's and don't resolve v0_xFROM_xTO because they can be unit propagated
    if(sign(definition[2]) == 1 && sigma > 0) {
        int first = var(definition[0]);
        int second = var(definition[1]);
        int third = var(definition[2]);
        bool resolved_one = false;

        // If first needs to be resolved
        if(first > formula_length + n_variables && constraint_store.find(first) != constraint_store.end()) {
            resolved_one = true;
            int first_constraint_id = constraint_store[first];
            int first_coeff = coeff_store.find(first_constraint_id) != coeff_store.end()? coeff_store[first_constraint_id] : 1;

            tree_derivation.push(new Operation(new Operation(new Single(constraint_counter), new Single(first_coeff), "*"),
                                                new Single(first_constraint_id),
                                                "+"));
            tree_constraint_counter++;
            coeff_store[tree_constraint_counter] = first_coeff;
        }

        // If second needs to be resolved
        if(second > formula_length + n_variables && constraint_store.find(second) != constraint_store.end()) {
            resolved_one = true;
            int second_constraint_id = constraint_store[second];
            int second_coeff = coeff_store.find(second_constraint_id) != coeff_store.end()? coeff_store[second_constraint_id] : 1;
            int third_coeff = coeff_store.find(constraint_counter) != coeff_store.end()? coeff_store[constraint_counter] : 1;

            tree_derivation.push(new Operation(new Operation(new Single(constraint_counter), new Single(second_coeff), "*"),
                                                new Operation(new Single(second_constraint_id), new Single(third_coeff), "*"),
                                                "+"));
            tree_constraint_counter++;
            coeff_store[tree_constraint_counter] = third_coeff * second_coeff;
        }

        // If no resolving had to be done
        if(resolved_one) constraint_store[third] -tree_constraint_counter;
        else constraint_store[third] = constraint_counter;
    }
    return constraint_counter;
}

//=================================================================================================
// OPB file

void Prooflogger::write_OPB_header(int nbvar, int nbclause) {
    formula_length = nbclause;
    n_variables = nbvar;
    OPB_file << "* #variable= " << nbvar+nbclause << " #constraint= " << nbclause << "\n";
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

void Prooflogger::write_OPB_constraint(vec<Lit>& constraint, int weight) {
    for (int i = 0; i < constraint.size(); i++) {
        if (sign(constraint[i]) == 1)
            constraints << "1 ~x" << var(constraint[i]) + 1 << " ";
        else
            constraints << "1 x" << var(constraint[i]) + 1 << " ";
    }
    constraints << " >= " << weight << " ;\n";
}



std::string Operation::apply(int constraint_id_at_start_of_printing) {
    //Operation(Expression* a, Expression* b, const char* operant)
    return a->apply(constraint_id_at_start_of_printing) + " " 
                + b->apply(constraint_id_at_start_of_printing) 
                + " " + this->operant ;
}

std::string Single::apply(int constraint_id_at_start_of_printing){
    std::cout << this->constraint_id << "\n";
    return std::to_string(this->constraint_id < 0 ? abs(this->constraint_id) +  constraint_id_at_start_of_printing : this->constraint_id);
}