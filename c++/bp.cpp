#include <math.h>
#include "unit_clause.cpp"

class BeliefPropagation : public Unit_Clause
{
    vector<unordered_map<int, double>> U, H;

    CNF *cnf;

    void init(CNF *cnf)
    {
        this->cnf = cnf;
        H.resize(cnf->N + 1);
        U.resize(cnf->M);
        for (clause_id id = 0; id < cnf->M; id++)
            for (int x : cnf->clauses[id])
                U[id][x] = 0.;
        for (var x = 1; x <= cnf->N; x++)
            for (clause_id id : cnf->var_to_clauses[x])
                H[x][id] = 0.;
        Unit_Clause::init(cnf);
    }

    // O(r*k^2)
    void update_var(var i)
    {
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
                H[i][id] = positives - U[id][i];
            else
                H[i][id] = negatives - U[id][i];
        }
    }

    double f(const vector<double> &hs)
    {
        double result = 1.;
        for (double h : hs)
            result *= (1 - tanh(h)) / 2.;
        result = -log(1. - result);
    }

    // O(k ^ 2)
    void update_clause(clause_id id)
    {
        clause &C = cnf->clauses[id];
        for (int x : C)
        {
            vector<double> hs;
            for (int y : C)
                if (y != x)
                    hs.push_back(H[y][id]);
            U[id][x] = f(hs);
        }
    }

    pair<double, double> extract_marginal(var x)
    {
        double result = 0.;
        for (clause_id id : cnf->var_to_clauses[x])
        {
            if (contains(cnf->clauses[id], x))
                result += U[id][x];
            else
                result -= U[id][x];
        }
        double pos = (1. + tanh(result)) / 2.;
        double neg = (1. - tanh(result)) / 2.;
        return {neg, pos};
    }

    void BP_round()
    {
        vector<pair<bool, int>> ordering;
        for (var x = 1; x <= cnf->N; x++)
            ordering.push_back({0, x});
        for (clause_id id = 0; id < cnf->M; id++)
            ordering.push_back({1, id});
        random_shuffle(ordering.begin(), ordering.end());
        for (auto p : ordering)
            if (p.first)
                update_clause(p.second);
            else
                update_var(p.second);
    }

public:
    string name() override { return "Belief Propagation"; }
};