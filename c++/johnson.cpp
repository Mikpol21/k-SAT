#include "CNF.cpp"
#include "unit_clause.cpp"

class Johnson_Heuristic : public Unit_Clause
{
    vector<int> weights;
    int B = 0;
    int cnt = 0;
    set<pair<int, var>> ordering;

protected:
    void init(CNF *cnf)
    {
        Unit_Clause::init(cnf);
        B = 1 << (cnf->K);
        assignment.clear(), assignment.resize(cnf->N + 1);
        weights.resize(2 * cnf->N + 1);
        for (int i = 0; i <= 2 * cnf->N; i++)
            weights[i] = 0;
        for (int i = 0; i < cnf->M; i++)
        {
            clause &C = cnf->clauses[i];
            for (int x : C)
                weights[cnf->N + x] += B >> C.size();
        }
        for (int i = 1; i <= cnf->N; i++)
        {
            ordering.insert({weights[cnf->N + i], i});
            ordering.insert({weights[cnf->N - i], -i});
        }
    }

    inline void satisfy(int x)
    {
        cnt++;
        if (cnt % (cnf->N / 10) == 0)
            cout << "\r\t" << float(cnt) / cnf->N * 100 << "%" << flush;
        assignment[abs(x)] = x > 0 ? true : false;
        for (clause_id id : cnf->var_to_clauses[abs(x)])
        {
            clause &C = cnf->clauses[id];
            if (contains(C, x))
                for (int y : C)
                {
                    ordering.erase({weights[cnf->N + y], y});
                    weights[cnf->N + y] -= B >> C.size();
                    ordering.insert({weights[cnf->N + y], y});
                }
            else
                for (int y : C)
                {
                    ordering.erase({weights[cnf->N + y], y});
                    weights[cnf->N + y] += B >> (C.size());
                    ordering.insert({weights[cnf->N + y], y});
                }
        }
        ordering.erase({weights[cnf->N + x], x});
        ordering.erase({weights[cnf->N - x], -x});
        Unit_Clause::satisfy(x);
    }

public:
    string name() override { return "Johnson Heuristic"; }
    bool solve(CNF *cnf) override
    {
        init(cnf);
        cout << "0%" << flush;
        cnt = 0;
        while (!ordering.empty())
        {
            int x = ordering.rbegin()->second;
            satisfy(x);
            while (size_to_clauses[1].size() > 0)
            {
                clause_id id = size_to_clauses[1][0];
                x = cnf->clauses[id][0];
                satisfy(x);
            }
        }
        return cnf->is_satisfied();
    }
};