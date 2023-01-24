#pragma once
#include "bp.cpp"

class ExperimentalBP : public BeliefPropagation
{
public:
    string name() override { return "Experimental Belief Propagation"; }
    bool solve(CNF *cnf) override
    {
        init(cnf);
        // cout << "initialized" << endl;
        StatsKeeper stats("BP iterations");

        int rate = sqrt(cnf->N);
        int t = rate;
        priority_queue<pair<double, int>> marginals;
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
                if (t >= rate)
                {
                    // cout << i << endl;
                    t = 0;
                    // cout << i << " " << t << " " << rate << endl;
                    Propagation();
                    while (!marginals.empty())
                        marginals.pop();
                    for (var x = 1; x <= cnf->N; x++)
                        if (!cnf->is_erased(x))
                        {
                            auto p = extract_marginal(x);
                            marginals.push({p.first, -x});
                            marginals.push({p.second, x});
                        }
                }
                int x_opt = NOT_A_VAR;
                while (!marginals.empty() && cnf->is_erased(marginals.top().second))
                    marginals.pop();
                if (marginals.empty())
                    break;
                x_opt = marginals.top().second;
                marginals.pop();
                satisfy(x_opt);
            }
        }
        return cnf->is_satisfied();
    }
    // stats.print();
    // cout << "done forced steps" << t << endl;
};