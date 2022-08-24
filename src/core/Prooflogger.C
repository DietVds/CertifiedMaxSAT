#include "Prooflogger.h"
#include <string>
#include <sstream>

//=================================================================================================
// Proof file

void Prooflogger::write_proof_header(int nbclause)
{
    proof << "pseudo-Boolean proof version 1.0\n";
    proof << "f " << nbclause << "\n";
}

void Prooflogger::write_comment(const char *comment)
{
    proof << "* " << comment << "\n";
}

void Prooflogger::write_contradiction()
{
    proof << "c " << constraint_counter << "\n";
}

void Prooflogger::write_empty_clause()
{
    proof << "u >= 1;\n";
    constraint_counter++;
    write_contradiction();
}

bool Prooflogger::is_aux_var(int var)
{
    return var + 1 > n_variables;
}

void Prooflogger::store_meaningful_name(int var, int from, int to, int n)
{
    meaningful_name_LB[var] = from + 1;
    meaningful_name_UB[var] = to + 1;
    meaningful_name_n[var] = n;
}

std::string Prooflogger::var_name(int var)
{
    std::string name;
    if (meaningful_names && meaningful_name_UB.find(var) != meaningful_name_UB.end())
    {
        int lb = meaningful_name_LB[var];
        int ub = meaningful_name_UB[var];
        int n = meaningful_name_n[var];
        name = "v" + std::to_string(n) + "_x" + std::to_string(lb) + "_x" + std::to_string(ub);
    }
    else if (is_aux_var(var))
    {
        name = "y";
        name += std::to_string(var + 1);
    }
    else
    {
        name = "x";
        name += std::to_string(var + 1);
    }
    return name;
}

void Prooflogger::write_literal(Lit literal)
{

    // Weight and sign
    std::string weight_and_sign = "1 ";
    weight_and_sign += (sign(literal) == 1 ? "~" : "");

    // Variable symbol
    std::string name = var_name(var(literal));

    // Write
    proof << weight_and_sign << name << " ";
}

void Prooflogger::write_literal_assignment(lbool assignment, int var)
{

    // Sign
    std::string sign = assignment == l_True ? "" : "~";

    // Variable symbol
    std::string symbol = var_name(var);

    // Write
    proof << sign << symbol << " ";
}

void Prooflogger::write_witness(Lit literal)
{
    std::string name = var_name(var(literal));
    proof << name << " -> " << std::to_string(sign(literal) == 0);
}

void Prooflogger::write_clause(vec<Lit> &clause)
{
    for (int i = 0; i < clause.size(); i++)
        write_literal(clause[i]);
}

void Prooflogger::write_clause(Clause &clause)
{
    for (int i = 0; i < clause.size(); i++)
    {
        write_literal(clause[i]);
    }
}

void Prooflogger::write_learnt_clause(vec<Lit> &clause)
{
    proof << "u ";
    write_clause(clause);
    proof << " >= 1;\n";
    constraint_counter++;
}

void Prooflogger::overwrite_learnt_clause(vec<Lit> &clause)
{
    // FIRST: add new clause. THEN, remove old one, THEN increment counter
    auto to_delete = constraint_counter;

    proof << "u ";
    write_clause(clause);
    proof << " >= 1;\n";
    constraint_counter++;

    proof << "del id " << to_delete << "\n";
}

void Prooflogger::implies_rule(int constraint_id, vec<Lit>& c){
    proof << "i " << constraint_id << " ";
    write_clause(c);
    proof << " >= 1;" << std::endl;
}

void Prooflogger::check_last_constraint(vec<Lit>& c){
    implies_rule(constraint_counter, c);
}

void Prooflogger::delete_learnt_clause(Clause &clause)
{
    proof << "del find ";
    write_clause(clause);
    proof << " >= 1;\n";
}

void Prooflogger::delete_constraint(int constraint_id)
{
    proof << "del id " << constraint_id << "\n";
}

