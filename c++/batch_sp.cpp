#pragma once
#include "SP.cpp"
#include "random_set.cpp"
#include "experimental_bp.cpp"
#include "walksat.cpp"

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
        cout << "Running BP" << endl;
        bp.assignment = this->assignment;
        bool res = bp.solve(cnf);
        this->assignment = bp.assignment;
        cout << "Unsat clauses: " << cnf->not_satisfied << endl;
        return res;
    }

    bool run_walksat(const vector<clause> &clauses, vector<bool> seed = vector<bool>())
    {
        WalkSAT ws = WalkSAT(50 * cnf->N, 30, false);
        cout << "Running walksat" << endl;
        int unsat;
        if (!seed.empty())
            unsat = ws.solve(clauses, cnf->M, cnf->N, seed);
        else
            unsat = ws.solve(clauses, cnf->M, cnf->N);
        for (var v = 1; v <= cnf->N; v++)
            if (!cnf->is_erased(v))
                this->assignment[v] = ws.assignment[v];
        cout << "Unsat clauses: " << unsat << endl;
        return unsat == 0;
    }

    bool exit_process()
    {
        bool res = cnf->is_satisfied();
        vector<clause> clauses_pre_bp = cnf->clauses;
        cout << "Left with " << cnf->not_satisfied << " unat clauses" << endl;
        print_clause_dist();
        if (cnf->is_satisfied())
            return true;
        bool bp_run = false;
        if (bp_run)
            res = run_bp();
        else
            res = run_walksat(cnf->clauses);
        cout << "Satisfied: " << res << endl;
        return res;
    }

    bool solve(CNF *cnf)
    {
        init(cnf);
        int t = cnf->N;
        int max_restarts = 0;
        float rate = 0.04;
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
                    // cout << "Propagation with " << PropagationAndBiases(biases) << " iterations" << endl;
                    // print_clause_dist();
                    // cout << "Vars left: " << cnf->N - i << endl;
                    // cout << "top bias: " << biases.top().first << endl;
                }
                while (!biases.empty() && cnf->is_erased(biases.top().second))
                    biases.pop();
                if (biases.empty())
                    break;
                int v_opt = biases.top().second;
                auto b = bias(v_opt);
                if (biases.top().first < 0.1 || (b.first == 0 && b.second == 0))
                {
                    cout << "\nSP concluded with " << cnf->N - i << " vars left" << endl;
                    return exit_process();
                }

                // cout << i << ": " << v_opt << " -> " << b.first << " / " << b.second << " , " << biases.top().first << endl;
                if (b.first < b.second)
                    satisfy(v_opt);
                else
                    satisfy(-v_opt);
            }
        }
        return cnf->is_satisfied();
    }
};