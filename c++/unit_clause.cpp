#pragma once
#include "CNF.cpp"

class Unit_Clause : public SAT_solver
{
protected:
    vector<vector<clause_id>> size_to_clauses;
    CNF *cnf;
    void satisfy(int x)
    {
        assignment[abs(x)] = x > 0 ? true : false;
        for (clause_id id : cnf->var_to_clauses[abs(x)])
        {
            clause &C = cnf->clauses[id];
            fast_erase(size_to_clauses[C.size()], id);
            if (contains(C, -x))
                size_to_clauses[C.size() - 1].push_back(id);
        }
        cnf->satisfy(x);
    }

    void init(CNF *cnf)
    {
        this->cnf = cnf;
        assignment.clear();
        assignment.resize(cnf->N + 1);
        size_to_clauses.clear();
        for (int i = 0; i <= cnf->K; i++)
            size_to_clauses.push_back(vector<clause_id>());
        for (int i = 0; i < cnf->M; i++)
            size_to_clauses[cnf->K].push_back(i);
    }

public:
    string name() override { return "Unit Clause"; }
    vector<bool> get_assignement() { return assignment; }

    bool solve(CNF *cnf) override
    {
        init(cnf);
        for (int i = 0; i < cnf->N; i++)
        {
            /*
            cnf->print();
            cout << "Size to clauses\n";
            print_vec(size_to_clauses);
            cout << "Var to clauses\n";
            print_vec(cnf->var_to_clauses);
            */
            if (size_to_clauses[1].size() > 0)
            {

                clause_id id = size_to_clauses[1][0];
                // assert(cnf->clauses[id].size() == 1);
                int x = cnf->clauses[id][0];
                // cout << "forced: " << x << endl;
                satisfy(x);
            }
            else
            {
                var x = cnf->next_rand_var();
                if (x == NOT_A_VAR)
                    break;
                // cout << "free: " << x << endl;
                if (coin_flip())
                    satisfy(x);
                else
                    satisfy(-x);
            }
        }
        return size_to_clauses[0].size() == 0;
    }
};