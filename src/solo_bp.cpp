#include "CNF.cpp"
#include "generator.cpp"
#include <chrono>
using namespace std;
using namespace std::chrono;

class BP
{
    int N, M;
    vector<clause> clauses;
    vector<vector<clause_id>> clauses_from;
    vector<int> positives, negatives;
    vector<unordered_map<int, double>> U, H;
    vector<pair<bool, int>> ordering;
    queue<int> pure_literals;
    vector<unordered_set<int>> per_size;
    unordered_set<int> erased;
    vector<bool> assignment;

public:
    BP(vector<clause> clauses, int N, int M, int k)
    {
        this->N = N;
        this->M = M;
        this->clauses = clauses;
        this->clauses.insert(this->clauses.begin(), vector<int>());
        this->positives.resize(N + 1);
        this->negatives.resize(N + 1);
        this->clauses_from.resize(1);
        this->U.resize(M + 1);
        this->H.resize(N + 1);
        this->assignment.resize(N + 1);
        for (int i = 0; i <= k; i++)
            per_size.push_back(unordered_set<int>());
        for (int i = 1; i <= N; i++)
        {
            clauses_from.push_back(vector<clause_id>());
            ordering.push_back({0, i});
        }
        for (int i = 1; i <= M; i++)
        {
            ordering.push_back({1, i});
            for (int x : this->clauses[i])
            {
                U[i][abs(x)] = 0.;
                H[abs(x)][i] = 0.5;
                if (x > 0)
                {
                    positives[x]++;
                    clauses_from[abs(x)].push_back(i);
                }
                else
                {
                    negatives[abs(x)]++;
                    clauses_from[abs(x)].push_back(-i);
                }
            }
            per_size[k].insert(i);
        }
        for (int i = 1; i <= N; i++)
        {
            if (positives[i] == 0)
                pure_literals.push(-i);
            else if (negatives[i] == 0)
                pure_literals.push(i);
        }
    }

    void satisfy(int x)
    {
        erased.insert(abs(x));
        if (x > 0)
            assignment[x] = true;
        else
            assignment[-x] = false;
        positives[abs(x)] = 0;
        negatives[abs(x)] = 0;

        for (clause_id id : clauses_from[abs(x)])
        {
            per_size[clauses[abs(id)].size()].erase(abs(id));
            if (id * x > 0)
            {
                for (int y : clauses[abs(id)])
                    if (y != x)
                    {
                        if (y > 0)
                        {
                            positives[y] -= 1;
                            if (positives[y] == 0)
                                pure_literals.push(-y);
                            fast_erase(clauses_from[abs(y)], abs(id));
                        }
                        else
                        {
                            negatives[-y] -= 1;
                            if (positives[-y] == 0)
                                pure_literals.push(y);
                            fast_erase(clauses_from[abs(y)], -abs(id));
                        }
                    }
                clauses[abs(id)] = vector<int>(1, SATISFIED);
            }
            else
            {
                /*cout << x << " shrinks clause " << id << " to ";
                for (int y : clauses[abs(id)])
                {
                    cout << y << " ";
                }*/
                // cout << endl;
                fast_erase(clauses[abs(id)], -x);
                per_size[clauses[abs(id)].size()].insert(abs(id));
            }
        }
        clauses_from[abs(x)].clear();
    }

    int get_pure_literal()
    {
        int x = NOT_A_VAR;
        while (!pure_literals.empty() && erased.count(x))
            pure_literals.pop();
        if (!pure_literals.empty())
        {
            x = pure_literals.front();
            pure_literals.pop();
        }
        return x;
    }

    int get_unit_clause()
    {
        if (per_size[1].empty())
            return NOT_A_VAR;
        int id = *per_size[1].begin();
        return id;
    }

    void update_var(var i)
    {
        assert(i > 0);
        double positives = 0., negatives = 0.;
        for (clause_id id : clauses_from[i])
        {
            if (id > 0)
                positives += U[id][i];
            else
                negatives += U[-id][i];
        }
        for (clause_id id : clauses_from[i])
        {
            if (id > 0)
                H[i][id] = tanh(positives - U[id][i] - negatives);
            else
                H[i][-id] = tanh(negatives - U[-id][i] - positives);
        }
    }

    double f(const vector<double> &hs)
    {
        const double epsilon = 0.0001;
        double result = 1.;
        for (double h : hs)
            result *= (1 - h);
        result /= pow(2., hs.size());
        if (result == 1.)
            result -= epsilon;
        result = -0.5 * log(1. - result);
        return result;
    }

    void update_clause(clause_id id)
    {
        clause &C = clauses[id];
        for (int x : C)
        {
            vector<double> hs;
            for (int y : C)
                if (y != x)
                    hs.push_back(H[abs(y)][id]);
            U[id][abs(x)] = f(hs);
        }
    }

    double signal(var x)
    {
        assert(x > 0);
        double result = 0.;
        for (clause_id id : clauses_from[x])
        {
            if (id > 0)
                result -= U[id][x];
            else
                result += U[-id][x];
        }
        return tanh(result);
    }

    pair<double, double> extract_marginal(var x)
    {
        double result = signal(x);
        double pos = (1. - result) / 2.;
        double neg = (1. + result) / 2.;
        return {neg, pos};
    }

