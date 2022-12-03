#pragma once
#include "CNF.cpp"

vector<clause> generate_CNF(int n, float r, int k)
{
    int m = int(n * r);
    vector<clause> clauses;
    for (int i = 0; i < m; i++)
    {
        clause C;
        for (int j = 0; j < k; j++)
        {
            var variable = rand() % n + 1;
            while (contains(C, variable) || contains(C, -variable))
                variable = rand() % n + 1;
            C.push_back(rand() % 2 ? variable : -variable);
        }
        clauses.push_back(C);
    }
    return clauses;
}
