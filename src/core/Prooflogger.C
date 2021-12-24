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

void Prooflogger::derived_empty_clause() {
    proof<< "u >= 1;\n";
    constraint_counter++;
    write_contradiction();
}

const char* Prooflogger::literal_symbol(int var) {
    if(var + 1 > formula_length + n_variables) return "y";
    else return "x";
}

void Prooflogger::write_sub_red(vec<Lit>& definition, bool ass) {
    int first = var(definition[0]);
    int second = var(definition[1]); 
    int third = var(definition[definition.size()-1]);

    // If variable does not already have a meaningful name
    if(meaningful_name_LB.find(third) == meaningful_name_LB.end()) {

        // First two are x's
        if(first+1 <= formula_length + n_variables && second <= formula_length + n_variables) {
            meaningful_name_LB[third] = first+1;
            meaningful_name_UB[third] = second+1;
            meaningful_name_n[third] = 1;
        }

        // First is an x
        else if(first+1 <= formula_length + n_variables) {
            meaningful_name_LB[third] = first+1;
            meaningful_name_n[third] = 2;
        }

        // First has a simplified name
        else if(meaningful_name_UB.find(first) != meaningful_name_UB.end()) {

            // Second is an x
            if(second+1 <= formula_length + n_variables) {
                meaningful_name_LB[third] = meaningful_name_LB[first];
                meaningful_name_UB[third] = second+1;
                meaningful_name_n[third] = meaningful_name_n[first];

            // Second is a y
            } else {
                meaningful_name_LB[third] = meaningful_name_LB[first];
                meaningful_name_UB[third] = meaningful_name_UB[first]+1;
                meaningful_name_n[third] = meaningful_name_n[first]+1;
            }
        }

    // Still need upper bound
    } else if(meaningful_name_UB.find(third) == meaningful_name_UB.end()) {
        meaningful_name_UB[third] = second+1;
    }

    // Write sub red line
    std::string variable;
    proof<< "red ";
    for (int i = 0; i < definition.size(); i++) {

        // Define variable name
        variable = literal_symbol(var(definition[i])) + std::to_string(var(definition[i])+1);

        // Write variable
        if (sign(definition[i]) == 1)
            proof<< "1 ~" << variable << " ";
        else
            proof<< "1 " << variable << " ";
    }
    proof<< " >= 1; " << variable << " -> " << std::to_string(sign(definition[definition.size()-1]) == 0) << "\n";
    constraint_counter++;
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

void Prooflogger::write_delete(int number) {
    proof << "d " << number << "\n";
    constraint_counter--;
}

void Prooflogger::write_proof() {

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
