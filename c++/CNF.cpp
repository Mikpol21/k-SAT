#pragma once
#include <bits/stdc++.h>
using namespace std;

#define lit(x) abs(x)
#define NOT_A_CLAUSE -1
#define NOT_A_VAR 0
typedef vector<int> clause;
typedef int clause_id;
typedef int var;

bool coin_flip()
{
    return rand() % 2;
}

void fast_erase(clause &C, int x)
{
    int last = C.size() - 1;
    for (int i = 0; i < C.size(); i++)
        if (C[i] == x)
        {
            int tmp = C[last];
            C[last] = C[i];
            C[i] = tmp;
            C.pop_back();
        }
}

bool contains(clause &C, int x)
{
    for (int i = 0; i < C.size(); i++)
        if (C[i] == x)
            return true;
    return false;
}

class CNF
{
public:
    int N, M, K; // N - vars, M - clauses
    vector<clause> clauses;
    set<var> variables;
    vector<set<clause_id>> size_to_clauses;
    vector<vector<clause_id>> var_to_clauses;
    CNF(int n, int m, int k, vector<clause> clauses)
    {
        N = n, M = m, K = k;
        for (int i = 1; i <= n; i++)
            variables.insert(i);

        size_to_clauses.resize(K + 1);
        var_to_clauses.resize(N + 1);
        this->clauses = clauses;
        for (int i = 0; i < M; i++)
        {
            clause &C = clauses[i];
            size_to_clauses[C.size()].insert(i);
            for (int j = 0; j < C.size(); j++)
                var_to_clauses[lit(C[j])].push_back(i);
        }
    }

    void satisfy(var x)
    {
        if (x == 0)
            return;
        variables.erase(abs(x));
        for (clause_id id : var_to_clauses[abs(x)])
        {
            clause &C = clauses[id];
            if (contains(C, x))
            {
                on_clause_erase(id);
                for (int j = 0; j < C.size(); j++)
                    if (C[j] != x)
                    {
                        fast_erase(var_to_clauses[abs(C[j])], id);
                        on_clause_erase(id, C[j]);
                    }
                size_to_clauses[C.size()].erase(id);
                C.clear();
                C.push_back(0);
            }
            else
            {
                on_clause_modified(id);
                size_to_clauses[C.size()].erase(id);
                fast_erase(C, -x);
                for (int j = 0; j < C.size(); j++)
                    on_clause_modified(id, C[j]);
                size_to_clauses[C.size()].insert(id);
            }
        }
    }

    clause_id get_unit_clause()
    {
        return size_to_clauses[1].size() ? *size_to_clauses[1].begin() : NOT_A_CLAUSE;
    }

    var get_any_var()
    {
        return variables.size() ? *variables.begin() : NOT_A_VAR;
    }

    bool is_satisfied()
    {
        return size_to_clauses[0].empty();
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

protected:
    inline void on_clause_erase(clause_id id) {}
    inline void on_clause_erase(clause_id id, var x) {}
    inline void on_clause_modified(clause_id id) {}
    inline void on_clause_modified(clause_id id, var x) {}
};

class SAT_solver
{
public:
    virtual string name() { return "None"; }
    virtual bool solve(CNF *cnf)
    {
        return false;
    }
};