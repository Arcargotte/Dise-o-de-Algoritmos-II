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

vector<int> local_search (vector<double> weight, vector<double> value, double max_weight){
    // vector<int> x = standard_greedy_algorithm(weight, value, max_weight);
    vector<int> x = {1, 0, 1, 0, 1};
    int N = x.size();
    //Generate neighborhood of initial solution x

    auto neighborhood = [N](vector<int> x){
        vector<int> vx;
        vector<int> arr_ones;
        vector<int> arr_zeros;
        vector<vector<int>> neighbors = {};
        for (int i = 0; i < N; i++){
            vx = x;
            (vx[i] == 1) ? arr_ones.push_back(i) : arr_zeros.push_back(i);

        }
        vector<int> banned_positions (vx.size(), 0);

        for (int i = 0; i < arr_ones.size(); i++){
            for (int j = 0; j < arr_zeros.size(); j++){
                int pos_ones = arr_ones[i];
                int pos_zeros = arr_zeros[j];

                for (int ones = 0; ones < arr_ones.size() && i != 0 && ones <= i; ones++){
                    banned_positions[arr_ones[ones]] = 1;
                }

                for (int zeros = 0; zeros < arr_zeros.size() && j != 0 && zeros <= j; zeros++){
                    banned_positions[arr_zeros[zeros]] = 1;
                }
                vx[pos_ones] = 0;
                vx[pos_zeros] = 1;

                for (int k = 0; k < vx.size(); k++){
                    if (banned_positions[k] == 0) {
                        int temp = vx[k];
                        if (vx[k] == 1) {
                            vx[k] = 0;
                        } else {
                            vx[k] = 1;
                        }
                        neighbors.push_back(vx);
                        vx[k] = temp;
                    }
                    
                }

                //Reset variable section
                for (int ones = 0; ones < arr_ones.size() && i != 0 && ones <= i; ones++){
                    banned_positions[arr_ones[ones]] = 0;
                }

                for (int zeros = 0; zeros < arr_zeros.size() && j != 0 && zeros <= j; zeros++){
                    banned_positions[arr_zeros[zeros]] = 0;
                }
                vx[pos_ones] = 1;
                vx[pos_zeros] = 0;
                //end
            }
        }

        return neighbors;
    };
    // vector<vector<int>> neighbors = neighborhood(x);


    // cout << "Tamaño de vecindad " << neighbors.size() << endl;

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

    vector <vector <int>> neighbors = {};
    bool best_found = false;
    double optimal =  f(x);
    double prev_optimal = 0;
    while (!best_found) {
        optimal =  f(x);
        prev_optimal = optimal;
        neighbors = neighborhood(x);

        // cout << "El BARRIO: " << endl;
        // for (int i = 0; i < neighbors.size(); i++){
            
        //     for (int j = 0; j < N; j++){
                
        //         cout << neighbors[i][j] << " ";
        //     }
        //     cout << endl;
        // }

        for (int i = 0; i < neighbors.size(); i++){
            if (f(neighbors[i]) > optimal){
                x = neighbors[i];
                optimal = f(x);
                cout << "Optimal: " << optimal << endl;
            }
        }

        if (prev_optimal >= optimal) {
            best_found = true;
        }

    }

    return x;

}

int main () {
    int MAX_WEIGHT = 11;
    vector<double> weight = {1, 2, 5, 6, 7};
    vector<double> value = {1, 6, 18, 22, 28};
    int N = weight.size();

    vector<int> knapsack_distribution = local_search(weight, value, MAX_WEIGHT);

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

    return 0;
}