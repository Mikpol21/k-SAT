#pragma once
#include "CNF.cpp"
#include "random_set.cpp"

class WalkSAT
{
    bool satisfied(clause const &C)
    {
        for (int x : C)
            if (assignment[abs(x)] == (x > 0))
                return true;
        return false;
    }

    double uniform()
    {
        return (double)rand() / INT_MAX;
    }

    int maxsteps, restarts;
    float p_random_flip;
    bool debug = false;

    void flip(int v)
    {
        assignment[v] = !assignment[v];
    }

    int run_walksat(vector<clause> const clauses, int M, int N, const int max_iter = 20)
    {
        RandomSet unsat_clauses = RandomSet(M);
        for (clause_id a = 0; a < M; a++)
            if (clauses[a].size() > 0 && clauses[a][0] != 0 && !satisfied(clauses[a]))
                unsat_clauses.insert(a);
        vector<vector<int>> var_to_clauses;
        var_to_clauses.resize(N + 1);
        for (clause_id a = 0; a < M; a++)
            for (int x : clauses[a])
                var_to_clauses[abs(x)].push_back(a);

        int min_clauses = unsat_clauses.size();
        if (debug)
            cout << "Running WalkSAT with " << unsat_clauses.size() << " unsat clauses" << endl;
        int iter;
        for (iter = 0; iter < maxsteps && unsat_clauses.size() > 0; iter++)
        {
            clause_id a = unsat_clauses.get_random();
            const clause C = clauses[a];

            // computing break
            int break_value = M;
            int best_x = 0;
            for (int v : C)
            {
                flip(abs(v));
                int unsat = 0;
                for (clause_id jd : var_to_clauses[abs(v)])
                    if (!satisfied(clauses[jd]) && !unsat_clauses.contains(jd))
                        unsat++;
                if (unsat < break_value)
                {
                    break_value = unsat;
                    best_x = v;
                }
                flip(abs(v));
            }
            int flipped_var = 0;
            if (break_value == 0)
                flipped_var = best_x;
            else if (uniform() <= p_random_flip)
                flipped_var = C[rand() % C.size()];
            else
                flipped_var = best_x;

            flipped_var = abs(flipped_var);
            flip(flipped_var);

            for (clause_id jd : var_to_clauses[flipped_var])
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
            min_clauses = min(min_clauses, unsat_clauses.size());
        }
        if (debug)
            cout << "Done after " << iter << " iterations" << endl;
        return min_clauses;
    }

public:
    vector<bool> assignment;
    WalkSAT(int maxsteps, int restarts, bool debug = false, float p_random_flip = 0.567)
    {
        this->maxsteps = maxsteps;
        this->restarts = restarts;
        this->p_random_flip = p_random_flip;
        this->debug = debug;
    }
    string name() { return "WalkSAT"; }
    int solve(vector<clause> const clauses, int M, int N, vector<bool> seed = vector<bool>())
    {
        for (clause c : clauses)
            if (c.size() == 0)
                return M;
        assignment.resize(N + 1);
        int min_unsat_clauses = M;
        vector<bool> best_assignment;
        for (int attempt = 0; attempt < restarts; attempt++)
        {
            if (seed.size() != N + 1)
                for (int i = 1; i <= N; i++)
                    assignment[i] = rand() % 2;
            else
                assignment = seed;
            int res = run_walksat(clauses, M, N);
            if (debug)
                cout << "Attempt " << attempt << " finished with " << res << " unsat clauses" << endl;
            if (res < min_unsat_clauses)
            {
                min_unsat_clauses = res;
                best_assignment = assignment;
            }
            if (min_unsat_clauses == 0)
            {
                for (clause_id a = 0; a < M; a++)
                    assert((clauses[a].size() == 1 && clauses[a][0] == 0) || satisfied(clauses[a]));
                return 0;
            }
        }
        assignment = best_assignment;
        return min_unsat_clauses;
    }
};