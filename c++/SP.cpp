#pragma once
#include "pure_literal.cpp"
#include "CNF.cpp"

class SurveryPropagation : public PureLiteral
{
    /**
        PI[v][a] = PIu[v][a]/(PIu[v][a] + PIs[v][a] + PI0[v][a])
        eta[a][v] = 1 - eta[a][v]
    */
protected:
    vector<unordered_map<int, double>> PI, eta;

    // pairs (b, x) -> if b = 1 then x is a clause else x is a variable
    vector<pair<bool, int>> ordering;
    CNF *cnf;

    void restart_prop()
    {
        for (var v = 1; v <= cnf->N; v++)
            for (int a : cnf->var_to_clauses[v])
            {
                PI[v][a] = (float)rand() / RAND_MAX;
                eta[a][v] = (float)rand() / RAND_MAX;
            }
    }

    void init(CNF *cnf)
    {
        this->cnf = cnf;
        PI.resize(cnf->N + 1);
        eta.resize(cnf->M + 1);
        for (var v = 1; v <= cnf->N; v++)
            for (int a : cnf->var_to_clauses[v])
            {
                PI[v][a] = (float)rand() / RAND_MAX;
                eta[a][v] = (float)rand() / RAND_MAX;
            }
        for (var v = 1; v <= cnf->N; v++)
            ordering.push_back({0, v});
        for (clause_id a = 0; a < cnf->M; a++)
            ordering.push_back({1, a});
        PureLiteral::init(cnf);
    }

    // Updating PI
    void update_var(const var v)
    {
        assert(v > 0);
        double positives = 1., negatives = 1.;
        for (clause_id a : cnf->var_to_clauses[v])
        {
            if (contains(cnf->clauses[a], v))
                positives *= eta[a][v];
            else
                negatives *= eta[a][v];
        }
        for (clause_id a : cnf->var_to_clauses[v])
        {
            double u, s, o;
            if (contains(cnf->clauses[a], v))
            {

                u = (1 - negatives) * positives;
                s = (eta[a][v] - positives) * negatives;
                o = positives * negatives;
                PI[v][a] = u / (u + s + o);
            }
            else
            {
                u = (1 - positives) * negatives;
                s = (eta[a][v] - negatives) * positives;
                o = positives * negatives;
                PI[v][a] = u / (u + s + o);
            }
        }
    }

    void update_clause(const clause_id a)
    {
        for (var v : cnf->clauses[a])
        {
            double product = 1.;
            for (var v2 : cnf->clauses[a])
                if (v2 != v)
                    product *= PI[abs(v2)][a];
            eta[a][abs(v)] = 1. - product;
            /*if (isnan(eta[a][abs(v)]))
            {
                cout << "nan error" << endl;
                cout << v << ": " << PI[abs(v)][a] << " " << product << endl;
                for (var v : cnf->clauses[a])
                {
                    cout << v << " -> " << PI[abs(v)][a] << " ";
                }
                cout << endl;
                exit(0);
            }*/
        }
    }

    void update()
    {
        random_shuffle(ordering.begin(), ordering.end());
        for (auto p : ordering)
            if (p.first)
            {
                clause_id a = p.second;
                if (cnf->clauses[a].size() == 1)
                {
                    assert(cnf->clauses[a][0] == SATISFIED);
                    continue;
                }
                if (cnf->clauses[a].size() == 0)
                    continue;
                else
                    update_clause(p.second);
            }
            else if (!cnf->is_erased(p.second))
                update_var(p.second);
    }

    int Propagation(bool debug = false, const int max_iter = 1000)
    {
        const double accuracy = 1e-3;
        vector<unordered_map<int, double>> eta_old = eta;
        int iter;
        for (iter = 0; iter < max_iter; iter++)
        {
            update();
            if (debug)
            {
                print_state();
                int n;
                cin >> n;
            }
            double max_error = 0;
            for (clause_id a = 0; a < cnf->M; a++)
                for (var v : cnf->clauses[a])
                    max_error = max(max_error, abs(eta[a][abs(v)] - eta_old[a][abs(v)]));
            if (max_error < accuracy)
                break;
            eta_old = eta;
        }
        return iter;
    }

    void print_state()
    {
        for (clause_id a = 0; a < cnf->M; a++)
        {
            cout << "clause " << a << endl;
            cout << "etas : ";
            for (var v : cnf->clauses[a])
                cout << v << " -> " << eta[a][abs(v)] << " ";
            cout << endl;
            cout << "PIs : ";
            for (var v : cnf->clauses[a])
                cout << v << " -> " << PI[abs(v)][a] << " ";
            cout << endl;
        }
    }

    /*
        returns {W-(v), W+(v)}
    */
    pair<double, double> bias(const var v)
    {
        double positives = 1., negatives = 1.;
        for (clause_id a : cnf->var_to_clauses[v])
        {
            if (contains(cnf->clauses[a], v))
                positives *= eta[a][v];
            else
                negatives *= eta[a][v];
        }
        double p, n, o;

        // cout << "positives = " << positives << endl;
        // cout << "negatives = " << negatives << endl;
        n = (1 - negatives) * positives;
        p = (1 - positives) * negatives;
        o = positives * negatives;
        /*if (p + n < 1e-3)
        {
            cout << v << ": " << p << " " << n << " " << o << endl;
            for (auto a : cnf->var_to_clauses[v])
            {
                cout << a << ": ";
                for (auto v2 : cnf->clauses[a])
                    cout << v2 << " ";
                cout << endl;
            }
        }*/
        return {n / (n + p + o), p / (n + p + o)};
    }

public:
    string name()
    {
        return "SP";
    }

    bool solve(CNF *cnf)
    {
        init(cnf);
        int t = 0;
        float rate = 0.01;
        for (int i = 0; i < cnf->N; i++)
        {
            int id = get_unit_clause();
            var x = get_pure_literal();
            if (id != NOT_A_CLAUSE)
                satisfy(cnf->clauses[id][0]), t++;
            else if (x != NOT_A_VAR)
                satisfy(x), t++;
            else
            {
                Propagation();
                var v_opt = -1;
                double bias_opt = 0.;
                for (var v = 1; v <= cnf->N; v++)
                    if (!cnf->is_erased(v))
                    {
                        auto b = bias(v);
                        auto abs_d = abs(b.first - b.second);
                        if (bias_opt <= abs_d)
                            bias_opt = abs_d, v_opt = v;
                    }
                if (v_opt == -1)
                {
                    cout << "Taking free step " << i << endl;
                    v_opt = cnf->next_rand_var();
                    if (coin_flip())
                        satisfy(v_opt);
                    else
                        satisfy(-v_opt);
                    continue;
                }
                assert(v_opt != -1);
                // cout << "Satisfying " << v_opt << endl;
                auto b = bias(v_opt);
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
