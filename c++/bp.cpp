#pragma once
#include <math.h>
#include "pure_literal.cpp"
#include "stats_keeper.cpp"

class BeliefPropagation : public PureLiteral
{
protected:
    // H[i][a] -> how likely i satisfies a
    // U[id][i] -> how much id influences i
    vector<unordered_map<int, double>> U, H;
    vector<pair<bool, int>> ordering;
    CNF *cnf;

    void init(CNF *cnf)
    {
        this->cnf = cnf;
        H.resize(cnf->N + 1);
        U.resize(cnf->M);
        ordering.clear();
        for (var x = 1; x <= cnf->N; x++)
            ordering.push_back({0, x});
        for (clause_id id = 0; id < cnf->M; id++)
            ordering.push_back({1, id});
        for (clause_id id = 0; id < cnf->M; id++)
            for (int x : cnf->clauses[id])
                U[id][abs(x)] = 0.;
        for (var x = 1; x <= cnf->N; x++)
            for (clause_id id : cnf->var_to_clauses[x])
                H[x][id] = 0.5;
        PureLiteral::init(cnf);
    }

    // O(r*k^2)
    // updating H[i][id] for all clause that contain i
    void update_var(var i)
    {
        assert(i > 0);
        double positives = 0., negatives = 0.;
        for (clause_id id : cnf->var_to_clauses[i])
        {
            if (contains(cnf->clauses[id], i))
                positives += U[id][i];
            else
                negatives += U[id][i];
        }
        for (clause_id id : cnf->var_to_clauses[i])
        {
            if (contains(cnf->clauses[id], i))
                H[i][id] = tanh(positives - U[id][i] - negatives);
            else
                H[i][id] = tanh(negatives - U[id][i] - positives);
        }
    }

    double f(const vector<double> &hs)
    {
        const double epsilon = 0.0001;
        double result = 1.;
        for (double h : hs)
            result *= (1 - h);
        result /= pow(2., hs.size());
        if (result == 1.)
            result -= epsilon;
        result = -0.5 * log(1. - result);
        return result;
    }

    // O(k ^ 2)
    // updating U[id][x] for all x inside clause[id]
    void update_clause(clause_id id)
    {
        clause &C = cnf->clauses[id];
        for (int x : C)
        {
            vector<double> hs;
            for (int y : C)
                if (y != x)
                    hs.push_back(H[abs(y)][id]);
            U[id][abs(x)] = f(hs);
        }
    }

    double signal(var x)
    {
        assert(x > 0);
        double result = 0.;
        for (clause_id id : cnf->var_to_clauses[x])
        {
            if (contains(cnf->clauses[id], x))
                result -= U[id][x];
            else
                result += U[id][x];
        }
        return result;
    }

    pair<double, double> extract_marginal(var x)
    {
        double result = signal(x);
        double pos = (1. - tanh(result)) / 2.;
        double neg = (1. + tanh(result)) / 2.;
        return {neg, pos};
    }

    void BP_round()
    {
        random_shuffle(ordering.begin(), ordering.end());
        for (auto p : ordering)
        {
            if (p.first)
            {
                clause_id id = p.second;
                if (cnf->clauses[id].size() == 0)
                    continue;
                if (cnf->clauses[id].size() == 1)
                {
                    // assert(cnf->clauses[id][0] == SATISFIED);
                    continue;
                }
                update_clause(p.second);
            }
            else if (!cnf->is_erased(p.second))
                update_var(p.second);
        }
    }

    void print_state()
    {
        cout << "H(i->a)" << endl;
        for (var x = 1; x <= cnf->N; x++)
        {
            cout << x << ": ";
            for (auto p : H[x])
                cout << "(" << p.first << ": " << p.second << ") ";
            cout << endl;
        }
        cout << "U(a->i)" << endl;
        for (clause_id a = 0; a < cnf->M; a++)
        {
            cout << a << ": ";
            for (auto p : U[a])
                cout << "(" << p.first << ": " << p.second << ") ";
            cout << endl;
        }
        for (int x = 1; x <= cnf->N; x++)
        {
            auto p = extract_marginal(x);
            cout << x << ": " << p.first << "/" << p.second << endl;
        }
        cnf->print();
    }

    int Propagation(const int max_iter = 200)
    {
        vector<double> previous_signal(cnf->N + 1, 0.5);
        int i;
        double max_diff = 0.;
        for (i = 0; i < max_iter; i++)
        {
            // cout << "Iteration: " << i + 1 << endl;
            BP_round();
            // print_state();
            max_diff = 0;
            for (var x = 1; x <= cnf->N; ++x)
            {
                double new_signal = tanh(signal(x));
                max_diff = max(max_diff, abs(new_signal - previous_signal[x]));
                previous_signal[x] = new_signal;
            }
            // cout << max_diff << endl;
            if (max_diff <= 1e-5)
                break;
        }
        // cout << max_diff << " " << i << endl;
        return i;
    }

public:
    string name() override { return "Belief Propagation with UCP"; }
    bool solve(CNF *cnf) override
    {
        init(cnf);
        // cout << "initialized" << endl;
        StatsKeeper stats("BP iterations");
        int t = 0;
        for (int i = 0; i < cnf->N; i++)
        {
            // if (i % (cnf->N / 100) == (cnf->N / 100) - 1)
            //     cout << (i + 1) << "%" << endl;
            int id = get_unit_clause();
            var x = get_pure_literal();
            if (id != NOT_A_CLAUSE)
                satisfy(cnf->clauses[id][0]), t++;
            else if (x != NOT_A_VAR)
                satisfy(x), t++;
            else
            {
                int iter = Propagation();
                stats.add(iter);
                // cout << iter << endl;
                var x_opt = -1;
                double max_marginal = 0;
                for (var x = 1; x <= cnf->N; x++)
                    if (!cnf->is_erased(x))
                    {
                        pair<double, double> p = extract_marginal(x);
                        // cout << "Bvar: " << x << endl;
                        // cout << "prob0: " << p.second << endl;
                        // cout << "prob1: " << p.first << endl;
                        if (p.first > max_marginal)
                            x_opt = -x, max_marginal = p.first;
                        if (p.second > max_marginal)
                            x_opt = x, max_marginal = p.second;
                    }
                // cout << "Bvar: " << x_opt << endl;
                // cout << "prob: " << max_marginal << endl;
                //  cout << "satisfying " << x_opt << " with marginal " << max_marginal << endl;
                satisfy(x_opt);
            }
        }
        stats.print();
        // cout << "done forced steps" << t << endl;
        return cnf->is_satisfied();
    }
};