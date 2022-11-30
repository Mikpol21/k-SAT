#pragma once
#include "CNF.cpp"

class Unit_Clause : public SAT_solver
{
public:
    string name() override { return "Unit Clause"; }
    bool solve(CNF *cnf) override
    {
        for (int t = 0; t < cnf->N; t++)
        {
            clause_id id = cnf->get_unit_clause();
            if (id != NOT_A_CLAUSE)
            {
                var x = cnf->clauses[id][0];
                cnf->satisfy(x);
            }
            else
            {
                var x = cnf->get_any_var();
                if (x == NOT_A_VAR)
                    break;
                if (coin_flip())
                    cnf->satisfy(x);
                else
                    cnf->satisfy(-x);
            }
            // cnf->print();
        }
        return cnf->is_satisfied();
    }
};