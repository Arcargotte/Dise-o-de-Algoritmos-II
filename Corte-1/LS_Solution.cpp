#include <iostream>
#include <vector>
using namespace std;

// Generate Initial Solution via Standard Greedy Algorithm
vector<int> standard_greedy_algorithm (vector<double> weight, vector<double> value, double max_weight)
{    
    long N = weight.size();
    double ratio [N];
    int items [N];
    double knapsack_total_weight = 0;
    vector<int> knapsack_distribution (N,0);
    
    for(int i = 0; i < N; i++){
        ratio[i] = value[i] / weight[i];
        items[i] = i;
    }

    double temp;
    int temp_int = 0;
    // Existen tres arreglos separados: uno de pesos, otro de valores y otros de densidad; hacemos bubbleSort para ordenar los tres arreglos simultaneamente en función de su densidad
    for (int i = 0; i < N; i++){
        for (int j = i; j < weight.size(); j++){
            if (ratio[i] < ratio[j]){
                temp = ratio[i];
                ratio[i] = ratio[j];
                ratio[j] = temp;

                temp = weight[i];
                weight[i] = weight[j];
                weight[j] = temp;

                
                temp = value[i];
                value[i] = value[j];
                value[j] = temp;

                temp_int = items[i];
                items[i] = items[j];
                items[j] = temp_int;
            }
        }
    }

    double total_weight = 0;
    double total_value = 0;

    for (int i = 0; i < N; i++){
        if (weight[i] + total_weight <= max_weight){
            total_weight += weight[i];
            total_value += value[i];
            knapsack_distribution[items[i]] = 1;
        }
    }
    return knapsack_distribution;
}

void local_search (vector<double> weight, vector<double> value, double max_weight){
    vector<int> x = standard_greedy_algorithm(weight, value, max_weight);
    int N = x.size();
    //Generate neighborhood of initial solution x

    auto neighborhood = [N](vector<int> x){
        vector<int> vx;
        vector<vector<int>> neighbors = {};
        for (int i = 0; i < N; i++){
            vx = x;
            if (x[i] == 1){
                vx[i] = 0;
            } else {
                vx[i] = 1;
            }

            for (int i = 0; i < N; i++){
                for (int j = 0; j < N; j++){

                }
            }



            neighbors.push_back(vx);
        }
        return neighbors;
    };

    // for (int i = 0; i < N; i++){
    //     for (int j = 0; j < N; j++){
    //         cout << neighborhood[i][j] << " ";
    //     }
    //     cout << endl;
    // }

    // end

    //Eval. function
    auto f = [value, weight, max_weight, N](vector<int> &solution)
    {
        double eval = 0;
        double w = 0;
        for (int i = 0; i < N; i++){
            if (solution[i] == 1){
                eval += value[i];
                w += weight[i];
            }
        }
        if (w > max_weight){
            eval = -1;
        }

        return eval;
    };
    // end

    int best_sol_found = false;
    vector <vector <int>> neighbors = {};
    while (!best_sol_found) {
        neighbors = neighborhood(x);
        for (int i = 0; i < N; i++){
            if (f(neighbors[i]) > f(x)){
                x = neighbors[i];
            } else {
                best_sol_found = true;
                break;
            }
        }
    }

}

int main () {
    int MAX_WEIGHT = 11;
    vector<double> weight = {1, 2, 5, 6, 7};
    vector<double> value = {1, 6, 18, 22, 28};
    vector<int> knapsack_distribution = standard_greedy_algorithm(weight, value, MAX_WEIGHT);
    int N = knapsack_distribution.size();

    local_search(weight, value, MAX_WEIGHT);


    // for (int i = 0; i < N; i++){
    //     cout << knapsack_distribution[i] << " ";
    // }

    cout << endl;
    return 0;
}