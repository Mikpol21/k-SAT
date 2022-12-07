#include "CNF.cpp"
#include "unit_clause.cpp"
#include "unit_clause_with_majority.cpp"
#include "johnson.cpp"
#include "generator.cpp"
#include "walksat.cpp"
#include "pure_literal.cpp"
#include "bp.cpp"
#include <chrono>
using namespace std;
using namespace std::chrono;
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
    if (name == "BP")
        return new BeliefPropagation();
    if (name == "PL")
        return new PureLiteral();
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

void print_stats(map<string, int> &satisfied,
                 map<string, int> &unsat_clauses,
                 map<string, time_t> &times,
                 vector<SAT_solver *> &solvers,
                 int test_cases)
{
    cout << "Prob of satisfying a formula" << endl;
    for (SAT_solver *solver : solvers)
    {
        float ratio = (float)satisfied[solver->name()] / (float)test_cases;
        cout << "  " << solver->name() << ": " << ratio << endl;
    }
    cout << "Average number of empty clauses" << endl;
    for (SAT_solver *solver : solvers)
    {
        float ratio = (float)unsat_clauses[solver->name()] / (float)(test_cases - satisfied[solver->name()]);
        cout << "  " << solver->name() << ": " << ratio << endl;
    }
    cout << "Average execution time" << endl;
    for (SAT_solver *solver : solvers)
    {
        float ratio = (float)times[solver->name()] / (float)test_cases;
        cout << "  " << solver->name() << ": " << ratio << "ms" << endl;
    }
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
    if (argc > 3)
        solvers.push_back(get_solver(argv[3]));
    else
    {
        solvers.push_back(get_solver("UC"));
        solvers.push_back(get_solver("UCM"));
        solvers.push_back(get_solver("Johnson"));
        solvers.push_back(get_solver("WalkSAT"));
        solvers.push_back(get_solver("BP"));
        solvers.push_back(get_solver("PL"));
    }
    map<string, int> satisfied;
    map<string, int> unsat_clauses;
    map<string, time_t> times;
    cout << "Solving " << k << "-SAT"
         << " on " << test_cases << " test cases using:" << endl;
    for (SAT_solver *solver : solvers)
        cout << "  - " << solver->name() << endl;
    for (int i = 0; i < test_cases; i++)
    {
        vector<clause> clauses = generate_CNF(n, r, k);
        // printf("test case %d\n", i);
        for (SAT_solver *solver : solvers)
        {
            cout << "\t";
            CNF *cnf = new CNF(n, m, k, clauses);
            auto t1 = high_resolution_clock::now();
            bool result = solver->solve(cnf);
            auto t2 = high_resolution_clock::now();
            if (result)
            {
                satisfied[solver->name()]++;
                assert(satisfies(clauses, solver->assignment));
            }
            else
                unsat_clauses[solver->name()] += cnf->not_satisfied;
            times[solver->name()] += duration_cast<milliseconds>(t2 - t1).count();
            delete cnf;
            print_progress((i + 1) / (float)test_cases);
        }
    }
    cout << endl;
    print_stats(satisfied, unsat_clauses, times, solvers, test_cases);
    return 0;
}