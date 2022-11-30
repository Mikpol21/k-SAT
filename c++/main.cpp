#include "CNF.cpp"
#include "unit_clause.cpp"
#include "unit_clause_with_majority.cpp"
#include "johnson.cpp"
#include "generator.cpp"

SAT_solver *get_solver(string name)
{
    if (name == "unit_clause" || name == "UC")
        return new Unit_Clause();
    if (name == "UCM")
        return new Unit_Clause_With_Majority();
    if (name == "Johnson")
        return new Johnson_Heuristic();
    return nullptr;
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

int main(int argc, char *argv[])
{
    srand(time(NULL));
    int k = 3;
    int n = atoi(argv[1]);
    float r = atof(argv[2]);
    int m = r * n;
    vector<SAT_solver *> solvers;
    cout << argc << endl;
    if (argc > 3)
        solvers.push_back(get_solver(argv[3]));
    else
    {
        solvers.push_back(get_solver("UC"));
        solvers.push_back(get_solver("UCM"));
        solvers.push_back(get_solver("Johnson"));
    }
    map<string, int> satisfied;
    int test_cases = 100;
    for (int i = 0; i < test_cases; i++)
    {
        vector<clause> clauses = generate_CNF(n, r, k);
        printf("test case %d\n", i);
        for (SAT_solver *solver : solvers)
        {
            cout << "\t";
            CNF *cnf = new CNF(n, m, k, clauses);
            if (solver->solve(cnf))
            {
                satisfied[solver->name()]++;
                assert(satisfies(clauses, solver->assignment));
                cout << solver->name() << ": satisfied\n";
            }
            else
                cout << solver->name() << ": not satisfied\n";
            delete cnf;
        }
    }
    for (SAT_solver *solver : solvers)
    {
        float ratio = (float)satisfied[solver->name()] / (float)test_cases;
        cout << solver->name() << ": " << ratio << endl;
    }
    return 0;
}