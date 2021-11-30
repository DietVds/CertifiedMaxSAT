#include "Prooflogger.h"

//=================================================================================================
// File methods

void Prooflogger::open()
{
    proof_file.open(proof_file_name);
}

void Prooflogger::close()
{
    proof_file.close();
}

void Prooflogger::set_name(const char* new_name)
{
    proof_file_name = new_name;
}


//=================================================================================================
// Writing methods


void Prooflogger::write_header(int nClauses) {
    proof_file << "pseudo-Boolean proof version 1.0\n";
    proof_file << "f " << nClauses << ";\n";
}

void Prooflogger::write_comment(const char* comment) {
    proof_file << "c " << comment << "\n";
}

void Prooflogger::derived_empty_clause() {
    proof_file << "u >= 1;\n";
    constraint_counter++;
    write_contradiction();
}

void Prooflogger::write_sub_red(vec<Lit>& definition, bool ass, int start_x) {
    const char* symbol = "y";
    proof_file << "red ";
    for (int i = 0; i < definition.size(); i++) {
        if(i == start_x) symbol = "x";
        if (sign(definition[i]) == 1)
            proof_file << "1 ~" << symbol << var(definition[i]) + 1 << " ";
        else
            proof_file << "1 " << symbol << var(definition[i]) + 1 << " ";
    }
    proof_file << " >= 1; y" << var(definition[0])+1 << " -> " << ass << "\n";
    constraint_counter++;
}

void Prooflogger::write_learnt_clause(vec<Lit>& clause) {
    proof_file << "u ";
    for (int i = 0; i < clause.size(); i++) {
        if (sign(clause[i]) == 1)
            proof_file << "1 ~x" << var(clause[i]) + 1 << " ";
        else
            proof_file << "1 x" << var(clause[i]) + 1 << " ";
    }
    proof_file << " >= 1;\n";
    constraint_counter++;
}

void Prooflogger::write_contradiction() {
    proof_file << "c " << constraint_counter;
}

void Prooflogger::write_delete(int number) {
    proof_file << "d " << number << "\n";
    constraint_counter--;
}