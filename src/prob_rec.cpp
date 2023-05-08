#pragma once
#include "pure_literal.cpp"
#include "CNF.cpp"
#include "stats_keeper.cpp"
class TreeSAT : public PureLiteral
{
    int N, M;
    vector<bool> vis_var, vis_clause;
    vector<double> T_bar, T;
    vector<double> var_1, var_0;
    vector<vector<int>> var_to_clauses;
    vector<bool> is_leaf;

public:
    string name() override { return "Tree Prop"; }
    void toggle_on()
    {
        for (int i = 0; i <= N; i++)
            vis_var[i] = true;
        for (int i = 0; i < M; i++)
            vis_clause[i] = true;
    }

    void toggle_off()
    {
        for (int i = 0; i <= N; i++)
            vis_var[i] = false;
        for (int i = 0; i < M; i++)
            vis_clause[i] = false, is_leaf[i] = false;
    }
    StatsKeeper vars_visited = StatsKeeper("vars visited");
    StatsKeeper clauses_visited = StatsKeeper("clauses visited");
    vector<int> visited_clauses, visited_vars;
    void smart_toggle_off()
    {
        // cout << "visited clauses " << visited_clauses.size() << endl;
        // cout << "visited variables " << visited_vars.size() << endl;
        vars_visited.add(visited_vars.size());
        clauses_visited.add(visited_clauses.size());
        for (clause_id id : visited_clauses)
            vis_clause[id] = false, is_leaf[id] = false;
        for (var v : visited_vars)
            vis_var[v] = false;
        visited_clauses.clear();
        visited_vars.clear();
    }

    void init(CNF *cnf)
    {
        N = cnf->N, M = cnf->M;
        vis_var.resize(N + 1);
        vis_clause.resize(M + 1);
        is_leaf.resize(M + 1);
        T_bar.resize(M + 1), T.resize(M + 1);
        var_1.resize(N + 1), var_0.resize(N + 1);
        this->cnf = cnf;
        for (var v = 1; v <= N; v++)
            var_to_clauses.clear();
        PureLiteral::init(cnf);
    }

    double log_sum(double x, double y)
    {
        // return log(exp(x) + exp(y));
        if (x > y)
            return x + log(1 + exp(y - x));
        else
            return y + log(1 + exp(x - y));
    }

    double log_diff(double x, double y)
    {
        // return log(exp(x) - exp(y));
        return x + log(1 - exp(y - x));
    }

    void clause_DFS(int id, int depth = 1000000)
    {
        vis_clause[id] = true;
        visited_clauses.push_back(id);
        // double prob = 1.0, without_c = 1.0;
        double prob = 0., without_c = 0.;
        random_shuffle(cnf->clauses[id].begin(), cnf->clauses[id].end());
        bool isleaf = true;
        double log_half = log(0.5);
        for (var v : cnf->clauses[id])
        {
            if (!vis_var[abs(v)])
            {
                isleaf = false;
                variable_DFS(abs(v), depth - 1);
                // prob *= 0.5 * (var_1[abs(v)] + var_0[abs(v)]);
                prob += log_half + log_sum(var_1[abs(v)], var_0[abs(v)]);
                if (v > 0)
                    // without_c *= 0.5 * var_0[abs(v)];
                    without_c += log_half + var_0[abs(v)];
                else // without_c *= 0.5 * var_1[abs(v)];
                    without_c += log_half + var_1[abs(v)];
            }
        }
        is_leaf[id] = isleaf;
        T_bar[id] = prob;
        // T[id] = prob - without_c
        T[id] = log_diff(prob, without_c);
        // cout << "clause " << id << ": with clause " << T[id] << ", without " << T_bar[id] << endl;
    }

    void variable_DFS(int v, int depth = 1000000)
    {
        if (depth == 0)
        {
            // var_1[v] = var_0[v] = 1.0;
            var_1[v] = var_0[v] = 0.0;
            return;
        }
        vis_var[v] = true;
        visited_vars.push_back(v);
        // double prob_0 = 1.0, prob_1 = 1.0;
        double prob_0 = 0.0, prob_1 = 0.0;
        random_shuffle(cnf->var_to_clauses[v].begin(), cnf->var_to_clauses[v].end());
        for (clause_id id : cnf->var_to_clauses[v])
        {
            if (!vis_clause[id])
            {
                clause_DFS(id, depth);
                if (is_leaf[id])
                    continue;
                if (contains(cnf->clauses[id], v))
                {
                    // prob_1 *= T_bar[id];
                    prob_1 += T_bar[id];
                    // prob_0 *= T[id];
                    prob_0 += T[id];
                }
                else
                {
                    // prob_1 *= T[id];
                    prob_1 += T[id];
                    // prob_0 *= T_bar[id];
                    prob_0 += T_bar[id];
                }
            }
        }
        var_1[v] = prob_1;
        var_0[v] = prob_0;
        /// cout << "var " << v << ": 0 -> " << var_0[v] << " | 1 -> " << var_1[v] << endl;
        // for (clause_id id : cnf->var_to_clauses[v])
        //     cout << id << " ";
        // cout << endl;
    }

    double compute_naively(int fixed, bool value)
    {
        int assignments = 0;
        vector<bool> assignment;
        assignment.resize(N + 1);
        for (int mask = 0; mask < (1 << N); mask++)
        {
            for (int i = 0; i < N; i++)
                assignment[i + 1] = mask & (1 << i);
            if (assignment[fixed] == value && satisfies(cnf->clauses, assignment))
                assignments++;
        }
        return double(assignments) / double(1 << (N - 1));
    }

    bool solve(CNF *cnf) override
    {
        init(cnf);
        for (int i = 0; i < cnf->N; i++)
        {
            int id = get_unit_clause();
            var x = get_pure_literal();
            if (id != NOT_A_CLAUSE)
                satisfy(cnf->clauses[id][0]);
            else if (x != NOT_A_VAR)
                satisfy(x);
            else
            {
                int samples = 10;
                int v = cnf->next_rand_var();
                double prob0 = 0.0, prob1 = 0.0;
                for (int i = 0; i < samples; i++)
                {
                    smart_toggle_off();
                    variable_DFS(v, 3);
                    prob0 += var_0[v];
                    prob1 += var_1[v];
                }
                // cout << "Tvar: " << v << endl;
                // cout << "prob0: " << prob0 << " " << exp(prob0) << endl;
                //  cout << "prob1: " << prob1 << " " << exp(prob1) << endl;
                //   cnf->print();
                if (prob0 > prob1)
                    satisfy(-v);
                else
                    satisfy(v);
            }
        }
        // vars_visited.print();
        // clauses_visited.print();
        return cnf->is_satisfied();
    }
};
/*
int main()
{
    vector<clause> instance = {
        {1, 2, 3},
        {-1, 4, -5},
        {-1, 6},
        {-2, 7, -8},
        {3, 9}};
    CNF cnf = CNF(9, 5, 3, instance);
    TreeSAT solver = TreeSAT();
    solver.init(&cnf);
    int v = 2;
    solver.toggle_off();
    solver.variable_DFS(v);
    solver.toggle_off();
    solver.variable_DFS(v);
    cout << solver.compute_naively(v, 0);
    cout << solver.compute_naively(v, 1);
    cout << solver.solve(&cnf) << endl;
}
*/