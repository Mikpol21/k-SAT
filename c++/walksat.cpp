#include "CNF.cpp"
#include "random_set.cpp"

class WalkSAT : public SAT_solver
{
    bool satisfied(clause const &C)
    {
        for (int x : C)
            if (assignment[abs(x)] == (x > 0))
                return true;
        return false;
    }

    const int C = 20;
    void init(CNF *cnf, RandomSet *unsat)
    {
        assignment.clear();
        assignment.resize(cnf->N + 1);
        for (int i = 1; i <= cnf->N; i++)
            assignment[i] = coin_flip();
        for (int id = 0; id < cnf->M; id++)
            if (!satisfied(cnf->clauses[id]))
                unsat->insert(id);
    }

public:
    string name() override { return "WalkSAT"; }
    bool solve(CNF *cnf) override
    {
        RandomSet *unsat = new RandomSet(cnf->M);
        init(cnf, unsat);
        // cout << unsat->size() << endl;
        for (int iter = 0; iter < cnf->N * C && unsat->size() > 0; iter++)
        {
            /*cout << "iteration: " << iter << endl;
            for (int i = 1; i <= cnf->N; i++)
                cout << i << " -> " << assignment[i] << endl;
            */
            int id = unsat->get_random();
            clause &C = cnf->clauses[id];
            int x = C[rand() % C.size()];
            // cout << "pivoting " << x << endl;
            assignment[abs(x)] = !assignment[abs(x)];
            unsat->remove(id);
            for (int jd : cnf->var_to_clauses[abs(x)])
            {
                if (!satisfied(cnf->clauses[jd]))
                    unsat->insert(jd);
            }
            /*for (int id = 0; id < cnf->M; id++)
            {
                cout << id << ": " << satisfied(cnf->clauses[id]) << " -> ";
                for (int x : cnf->clauses[id])
                    cout << x << " ";
                cout << endl;
            }*/
        }

        // cout << "done" << endl;
        bool result = unsat->size() == 0;
        delete unsat;
        return result;
    }
};