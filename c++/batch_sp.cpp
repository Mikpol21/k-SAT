#pragma once
#include "SP.cpp"
#include "random_set.cpp"
#include "experimental_bp.cpp"

class BatchSP : public SurveryPropagation
{
    ExperimentalBP bp = ExperimentalBP();

public:
    void print_clause_dist()
    {
        int m3 = 0, m2 = 0, m1 = 0, m0 = 0;
        for (clause_id a = 0; a < cnf->M; a++)
        {
            if (cnf->clauses[a].size() == 0)
                m0++;
            if (cnf->clauses[a].size() == 1 && cnf->clauses[a][0] != SATISFIED)
                m1++;
            if (cnf->clauses[a].size() == 2)
                m2++;
            if (cnf->clauses[a].size() == 3)
                m3++;
        }
        cout << "size 0 -> " << m0 << " size 1 ->" << m1 << " size 2 ->" << m2 << " size 3 ->" << m3 << endl;
    }

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

    int run_walksat(vector<clause> const clauses, const int max_iter = 20)
    {
        RandomSet unsat_clauses = RandomSet(cnf->M);
        for (clause_id a = 0; a < cnf->M; a++)
        {
            if (clauses[a].size() > 0 && clauses[a][0] != 0 && !satisfied(clauses[a]))
                unsat_clauses.insert(a);
        }
        vector<vector<int>> var_to_clauses;
        var_to_clauses.resize(cnf->N + 1);
        for (clause_id a = 0; a < cnf->M; a++)
        {
            for (int x : clauses[a])
            {
                var_to_clauses[abs(x)].push_back(a);
            }
        }
        int min_clauses = unsat_clauses.size();
        cout << "Running WalkSAT with " << unsat_clauses.size() << " unsat clauses" << endl;
        for (int iter = 0; iter < max_iter * cnf->N && unsat_clauses.size() > 0; iter++)
        {
            clause_id a = unsat_clauses.get_random();
            min_clauses = min(min_clauses, (int)unsat_clauses.size());
            // cnf->is_satisfied();
            clause C = clauses[a];
            int x = C[rand() % C.size()];
            // assert(!satisfied(clauses[a]));
            if (coin_flip()) // coin_flip())
            {
                int min_unsat = cnf->M;
                int best_x = 0;
                for (int v : C)
                {
                    assignment[abs(x)] = !assignment[abs(x)];
                    int unsat = 0;
                    for (clause_id jd : var_to_clauses[abs(v)])
                    {
                        if (!satisfied(clauses[jd]) && !unsat_clauses.contains(jd))
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
            int prv = unsat_clauses.size();
            assignment[abs(x)] = !assignment[abs(x)];
            int nxt = unsat_clauses.size();
            if (prv != nxt)
                cout << prv << " != " << nxt << endl;
            for (clause_id jd : var_to_clauses[abs(x)])
            {
                if (!satisfied(clauses[jd]))
                {
                    unsat_clauses.insert(jd);
                    assert(unsat_clauses.contains(jd));
                }
                else
                {
                    unsat_clauses.remove(jd);
                    assert(!unsat_clauses.contains(jd));
                }
            }
        }
        min_clauses = min(min_clauses, (int)unsat_clauses.size());
        return min_clauses;
    }

    int PropagationAndBiases(priority_queue<pair<double, var>> &biases, bool debug = false)
    {
        while (!biases.empty())
            biases.pop();
        int iter = Propagation(debug);
        for (var v = 1; v <= cnf->N; v++)
            if (!cnf->is_erased(v))
            {
                auto b = bias(v);
                biases.push({abs(b.first - b.second), v});
            }
        return iter;
    }

    bool run_bp()
    {
        bp.assignment = this->assignment;
        bool res = bp.solve(cnf);
        this->assignment = bp.assignment;
        cout << "result of bp: " << res << endl;
        return res;
    }

    bool run_multiple_walksats(vector<clause> const clauses)
    {
        for (int i = 0; i < 20; i++)
        {
            vector<bool> assignment_backup = this->assignment;
            for (var v = 1; v <= cnf->N; v++)
                if (!cnf->is_erased(v))
                    assignment[v] = coin_flip();
            cnf->is_satisfied();
            int cl = run_walksat(clauses);
            if (cl == 0)
            {
                cout << "Found solution" << endl;
                return true;
            }
            // this->assignment = assignment_backup;
            cout << "Ended with " << cl << endl;
        }
        return false;
    }

    bool exit_process()
    {
        bool res = cnf->is_satisfied();
        vector<clause> clauses_pre_bp = cnf->clauses;
        cout << "Done with " << cnf->not_satisfied << " not sat clauses" << endl;
        if (cnf->is_satisfied())
            return true;
        bool bp_run = true;
        if (bp_run)
            res = run_bp();
        else
            res = 0 == run_multiple_walksats(clauses_pre_bp);
        assert(res == cnf->is_satisfied());
        cout << "Done with " << cnf->not_satisfied << " not sat clauses" << endl;
        return res;
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
                    cout << "Propagation with " << PropagationAndBiases(biases) << " iterations" << endl;
                    print_clause_dist();
                    cout << "Vars left: " << cnf->N - i << endl;
                    cout << "top bias: " << biases.top().first << endl;
                }
                while (!biases.empty() && cnf->is_erased(biases.top().second))
                    biases.pop();
                if (biases.empty())
                    break;
                int v_opt = biases.top().second;
                // cout << "Satisfying " << v_opt << endl;
                auto b = bias(v_opt);
                if (biases.top().first < 0.1 || (b.first == 0 && b.second == 0))
                {
                    cout << "Running Bp with " << cnf->N - i << " vars left" << endl;
                    return exit_process();
                }

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