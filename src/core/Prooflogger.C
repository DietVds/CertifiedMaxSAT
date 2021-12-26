#include "Prooflogger.h"

//=================================================================================================
// Proof writing methods


void Prooflogger::write_proof_header(int nbclause) {
    proof << "pseudo-Boolean proof version 1.0\n";
    proof << "f " << nbclause << "\n";
}

void Prooflogger::write_comment(const char* comment) {
    proof<< "* " << comment << "\n";
}

void Prooflogger::write_empty_clause() {
    proof<< "u >= 1;\n";
    constraint_counter++;
    write_contradiction();
}

const char* Prooflogger::literal_symbol(int var) {
    if(var + 1 > formula_length + n_variables) return "y";
    else return "x";
}

void Prooflogger::write_unit_sub_red(vec<Lit>& definition) {
    if(simplify) {
        unit_store[var(definition[0])] = sign(definition[0]);
        return;
    } else {
        std::string variable;
        proof<< "red ";

        // Write variable
        if (sign(definition[0]) == 1)
            proof << "1 ~y" << var(definition[0])+1 << " ";
        else
            proof << "1 y" << var(definition[0])+1 << " ";
    }
    proof<< " >= 1; y" << var(definition[0])+1 << " -> " << std::to_string(sign(definition[0]) == 0) << "\n";
    constraint_counter++;
}

void Prooflogger::write_C2_v1_sum(vec<int>& constraint_ids, int key) {
    int total_coeff = 0;
    if(constraint_ids.size() > 1) {
        proof << "p " << constraint_ids[0] << " " << constraint_ids[1] << " + ";
        total_coeff += coeff_store.find(constraint_ids[0]) != coeff_store.end() ? coeff_store[constraint_ids[0]] : 1; 
        total_coeff += coeff_store.find(constraint_ids[1]) != coeff_store.end() ? coeff_store[constraint_ids[1]] : 1; 
        for(int i = 2; i < constraint_ids.size(); i++) {
            proof << constraint_ids[i] << " + ";
            total_coeff += coeff_store.find(constraint_ids[i]) != coeff_store.end() ? coeff_store[constraint_ids[i]] : 1;
        }
        proof << "\n";
        constraint_counter++;

        // Store constraint id
        constraint_store[key] = constraint_counter;

        // Store sum of coeffs 
        coeff_store[key] = total_coeff;
    }
}

int Prooflogger::write_C_sub_red(vec<Lit>& definition, int sigma, int from, int to) {

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
        int third = var(definition[definition.size()-1]);

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

            // Define variable name
            variable = literal_symbol(var(definition[i])) + std::to_string(var(definition[i])+1);

            // Write variable
            if (sign(definition[i]) == 1)
                proof<< "1 ~" << variable << " ";
            else
                proof<< "1 " << variable << " ";
        }
    }
    proof<< " >= 1; " << variable << " -> " << std::to_string(sign(definition[definition.size()-1]) == 0) << "\n";
    proof << "* " << std::to_string(constraint_counter+1) << "\n";
    constraint_counter++;

    // For C2's write resolving
    if(sign(definition[2]) == 1 && sigma > 0) {
        int first = var(definition[0]);
        int second = var(definition[1]);
        int third = var(definition[2]);

        // If first needs to be resolved
        if(first > formula_length + n_variables && constraint_store.find(first) != constraint_store.end()) {
            int constraint_id = constraint_store[first];
            int coeff = coeff_store.find(constraint_id) != coeff_store.end()? coeff_store[constraint_id] : 1;

            proof << "p ";
            proof << constraint_counter; 
            proof << " ";
            proof << coeff;
            proof << " * ";
            proof << constraint_id;
            proof << " +\n";
            constraint_counter++;
            coeff_store[constraint_counter] = coeff;
        }

        // If second needs to be resolved
        if(second > formula_length + n_variables && constraint_store.find(second) != constraint_store.end()) {
            int constraint_id = constraint_store[second];
            int coeff_second = coeff_store.find(constraint_id) != coeff_store.end()? coeff_store[constraint_id] : 1;
            int coeff_third = coeff_store.find(constraint_counter) != coeff_store.end()? coeff_store[constraint_counter] : 1;

            proof << "p ";
            proof << constraint_counter; 
            proof << " ";
            proof << coeff_second;
            proof << " * ";
            proof << constraint_id;
            proof << " ";
            proof << coeff_third;
            proof << " * ";
            proof << " +\n";
            constraint_counter++;
            coeff_store[constraint_counter] = coeff_third * coeff_second;
        }
        constraint_store[third] = constraint_counter;
    }
    return constraint_counter;
}

void Prooflogger::write_bound_update(vec<lbool>& model) {
    const char* symbol;
    proof<< "o ";

    for(int i = 0; i < model.size(); i++) {
        symbol = literal_symbol(i);
        if(model[i] == l_True) proof << symbol << i+1 << " ";
        else if(model[i] == l_False) proof << "~" << symbol << i+1 << " ";
    }
    proof << "\n";

    // Veripb automatically adds an improvement constraint so counter needs to be incremented
    constraint_counter++;
}

void Prooflogger::write_learnt_clause(vec<Lit>& clause) {
    const char* symbol;
    proof << "u ";
    for (int i = 0; i < clause.size(); i++) {
        symbol = literal_symbol(var(clause[i]));
        if (sign(clause[i]) == 1)
            proof << "1 ~" << symbol << var(clause[i]) + 1 << " ";
        else
            proof << "1 " << symbol << var(clause[i]) + 1 << " ";
    }
    proof << " >= 1;\n";
    constraint_counter++;
}

void Prooflogger::write_contradiction() {
    proof << "c " << constraint_counter << "\n";
}

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

//=================================================================================================
// OPB writing methods

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