void Prooflogger::write_linkingVar_clause(vec<Lit> &clause, int n, int answerNew)
{
    int variable = var(clause[0]);
    int constraint_id = P2_store[variable];
    if (constraint_id != 0)
    {
        proof << "p " << constraint_id << " " << last_bound_constraint_id << " + " << n-answerNew+1  << " d s\n";
        constraint_counter++;
        // The P2-constraint is not necessary anymore and can therefore be deleted.
        delete_constraint(constraint_id);
        P2_store.erase(constraint_id);
    }
}

void Prooflogger::write_bound_update(vec<lbool> &model)
{
    proof << "o ";
    for (int i = 0; i < model.size(); i++)
        write_literal_assignment(model[i], i);
    proof << "\n";

    // Veripb automatically adds an improvement constraint so counter needs to be incremented
    last_bound_constraint_id = ++constraint_counter;
}

void Prooflogger::write_sub_red(vec<Lit> &clause)
{
    // Proof a clause using substitution redundancy using the first literal as witness.
    proof << "red ";
    write_clause(clause);
    proof << ">= 1; ";
    write_witness(clause[0]);
    proof << "\n";
    constraint_counter++;
}

void Prooflogger::write_P1_sub_red_cardinality(int var, int sigma, int from, int to)
{
    proof << "* P1 for " << var_name(var) << " from " << std::to_string(from) << " to " << std::to_string(to) << "\n";
    int weight = (to - from + 1) - (sigma - 1);
    proof << "red ";
    // if(sigma > 0){
    for (int i = from; i < to + 1; i++)
    {
        write_literal(~Lit(i));
    }
    //} else{
    //    weight = 1;
    //}
    if(weight > 0 ){
        proof << weight << " " << var_name(var);
    }
    proof << " >= " << weight << "; ";
    write_witness(Lit(var));
    proof << "\n";
    constraint_counter++;
    P1_store[var] = constraint_counter;
}

void Prooflogger::write_P2_sub_red_cardinality(int var, int sigma, int from, int to)
{
    proof << "* P2 for " << var_name(var) << " from " << std::to_string(from) << " to " << std::to_string(to) << "\n";
    int weight = sigma;
    proof << "red ";
    // if(weight <= (to-from+1)){
    for (int i = from; i < to + 1; i++)
    {
        write_literal(Lit(i));
    }
    //} else{
    //    weight = 1;
    //}
    if(weight > 0){
        proof << weight << " ~" << var_name(var);
    }
    proof << " >= " << weight << "; ";
    write_witness(~Lit(var));
    proof << "\n";
    constraint_counter++;
    P2_store[var] = constraint_counter;
}

void Prooflogger::delete_P(const vec<Lit> &reification_literals, std::map<int, int> &constraint_store)
{
    for (int i = 0; i < reification_literals.size(); i++)
    {
        int variable = var(reification_literals[i]);
        int constraint_id = constraint_store[variable];

        if (constraint_id != 0) // Only remove constraint if it is one that can be deleted.

            // constraint_ids_to_delete.push_front(constraint_id);
            // proof << "del id " << constraint_id << "\n";
            delete_constraint(constraint_id);

        constraint_store.erase(variable);
    }
}

void Prooflogger::delete_P1(const vec<Lit> &reification_literals)
{
    delete_P(reification_literals, P1_store);
}

void Prooflogger::delete_P2(const vec<Lit> &reification_literals)
{
    delete_P(reification_literals, P2_store);
}

void Prooflogger::delete_cardinality_defs(const vec<Lit> &reification_literals)
{
    delete_P1(reification_literals);
    delete_P2(reification_literals);
}

void Prooflogger::write_deletes()
{
    while (!constraint_ids_to_delete.empty())
    {
        int constraint_id = constraint_ids_to_delete.front();

        proof << "del id " << constraint_id << "\n";
        constraint_ids_to_delete.pop_front();
    }
}

