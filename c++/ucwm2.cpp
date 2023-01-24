#pragma once
#include "unit_clause_with_majority.cpp"

class Unit_Clause_With_Majority : public Unit_Clause_With_Majority
{
public:
    string name() override { return "Unit Clause With Majority 2"; }
    bool solve(CNF *cnf) override
    {
        Unit_Clause_With_Majority.init(cnf);
        for (int i = 0; i < cnf->N; i++)
        {
            int id = get_unit_clause();
            if (id != NOT_A_CLAUSE)
                satisfy(cnf->clauses[id][0]);
            else
            {
                var x = cnf->next_rand_var();
                if (x == NOT_A_VAR)
                    return true;
                if (positives[x] > negatives[x])
                    satisfy(x);
                else
                    satisfy(-x);
            }
        }
        return false;
    }
};