    void BP_round()
    {
        random_shuffle(ordering.begin(), ordering.end());
        for (auto p : ordering)
        {
            if (p.first)
            {
                clause_id id = p.second;
                if (clauses[id].size() == 0)
                    continue;
                if (clauses[id].size() == 1)
                {
                    // assert(cnf->clauses[id][0] == SATISFIED);
                    continue;
                }
                update_clause(p.second);
            }
            else if (!erased.count(p.second))
                update_var(p.second);
        }
    }

    int Propagation()
    {
        const int max_iter = 200;
        vector<double> previous_signal(N + 1, 0.5);
        int i;
        double max_diff = 0.;
        for (i = 0; i < max_iter; i++)
        {
            // cout << "Iteration: " << i + 1 << endl;
            BP_round();
            // print_state();
            max_diff = 0;
            for (var x = 1; x <= N; ++x)
            {
                double new_signal = signal(x);
                max_diff = max(max_diff, abs(new_signal - previous_signal[x]));
                previous_signal[x] = new_signal;
            }
            // cout << max_diff << endl;
            if (max_diff <= 1e-5)
                break;
        }
        // cout << max_diff << " " << i << endl;
        return i;
    }

    void print_state()
    {
        cout << "H(i->a)" << endl;
        for (var x = 1; x <= N; x++)
        {
            cout << x << ": ";
            for (auto p : H[x])
                cout << "(" << p.first << ": " << p.second << ") ";
            cout << endl;
        }
        cout << "U(a->i)" << endl;
        for (clause_id a = 0; a < M; a++)
        {
            cout << a << ": ";
            for (auto p : U[a])
                cout << "(" << p.first << ": " << p.second << ") ";
            cout << endl;
        }
        cout << "Marginals" << endl;
        for (int x = 1; x <= N; x++)
        {
            auto p = extract_marginal(x);
            cout << x << ": " << p.first << "/" << p.second << endl;
        }
        cout << "Printing clauses" << endl;
        print_vec(clauses);
        cout << "Printing inverse" << endl;
        print_vec(clauses_from);
    }

    vector<bool> solve()
    {
        for (int i = 0, t = 0; i < N; i++, t++)
        {
            int id = get_unit_clause();

            var x = get_pure_literal();

            // cout << i << ": " << endl;
            // cout << "unit clause: " << id << endl;
            // cout << "pure literal: " << x << endl;
            if (id != 0)
                satisfy(clauses[id][0]);
            else if (x != NOT_A_VAR)
                satisfy(x);
            else
            {
                int iter = Propagation();
                // cout << "BP: " << iter << endl;
                //  cout << iter << endl;
                var x_opt = -1;
                double max_marginal = 0;
                for (var x = 1; x <= N; x++)
                    if (!erased.count(x))
                    {
                        pair<double, double> p = extract_marginal(x);
                        // cout << "Bvar: " << x << endl;
                        // cout << "prob0: " << p.second << endl;
                        // cout << "prob1: " << p.first << endl;
                        if (p.first > max_marginal)
                            x_opt = -x, max_marginal = p.first;
                        if (p.second > max_marginal)
                            x_opt = x, max_marginal = p.second;
                    }
                // print_state();
                //  cout << "x_opt: " << x_opt << endl;
                // cout << "Bvar: " << x_opt << endl;
                // cout << "prob: " << max_marginal << endl;
                //   cout << "satisfying " << x_opt << " with marginal " << max_marginal << endl;
                satisfy(x_opt);
            }
        }
        // cout << "done forced steps" << t << endl;
        return assignment;
    }
};

int main(int argc, char *argv[])
{
    srand(time(NULL));
    int N = 100, k = 4;
    double r = 8.0;
    try
    {
        N = stoi(argv[1]);
        r = stod(argv[2]);
        k = stoi(argv[3]);
    }
    catch (...)
    {
        cout << "Usage: ./bp N r k" << endl;
        cout << "N: number of variables" << endl;
        cout << "r: ratio of clauses to variables" << endl;
        cout << "k: number of literals per clause" << endl;
        return 0;
    }
    int M = int(double(N) * r);
    cout << "Solving a random CNF with " << N << " variables, " << M << " clauses and " << k << " literals per clause" << endl;
    int tests = 50;
    int satisfied = 0;
    time_t avg_time = 0;
    for (int i = 0; i < tests; i++)
    {
        vector<clause> cnf = generate_CNF(N, r, k);
        vector<clause> cnf_copy;
        for (auto c : cnf)
            cnf_copy.push_back(c);
        auto t1 = high_resolution_clock::now();
        BP bp(cnf, N, M, k);
        vector<bool> assignement = bp.solve();
        auto t2 = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(t2 - t1).count();
        cout << "duration: " << duration << " ms" << endl;
        avg_time += duration;
        // print_vec(cnf_copy);
        if (satisfies(cnf_copy, assignement))
            satisfied++;
        cout << i << ": " << satisfies(cnf_copy, assignement) << endl;
    }
    cout << satisfied << "/" << tests << endl;
    cout << double(avg_time / tests) << endl;
}