#include "Prooflogger.h"

//=================================================================================================
// Proof writing methods


void Prooflogger::write_proof_header(int nClauses) {
    formula_length = nClauses;
    proof_file << "pseudo-Boolean proof version 1.0\n";
    proof_file << "f " << nClauses << "\n";
}

void Prooflogger::write_comment(const char* comment) {
    proof_file << "* " << comment << "\n";
}

void Prooflogger::derived_empty_clause() {
    proof_file << "u >= 1;\n";
    constraint_counter++;
    write_contradiction();
}

const char* Prooflogger::literal_symbol(Lit lit) {
    if(var(lit) + 1 > formula_length) return "y";
    else return "x";
}

void Prooflogger::write_sub_red(vec<Lit>& definition, bool ass) {
    const char* symbol;
    proof_file << "red ";
    for (int i = 0; i < definition.size(); i++) {
        symbol = literal_symbol(definition[i]);
        if (sign(definition[i]) == 1)
            proof_file << "1 ~" << symbol << var(definition[i]) + 1 << " ";
        else
            proof_file << "1 " << symbol << var(definition[i]) + 1 << " ";
    }
    proof_file << " >= 1; y" << var(definition[0])+1 << " -> " << ass << "\n";
    constraint_counter++;
}

void Prooflogger::write_learnt_clause(vec<Lit>& clause) {
    const char* symbol;
    proof_file << "u ";
    for (int i = 0; i < clause.size(); i++) {
        symbol = literal_symbol(clause[i]);
        if (sign(clause[i]) == 1)
            proof_file << "1 ~" << symbol << var(clause[i]) + 1 << " ";
        else
            proof_file << "1 " << symbol << var(clause[i]) + 1 << " ";
    }
    proof_file << " >= 1;\n";
    constraint_counter++;
}

void Prooflogger::write_contradiction() {
    proof_file << "c " << constraint_counter << "\n";
}

void Prooflogger::write_delete(int number) {
    proof_file << "d " << number << "\n";
    constraint_counter--;
}

//=================================================================================================
// OPB writing methods

void Prooflogger::write_OPB_header(int nbvar, int nbclause) {
    OPB_file << "* #variable= " << nbvar << " #constraint= " << nbclause << "\n";
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
            constraints << "-1 x" << var(constraint[i]) + 1 << " ";
        else
            constraints << "1 x" << var(constraint[i]) + 1 << " ";
    }
    constraints << " >= " << weight << " ;\n";
}
