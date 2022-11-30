#include "CNF.cpp"

class Johnson_CNF : public CNF
{
    int B;

public:
    vector<int> weight;
    priority_queue<pair<int, var>> Q;
    Johnson_CNF(CNF *cnf) : CNF(cnf->N, cnf->M, cnf->K, cnf->clauses)
    {
        B = 1 << K;
        weight.resize(N * 2 + 2);
        for (int i = 1; i <= N; i++)
        {
            weight[N + i] = 0;
            weight[N - i] = 0;
        }

        for (int i = 0; i < M; i++)
        {
            clause &C = clauses[i];
            for (int j = 0; j < C.size(); j++)
                weight[N + C[j]] += B >> C.size();
        }
        for (int i = 1; i <= N; i++)
            Q.push({weight[N + i], i}), Q.push({weight[N - i], -i});
    }

    var get_next_var()
    {
        if (Q.empty())
            return NOT_A_VAR;
        var x = Q.top().second;
        Q.pop();
        while (!Q.empty() && variables.find(lit(x)) != variables.end())
        {
            x = Q.top().second;
            Q.pop();
        }
        if (variables.find(lit(x)) != variables.end())
            return NOT_A_VAR;
        return x;
    }

protected:
    void on_clause_erase(clause_id id, var x)
    {
        clause &C = clauses[id];
        weight[N + x] -= B >> C.size();
    }
    void on_clause_modified(clause_id id, var x)
    {
        clause &C = clauses[id];
        // - 2^(B - (k - 1)) + 2^(B - k) = 2^(B-k-1)
        weight[N + x] += B >> (C.size() + 1);
        Q.push({weight[N + x], x});
    }
};

class Johnson_Heuristic : public SAT_solver
{
public:
    string name() override { return "Johnson Heuristic"; }
    bool solve(CNF *cnf) override
    {
        Johnson_CNF *cnf_2 = new Johnson_CNF(cnf);
        for (int t = 0; t < cnf->N; t++)
        {
            var x = cnf_2->get_next_var();
            if (x == NOT_A_VAR)
                break;
            cnf_2->satisfy(x);
        }
        return cnf_2->is_satisfied();
    }
};