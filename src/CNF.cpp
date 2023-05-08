#pragma once
#include <bits/stdc++.h>
#include <iostream>
using namespace std;

#define lit(x) abs(x)
#define NOT_A_CLAUSE -1
#define NOT_A_VAR 0
#define SATISFIED 0
typedef vector<int> clause;
typedef int clause_id;
typedef int var;

bool coin_flip()
{
    return rand() % 2;
}

void fast_erase(clause &C, int x)
{

    for (int i = 0; i < C.size(); i++)
        if (C[i] == x)
        {
            int last = C.size() - 1;
            int tmp = C[last];
            C[last] = C[i];
            C[i] = tmp;
            C.pop_back();
        }
}

void print_vec(vector<vector<int>> &V)
{
    for (int i = 0; i < V.size(); i++)
    {
        cout << i << ": ";
        for (int x : V[i])
            cout << x << " ";
        cout << endl;
    }
}

bool contains(clause &C, int x)
{
    for (int i = 0; i < C.size(); i++)
        if (C[i] == x)
            return true;
    return false;
}

bool satisfies(vector<clause> const clauses, vector<bool> const assignment)
{
    for (clause C : clauses)
    {
        bool satisfied = false;
        for (int x : C)
        {
            if (x > 0 && assignment[x])
                satisfied = true;
            if (x < 0 && !assignment[-x])
                satisfied = true;
        }
        if (!satisfied)
            return false;
    }
    return true;
}

class CNF
{
    vector<bool> vars_seen;
    vector<int> vars_permutation;

public:
    int current_permutation_index = 0;
    int N, M, K; // N - vars, M - clauses
    vector<clause> clauses;
    vector<vector<clause_id>> var_to_clauses;
    CNF(int N, int M, int K, vector<clause> clauses)
    {
        this->N = N;
        this->M = M;
        this->K = K;
        this->clauses = clauses;
        for (int i = 0; i <= N; i++)
            vars_seen.push_back(0);
        for (int i = 1; i <= N; i++)
            vars_permutation.push_back(i);

        random_shuffle(vars_permutation.begin(), vars_permutation.end());
        var_to_clauses.resize(N + 1);
        for (int i = 1; i <= N; i++)
            var_to_clauses[i].clear();
        for (int i = 0; i < M; i++)
        {
            clause &C = clauses[i];
            for (var x : C)
                var_to_clauses[abs(x)].push_back(i);
        }
        /*int max_clauses = 0, min_clauses = 100000000, average_clauses = 0;
        for (int i = 1; i <= N; i++)
        {
            max_clauses = max(max_clauses, (int)var_to_clauses[i].size());
            min_clauses = min(min_clauses, (int)var_to_clauses[i].size());
            average_clauses += var_to_clauses[i].size();
        }
        average_clauses /= N;
        cout << "Max clauses: " << max_clauses << endl;
        cout << "Min clauses: " << min_clauses << endl;
        cout << "Average clauses: " << average_clauses << endl;
        */
    }
    void erase_var(int x)
    {
        x = abs(x);
        vars_seen[x] = 1;
    }
    int next_rand_var()
    {
        while (current_permutation_index < N && vars_seen[vars_permutation[current_permutation_index]])
            current_permutation_index++;
        // cout << "OHOHOHOHOHOHOH " << current_permutation_index << endl;
        if (current_permutation_index == N)
            return NOT_A_VAR;
        return vars_permutation[current_permutation_index];
    }
    bool is_erased(var x)
    {
        return vars_seen[abs(x)];
    }
    void satisfy(int x)
    {
        if (is_erased(x))
            return;
        erase_var(x);
        for (clause_id id : var_to_clauses[lit(x)])
        {
            // assert(contains(clauses[id], x) || contains(clauses[id], -x));
            clause &C = clauses[id];
            if (contains(C, x))
            {
                fast_erase(C, x);
                for (var y : C)
                    fast_erase(var_to_clauses[abs(y)], id);
                C = vector<int>(1, SATISFIED);
            }
            else
            {
                fast_erase(C, -x);
                // if (C.size() == 0)
                // cout << "Error while satisfying " << x << endl;
            }
        }
        var_to_clauses[lit(x)].clear();
    }
    int not_satisfied = 0;
    bool is_satisfied()
    {
        not_satisfied = 0;
        for (clause &C : clauses)
        {
            // assert(C.size() <= 1);
            // assert(C.size() == 0 || C[0] == SATISFIED);
            if (C.size() == 0 || (C[0] != SATISFIED))
                not_satisfied++;
        }
        // cout << "Found " << not_satisfied << " not satisfied clauses" << endl;
        return not_satisfied == 0;
    }
    void print()
    {
        cout << "N = " << N << ", M = " << M << endl;
        for (int i = 0; i < M; i++)
        {
            clause &C = clauses[i];
            cout << "(";
            for (int j = 0; j < C.size(); j++)
            {
                cout << C[j];
                if (j != C.size() - 1)
                    cout << " and ";
            }
            cout << ")";
            if (i != M - 1)
                cout << " or ";
        }
        cout << endl;
    }
};

class SAT_solver
{
public:
    vector<bool> assignment;
    virtual string name() { return "None"; }
    virtual bool solve(CNF *cnf)
    {
        return false;
    }
};