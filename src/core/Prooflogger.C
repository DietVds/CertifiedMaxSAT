#include "Prooflogger.h"

//=================================================================================================
// Proof writing methods


void Prooflogger::write_proof_header(int nbclause) {
    proof_file << "pseudo-Boolean proof version 1.0\n";
    proof_file << "f " << nbclause << "\n";
}

void Prooflogger::write_order(vec<Lit>& linkingVar) {
    proof_file << "pre_order exp2\n";

    // Auxiliairy variables
    proof_file << "     vars\n";
    proof_file << "         left ";
    for(int i = 0; i < linkingVar.size(); i++) {
        proof_file << "u" << i + 1 << " ";
    }
    proof_file << "\n         right ";
    for(int i = 0; i < linkingVar.size(); i++) {
        proof_file << "v" << i + 1 << " ";
    }
    proof_file << "\n         aux";
    proof_file << "\n     end\n";

    // Constraint
    proof_file << "     def\n       ";
    int current_weight = 1;
    for(int i = linkingVar.size(); i > 0; i--) {
        proof_file << "-" << current_weight << " u" << i << " ";
        proof_file << current_weight << " v" << i << " ";
        current_weight = current_weight * 2;
    }
    proof_file << " >= 0;\n";
    proof_file << "     end\n\n";

    // Transitivity proof
    proof_file << "     transitivity\n";
    proof_file << "         vars\n";
    proof_file << "             fresh_right ";
    for(int i = 0; i < linkingVar.size(); i++) {
        proof_file << "w" << i + 1 << " ";
    }
    proof_file << "\n         end\n";
    proof_file << "         proof\n";
    proof_file << "             proofgoal #1\n";
    proof_file << "                 p 1 2 + 3 +\n";
    proof_file << "                 c -1\n";
    proof_file << "             qed\n";
    proof_file << "         qed\n";
    proof_file << "     end\n";
    proof_file << "end\n";

    // Load
    proof_file << "load_order exp2 ";
    for(int i = linkingVar.size()-1; i >= 0; i--) {
        proof_file << "y" << var(linkingVar[i])+1 << " ";
    }
    proof_file << "\n";
}

void Prooflogger::write_comment(const char* comment) {
    proof_file << "* " << comment << "\n";
}

void Prooflogger::derived_empty_clause() {
    proof_file << "u >= 1;\n";
    constraint_counter++;
    write_contradiction();
}

const char* Prooflogger::literal_symbol(int var) {
    if(var + 1 > formula_length + n_variables) return "y";
    else return "x";
}

void Prooflogger::write_sub_red(vec<Lit>& definition, bool ass) {
    const char* symbol;
    proof_file << "red ";
    for (int i = 0; i < definition.size(); i++) {
        symbol = literal_symbol(var(definition[i]));
        if (sign(definition[i]) == 1)
            proof_file << "1 ~" << symbol << var(definition[i]) + 1 << " ";
        else
            proof_file << "1 " << symbol << var(definition[i]) + 1 << " ";
    }
    proof_file << " >= 1; y" << var(definition[definition.size()-1])+1 << " -> " << ass << "\n";
    constraint_counter++;
    //proof_file << "p " << constraint_counter-1 << " " << constraint_counter << " +\n";
    //constraint_counter++;
}

void Prooflogger::write_dom(vec<Lit>& linkingVar, int start, int stop) {
    proof_file << "dom ";

    // Constraint
    for(int i = start; i < stop; i++) {
        proof_file << "1 ~y" << var(linkingVar[i]) + 1 << " ";
    }
    proof_file << ">= " << stop - start << "; ";

    // Witness
    for(int i = start; i < stop; i++) {
        proof_file << "y" << var(linkingVar[i]) + 1 << " -> 0 ";

    }
    proof_file << "\n";

    // TODO: subproof 
    constraint_counter++;
}

void Prooflogger::write_bound_update(vec<lbool>& model) {
    const char* symbol;
    proof_file << "o ";

    for(int i = 0; i < model.size(); i++) {
        symbol = literal_symbol(i);
        if(model[i] == l_True) proof_file << symbol << i+1 << " ";
        else if(model[i] == l_False) proof_file << "~" << symbol << i+1 << " ";
    }
    proof_file << "\n";

    // Veripb automatically adds an improvement constraint so counter needs to be incremented
    constraint_counter++;
}

void Prooflogger::write_learnt_clause(vec<Lit>& clause) {
    const char* symbol;
    proof_file << "u ";
    for (int i = 0; i < clause.size(); i++) {
        symbol = literal_symbol(var(clause[i]));
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
