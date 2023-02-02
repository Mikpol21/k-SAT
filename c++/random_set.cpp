#pragma once
#include <bits/stdc++.h>
using namespace std;

class RandomSet
{
    int N = 1, n;
    vector<int> S;

public:
    RandomSet(int n)
    {
        while (N < n)
            N *= 2;
        this->n = n;
        assert(n <= N);
        S.resize(N * 2 + 1);
        for (int i = 0; i <= N * 2; i++)
            S[i] = 0;
    }

    void insert(int x)
    {
        if (S[x + N] == 1)
            return;
        int node = x + N;
        while (node > 0)
        {
            // cout << node << endl;
            S[node]++;
            node /= 2;
        }
    }

    void remove(int x)
    {
        assert(x + N <= 2 * N);
        if (S[x + N] == 0)
            return;
        int node = x + N;
        while (node > 0)
        {
            S[node]--;
            node /= 2;
        }
    }
    bool contains(int x)
    {
        return S[x + N] == 1;
    }
    int size()
    {
        return S[1];
    }
    int get_random()
    {
        int r = (rand() % S[1]) + 1;
        int node = 1;
        while (node < N)
        {
            node *= 2;
            if (S[node] < r)
                r -= S[node], ++node;
        }
        return node - N;
    }
    void print()
    {
        for (int i = 0; i < n; i++)
            if (S[N + i])
                cout << i << " ";
        cout << endl;
    }
};