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
        cnt = 0;
        for (int i = 0; i < cnf->N; i++)
        {
            clause_id id = id = get_unit_clause();
            if (id != NOT_A_CLAUSE)
                satisfy(cnf->clauses[id][0]);
            else
            {
                if (ordering.empty())
                    break;
                int x = ordering.rbegin()->second;
                satisfy(x);
            }
        }
        return cnf->is_satisfied();
    }
};