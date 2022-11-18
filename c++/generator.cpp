#pragma once
#include "CNF.cpp"

CNF *generate_CNF2(int n, int m, int k)
{
    vector<clause> clauses;
    for (int i = 0; i < m; i++)
    {
        clause C;
        for (int j = 0; j < k; j++)
        {
            var variable = rand() % n + 1;
            while (contains(C, -variable) || contains(C, -variable))
                variable = rand() % n + 1;
            C.push_back(rand() % 2 ? variable : -variable);
        }
        clauses.push_back(C);
    }
    return new CNF(n, m, k, clauses);
}

CNF *generate_CNF(int n, float r, int k)
{
    return generate_CNF2(n, (int)(n * r), k);
}
