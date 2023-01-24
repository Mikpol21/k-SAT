#pragma once
#include <bits/stdc++.h>
using namespace std;

class StatsKeeper
{
    string name = "";
    int _max = 0;
    int _min = 1000000000;
    int sum = 0;
    int size = 0;

public:
    StatsKeeper(string name)
    {
        this->name = name;
    }

    void add(int entry)
    {
        _max = max(_max, entry);
        _min = min(_min, entry);
        sum += entry;
        size++;
    }

    void print()
    {
        if (name != "")
            cout << name << ": \n";
        cout << "Max: " << _max << endl;
        cout << "Min: " << _min << endl;
        cout << "Average: " << float(sum) / float(size) << endl;
    }
};