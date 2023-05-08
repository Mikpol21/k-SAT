#pragma once
#include "CNF.cpp"
#include "unit_clause.cpp"

class Unit_Clause_With_Majority : public Unit_Clause
{
protected:
    vector<int> positives, negatives;
    void init(CNF *cnf)
    {
        Unit_Clause::init(cnf);
        positives.clear(), negatives.clear();
        for (int i = 0; i <= cnf->N; i++)
            positives.push_back(0), negatives.push_back(0);
        for (int id = 0; id < cnf->M; id++)
        {
            clause &C = cnf->clauses[id];
            for (int x : C)
            {
                if (x > 0)
                    positives[x]++;
                else
                    negatives[-x]++;
            }
        }
    }

    void satisfy(int x)
    {
        for (clause_id id : cnf->var_to_clauses[abs(x)])
        {
            clause &C = cnf->clauses[id];
            if (contains(C, -x))
                continue;
            // otherwise C is bound to be erased
            for (int y : C)
            {
                if (y > 0)
                    positives[y]--;
                else
                    negatives[-y]--;
            }
        }
        Unit_Clause::satisfy(x);
    }

public:
    string name() override { return "Unit Clause With Majority"; }
    bool solve(CNF *cnf) override
    {
        init(cnf);
        for (int i = 0; i < cnf->N; i++)
        {
            int id = get_unit_clause();
            if (id != NOT_A_CLAUSE)
                satisfy(cnf->clauses[id][0]);
            else
            {
                var x = cnf->next_rand_var();
                if (x == NOT_A_VAR)
                    break;
                if (positives[x] >= negatives[x])
                    satisfy(x);
                else
                    satisfy(-x);
            }
        }
        return cnf->is_satisfied();
    }
};