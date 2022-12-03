#pragma once
#include "CNF.cpp"

class Unit_Clause : public SAT_solver
{
protected:
    queue<clause_id> unit_clauses;
    vector<bool> erased_clauses;
    CNF *cnf;
    void satisfy(int x)
    {
        if (cnf->is_erased(x))
            return;
        assignment[abs(x)] = x > 0 ? true : false;
        for (clause_id id : cnf->var_to_clauses[abs(x)])
        {
            clause &C = cnf->clauses[id];
            if (contains(C, x))
                erased_clauses[id] = true;
            else
            {
                if (C.size() == 2)
                    unit_clauses.push(id);
                if (C.size() == 1)
                    erased_clauses[id] = true;
            }
        }
        cnf->satisfy(x);
    }

    clause_id get_unit_clause()
    {
        while (!unit_clauses.empty() && erased_clauses[unit_clauses.front()])
            unit_clauses.pop();
        if (unit_clauses.empty())
            return NOT_A_CLAUSE;
        clause_id id = unit_clauses.front();
        erased_clauses[id] = true;
        unit_clauses.pop();
        return id;
    }

    void init(CNF *cnf)
    {
        this->cnf = cnf;
        assignment.clear();
        assignment.resize(cnf->N + 1);
        erased_clauses.resize(cnf->M);
        for (int i = 0; i < cnf->M; i++)
            erased_clauses[i] = false;
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
            int id = get_unit_clause();
            if (id != NOT_A_CLAUSE)
                satisfy(cnf->clauses[id][0]);
            else
            {
                var x = cnf->next_rand_var();
                // cout << "satisfying " << x << endl;
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