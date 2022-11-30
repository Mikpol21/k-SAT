#include "CNF.cpp"

class WalkSAT : public SAT_solver
{
public:
    string name() const { return "WalkSAT"; }
    bool solve(vector<clause> cnf)
    {
    }
};