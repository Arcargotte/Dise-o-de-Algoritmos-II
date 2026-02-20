#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <tuple>
#include "conmons.h"

using namespace std;
vector <int> bit;
unsigned long long id = 0;

tuple<double, double> f(vector<int> &solution, vector<double> &value, vector<double> &weight, double max_weight, int n)
{   
    double eval = 0;
    double w = 0;
    for (int i = 0; i < n; i++){
        if (solution[i] == 1){
            eval += value[i];
            w += weight[i];
        }
    }
    if (w > max_weight){
        eval = -1;
    }

    auto tuple_answer = make_tuple(eval, w);

    return tuple_answer;
};

int main(){

    parser();

    bit.assign(N, 0);

    double eval = 0;
    double w = 0;
    
    ofstream file("data.csv");

    file << "solution_id,beneficio\n";
    long long optimal = 0;
    long long optimal_representation = 0;

    unsigned long long total = 1ULL << N;

    for(unsigned long long id = 1; id < total; id++){

        for(int i = 0; i < N; i++){
            bit[N - 1 - i] = (id  >> i) & 1;
        }
        auto [eval, w] = f(bit, value, weight, max_weight, N);

        file << id << "," << eval << "\n";

        if(eval > optimal){
            optimal = eval;
            optimal_representation = id;
        }

    }
    
    file.close();

    ofstream Optfile("optimal.csv");
    Optfile << "Solution,Value\n";
    Optfile << optimal_representation << "," << optimal << "\n";
    Optfile.close();

    return 0;
}