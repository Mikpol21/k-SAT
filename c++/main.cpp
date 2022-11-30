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

int main(int argc, char *argv[])
{
    srand(time(NULL));
    int k = 3;
    int n = atoi(argv[1]);
    float r = atof(argv[2]);
    SAT_solver *solver = get_solver(argv[3]);
    int test_cases = 100;
    int satisfied = 0;

    for (int i = 0; i < test_cases; i++)
    {
        CNF *cnf = generate_CNF(n, r, k);
        if (solver->solve(cnf))
        {
            satisfied++;
            printf("case: %d satisfied\n", i + 1);
        }
        else
            printf("case: %d not satisfied\n", i + 1);
        delete cnf;
    }
    cout << float(satisfied) / float(test_cases) << endl;
    return 0;
}