#pragma once
#include "SP.cpp"
#include "random_set.cpp"
#include "experimental_bp.cpp"

class BatchSP : public SurveryPropagation
{
    ExperimentalBP bp = ExperimentalBP();

public:
    string name()
    {
        return "Batch SP";
    }

    bool satisfied(clause const &C)
    {
        for (int x : C)
            if (assignment[abs(x)] == (x > 0))
                return true;
        return false;
    }

    bool run_walksat(const int max_iter = 100)
    {
        cout << "Walksat kicking off" << endl;
        RandomSet unsat_clauses = RandomSet(cnf->M);
        for (clause_id a = 0; a < cnf->M; a++)
        {
            if (cnf->clauses[a].size() == 0)
            {
                cout << "Fucked up" << endl;
                return false;
            }
            if (!satisfied(cnf->clauses[a]))
                unsat_clauses.insert(a);
        }

        for (int iter = 0; iter < max_iter * cnf->N && unsat_clauses.size() > 0; iter++)
        {
            clause_id a = unsat_clauses.get_random();
            cout << "clause " << a << endl;

            clause &C = cnf->clauses[a];
            int x = C[rand() % C.size()];
            if (coin_flip())
            {
                int min_unsat = cnf->M;
                int best_x = 0;
                for (int v : C)
                {
                    assignment[abs(x)] = !assignment[abs(x)];
                    int unsat = 0;
                    for (clause_id jd : cnf->var_to_clauses[abs(v)])
                    {
                        if (!satisfied(cnf->clauses[jd]))
                            unsat++;
                    }
                    if (unsat < min_unsat)
                    {
                        min_unsat = unsat;
                        best_x = v;
                    }
                    assignment[abs(x)] = !assignment[abs(x)];
                }
                x = best_x;
            }
            cout << "before" << unsat_clauses.size() << endl;
            unsat_clauses.remove(a);
            assignment[abs(x)] = !assignment[abs(x)];
            // unsat_clauses.remove(a);
            for (clause_id jd : cnf->var_to_clauses[abs(x)])
            {
                if (!satisfied(cnf->clauses[jd]))
                    unsat_clauses.insert(jd);
                else
                {
                    unsat_clauses.remove(jd);
                }
            }
            cout << "after" << unsat_clauses.size() << endl;
        }
        for (var v = 1; v <= cnf->N; v++)
            if (assignment[v])
                satisfy(v);
            else
                satisfy(-v);
        return cnf->is_satisfied();
    }

    void PropagationAndBiases(priority_queue<pair<double, var>> &biases)
    {
        while (!biases.empty())
            biases.pop();
        Propagation();
        for (var v = 1; v <= cnf->N; v++)
            if (!cnf->is_erased(v))
            {
                auto b = bias(v);
                biases.push({abs(b.first - b.second), v});
            }
    }

    bool solve(CNF *cnf)
    {
        init(cnf);
        int t = cnf->N;
        int max_restarts = 0;
        float rate = 0.01;
        priority_queue<pair<double, var>> biases;
        for (int i = 0; i < cnf->N; i++, t++)
        {
            int id = get_unit_clause();
            var x = get_pure_literal();
            if (id != NOT_A_CLAUSE)
                satisfy(cnf->clauses[id][0]);
            else if (x != NOT_A_VAR)
                satisfy(x);
            else
            {
                if (t >= rate * (cnf->N - i))
                {
                    t = 0;
                    PropagationAndBiases(biases);

                    if (isnan(biases.top().first))
                    {
                        bp.assignment = this->assignment;
                        bool res = bp.solve(cnf);
                        this->assignment = bp.assignment;
                        // cout << "result of bp: " << res << endl;
                        return res;
                    }
                    /*
                    while (isnan(biases.top().first))
                    {
                        cout << "Crap" << endl;
                        restart_prop();
                        if (max_restarts-- == 0)
                            return run_walksat();
                        PropagationAndBiases(biases);
                    }
                    */
                }
                while (!biases.empty() && cnf->is_erased(biases.top().second))
                    biases.pop();
                if (biases.empty())
                    break;
                int v_opt = biases.top().second;
                // cout << "Satisfying " << v_opt << endl;
                auto b = bias(v_opt);
                // cout << i << ": " << v_opt << " -> " << b.first << " / " << b.second << " , " << biases.top().first << endl;
                if (b.first < b.second)
                    satisfy(v_opt);
                else
                    satisfy(-v_opt);
                // cnf->print();
            }
        }
        return cnf->is_satisfied();
    }
};