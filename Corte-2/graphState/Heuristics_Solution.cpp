#include <iostream>
#include <vector>
#include <algorithm>
#include "../conmons.h"

using namespace std;


int main () {

    parser();

    vector<int> knapsack_distribution = standard_greedy_algorithm(weight, value, max_weight);

    double total_value = 0;

    for (int i = 0; i < N; i++){
        if (knapsack_distribution[i] == 1) {
            total_value += value[i];
        }
    }
    
    cout << total_value << endl;
    for (int i = 0; i < N; i++){
        if (knapsack_distribution[i] == 1) {
            cout << value[i] << " " << weight[i] << endl;
        }
    }

    cout << endl;

    long long binary_id = 0;

    for (int i = 0; i < knapsack_distribution.size(); i++) {
        binary_id  = (binary_id  << 1) | knapsack_distribution[i];
    }

    ofstream file("./output/heuristic.csv");

    file << "Solution,Value\n";

    file << binary_id << "," << total_value << "\n";

    file.close();

    return 0;
}