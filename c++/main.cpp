#include "CNF.cpp"
#include "unit_clause.cpp"
#include "unit_clause_with_majority.cpp"
#include "johnson.cpp"
#include "generator.cpp"
#include "walksat.cpp"
using namespace std;
#define ESC “x1b”
#define CSI “x1b[”
using namespace std::chrono_literals;
#define PBSTR "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"
#define PBWIDTH 60

SAT_solver *get_solver(string name)
{
    if (name == "unit_clause" || name == "UC")
        return new Unit_Clause();
    if (name == "UCM")
        return new Unit_Clause_With_Majority();
    if (name == "Johnson")
        return new Johnson_Heuristic();
    if (name == "WalkSAT")
        return new WalkSAT();
    return nullptr;
}

void parse_config(int &k, int &test_cases)
{
    ifstream file;
    try
    {
        file.open("params.config");
        string line;
        getline(file, line);
        k = stoi(line);
        getline(file, line);
        test_cases = stoi(line);
        file.close();
    }
    catch (exception e)
    {
        k = 3, test_cases = 100;
        cout << "Error: " << e.what() << endl;
    }
}

void print_progress(double percentage)
{
    int val = (int)(percentage * 100);
    int lpad = (int)(percentage * PBWIDTH);
    int rpad = PBWIDTH - lpad;
    printf("\r%3d%% [%.*s%*s]", val, lpad, PBSTR, rpad, "");
    fflush(stdout);
}

int main(int argc, char *argv[])
{
    srand(time(NULL));
    int k, test_cases;
    parse_config(k, test_cases);
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
        solvers.push_back(get_solver("WalkSAT"));
    }
    map<string, int> satisfied;
    cout << "Solving " << k << "-SAT"
         << " on " << test_cases << " test cases using:" << endl;
    for (SAT_solver *solver : solvers)
        cout << "  - " << solver->name() << endl;
    cout << endl;
    for (int i = 0; i < test_cases; i++)
    {
        vector<clause> clauses = generate_CNF(n, r, k);
        // printf("test case %d\n", i);
        for (SAT_solver *solver : solvers)
        {
            cout << "\t";
            CNF *cnf = new CNF(n, m, k, clauses);
            if (solver->solve(cnf))
            {
                satisfied[solver->name()]++;
                assert(satisfies(clauses, solver->assignment));
                // cout << solver->name() << ": satisfied\n";
            }
            else
                // cout << solver->name() << ": not satisfied\n";
                delete cnf;
            print_progress((i + 1) / (float)test_cases);
        }
    }
    cout << endl;
    for (SAT_solver *solver : solvers)
    {
        float ratio = (float)satisfied[solver->name()] / (float)test_cases;
        cout << solver->name() << ": " << ratio << endl;
    }
    return 0;
}