void Prooflogger::write_C1(vec<Lit> &definition, int sigma, int from, int to)
{

    int first = var(definition[0]);
    int second = var(definition[1]);
    int third = var(definition[2]);

    proof << "* C1 ";
    for (int i = 0; i < 3; i++)
        write_literal(definition[i]);
    proof << "\n";

    // Write derivation of parts
    proof << "p " << P1_store[third];
    if (P2_store.find(first) != P2_store // Don't write the P1/P2 definitions for sigma=1, since the counting variable is then equal to the variable itself.
                                    .end())
    {
        proof << " " << P2_store[first] << " + ";
    }
    if (P2_store.find(second) != P2_store.end())
    {
        proof << " " << P2_store[second] << " +";
    }

    // In any case, we print the result. So that addClause does not ahve to  do logging afterwards
    proof << " s\n";
    constraint_counter++;
    // Should not be deleted! Goes to solver
    // constraint_ids_to_delete.push_front(constraint_counter);
}

void Prooflogger::add_P1_as_clause(Var var)
{
    proof << "p " << P1_store[var] << "\n";
    constraint_counter++;
}

void Prooflogger::add_P2_as_clause(Var var)
{
    proof << "p " << P2_store[var] << "\n";
    constraint_counter++;
}

void Prooflogger::write_C2(vec<Lit> &definition, int sigma, int from, int to)
{
    int first = var(definition[0]);
    int second = var(definition[1]);
    int third = var(definition[2]);

    proof << "* C2 ";
    for (int i = 0; i < 3; i++)
        write_literal(definition[i]);
    proof << "\n";

    // Write derivation of parts
    proof << "p " << P2_store[third];
    if (P1_store.find(first) != P1_store.end())
    {
        proof << " " << P1_store[first] << " + ";
    }
    if (P1_store.find(second) != P1_store.end())
    {
        proof << " " << P1_store[second] << " + ";
    }
    proof << " s\n";
    constraint_counter++;
    // Should not be deleted! Goes to solver
    // constraint_ids_to_delete.push_front(constraint_counter);
}

void Prooflogger::genCardinalDefinitions(int from, int to, vec<Lit> &linkingVar)
{
    int inputSize = to - from + 1;
    proof << "* genCardinalDefinitions from = " << std::to_string(from + 1) << " to = " << std::to_string(to + 1) << "\n";

    if (inputSize > 1)
    {
        for (int sigma = 0; sigma <= inputSize + 1; sigma++)
        {

            // Verify if PB cardinality definition exists
            if (P1_store.find(var(linkingVar[sigma])) == P1_store.end())
            {
                // Write a substitution redundancy PB cardinality definition
                write_P1_sub_red_cardinality(var(linkingVar[sigma]), sigma, from, to);
            }

            // Verify if PB cardinality definition exists
            if (P2_store.find(var(~linkingVar[sigma])) == P2_store.end())
            {
                // Write a substitution redundancy PB cardinality definition
                write_P2_sub_red_cardinality(var(~linkingVar[sigma]), sigma, from, to);
            }
        }
    }
    else
    {
        // Don't write the P1/P2 definitions for sigma=1, since the counting variable is then equal to the variable itself.
        // Also, P1 constraint for n = 0 is not written, because it is never used (C2 for n=0 is never created and deriving C1 only uses C2 constraints from its children nodes).
        // The same reasoning is valid for P2 for n = 2.
        write_P2_sub_red_cardinality(var(~linkingVar[0]), 0, from, to);
        write_P1_sub_red_cardinality(var(linkingVar[2]), 2, from, to);
    }
}

void Prooflogger::write_clause_as_comment(vec<Lit> &clause)
{
    proof << "* ";
    write_clause(clause);
    proof << ">= 1;\n";
}

void Prooflogger::write_clause_as_comment(Clause &clause)
{
    proof << "* ";
    write_clause(clause);
    proof << " >= 1;\n";
}