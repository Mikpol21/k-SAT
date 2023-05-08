#pragma once
#include "unit_clause.cpp"

class PureLiteral : public Unit_Clause
{
protected:
    queue<var> pure_literals;
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
        while (!pure_literals.empty())
            pure_literals.pop();
        for (var i = 1; i <= cnf->N; i++)
        {
            if (positives[i] == 0 && negatives[i] != 0)
                pure_literals.push(-i);
            if (negatives[i] == 0 && positives[i] != 0)
                pure_literals.push(i);
        }
    }

    var get_pure_literal()
    {
        while (!pure_literals.empty() && cnf->is_erased(pure_literals.front()))
            pure_literals.pop();
        if (pure_literals.empty())
            return NOT_A_VAR;
        var x = pure_literals.front();
        pure_literals.pop();
        return x;
    }

    void satisfy(int x)
    {
        for (clause_id id : cnf->var_to_clauses[abs(x)])
        {
            clause &C = cnf->clauses[id];
            if (!contains(C, x))
                continue;
            for (int y : C)
            {
                if (x == y)
                    continue;
                if (y > 0)
                {
                    positives[y]--;
                    if (positives[y] == 0)
                        pure_literals.push(-y);
                }
                else
                {
                    negatives[-y]--;
                    if (negatives[-y] == 0)
                        pure_literals.push(-y);
                }
            }
        }
        Unit_Clause::satisfy(x);
    }

public:
    string name() override { return "Pure Literal"; }
    bool solve(CNF *cnf) override
    {
        init(cnf);
        for (int i = 0; i < cnf->N; i++)
        {
            int id = get_unit_clause();
            var x = get_pure_literal();
            if (id != NOT_A_CLAUSE)
                satisfy(cnf->clauses[id][0]);
            else if (x != NOT_A_VAR)
                satisfy(x);
            else
            {
                x = cnf->next_rand_var();
                if (x == NOT_A_VAR)
                    break;
                if (coin_flip())
                    satisfy(x);
                else
                    satisfy(-x);
            }
        }
        return cnf->is_satisfied();
    }
};