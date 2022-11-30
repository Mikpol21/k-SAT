#pragma once
#include "CNF.cpp"

class CNFwithMajority : public CNF
{
public:
    vector<int> positives, negatives;
    CNFwithMajority(CNF *cnf) : CNF(cnf->N, cnf->M, cnf->K, cnf->clauses)
    {
        positives.resize(N + 1);
        negatives.resize(N + 1);
        for (int i = 1; i <= N; i++)
        {
            positives[i] = 0;
            negatives[i] = 0;
        }
        for (int i = 0; i < M; i++)
        {
            clause &C = clauses[i];
            for (int j = 0; j < C.size(); j++)
            {
                if (C[j] > 0)
                    positives[C[j]]++;
                else
                    negatives[-C[j]]++;
            }
        }
    }

protected:
    inline void on_clause_erase(clause_id id, var x)
    {
        if (x > 0)
            positives[x]--;
        else
            negatives[-x]--;
    }
};

class Unit_Clause_With_Majority : public SAT_solver
{
public:
    string name() override { return "Unit Clause With Majority"; }
    bool solve(CNF *cnf_2) override
    {
        CNFwithMajority *cnf = new CNFwithMajority(cnf_2);
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
                if (cnf->positives[x] >= cnf->negatives[x])
                    cnf->satisfy(x);
                else
                    cnf->satisfy(-x);
            }
            // cnf->print();
        }
        bool result = cnf->is_satisfied();
        delete cnf;
        return result;
    }
};