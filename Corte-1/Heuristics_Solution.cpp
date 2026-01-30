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

int main () {
    vector<double> weight = {1, 2, 5, 6, 7};
    vector<double> value = {1, 6, 18, 22, 28};
    int N = weight.size();

    vector<int> knapsack_distribution = standard_greedy_algorithm(weight, value, 11);

    for (int i = 0; i < N; i++){
        cout << knapsack_distribution[i] << " ";
    }

    cout << endl;
    return 0;
}