#include <iostream>
#include <vector>
#include <algorithm>
#include "conmons.h"

using namespace std;

struct pair_density_item {
    double density;
    double item;
};

vector<int> local_search (vector<int> &x, vector<double> &weight, vector<double> &value, double &max_weight){
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

                neighbors.push_back(vx);

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

vector<int> iterative_local_search (vector<double> &weight, vector<double> &value, double &max_weight){
    // Create first solution
    vector<int> x = standard_greedy_algorithm(weight, value, max_weight);
    int N = x.size();
    vector<int> x_star = local_search(x, weight, value, max_weight);

    // end

    // for (int i = 0; i < N; i++){
    //     cout << "(" << density_arr[i].density << ", " << density_arr[i].item << ")"<< " ";
    // }

    // cout << endl;

    // Perturb function
    auto perturba_inador = [weight, value, max_weight, N](vector<int> x){
        int k = N / 4;
        vector<int> arr_ones;
        vector<int> arr_zeros;

        for (int i = 0; i < N; i++){
            if (x[i] == 1) {
                arr_ones.push_back(i);
            } else {
                arr_zeros.push_back(i);
            }
        }

        int ones_size = arr_ones.size();
        int zeros_size = arr_zeros.size();
        pair_density_item in_items_density [ones_size];
        pair_density_item out_items_density [zeros_size];


        for (int i = 0; i < ones_size; i++){
            in_items_density[i].density = value[arr_ones[i]] / weight[arr_ones[i]];
            in_items_density[i].item = arr_ones[i];
        }
        for (int i = 0; i < zeros_size; i++){
            out_items_density[i].density = value[arr_zeros[i]] / weight[arr_zeros[i]];
            out_items_density[i].item = arr_zeros[i];
        }

        sort(in_items_density, in_items_density + ones_size, [](const pair_density_item &a, const pair_density_item &b){
            return a.density > b.density; 
        });
        sort(out_items_density, out_items_density + zeros_size, [](const pair_density_item &a, const pair_density_item &b){
            return a.density < b.density; 
        });

        //Eliminate k densest items

        auto x_prima = x;

        if (k > ones_size){
            k = ones_size / 2;
        }

        for (int i = 0; i < k; i++){
            x_prima[in_items_density[i].item] = 0;
        }

        double current_weight = 0;
        for (int i = 0; i < k; i++){
            if (x_prima[i] == 1) {
                current_weight += weight[i];
            }
        }

        for (int i = 0; i < zeros_size; i++){
            if (current_weight + weight[out_items_density[i].item] <= max_weight){
                x_prima[out_items_density[i].item] = 1;
                current_weight += weight[out_items_density[i].item];
            }
        }

        return x_prima;
    };
        //end
        //end

        // cout << "in_items_density:\n";
        // for (int i = 0; i < ones_size; i++){
            
        //     cout << "(" << in_items_density[i].density << ", " << in_items_density[i].item << ")"<< " ";
        // }

        // cout << endl;
        // cout << "out_items_density:\n";
        // for (int i = 0; i < zeros_size; i++){
            
        //     cout << "(" << out_items_density[i].density << ", " << out_items_density[i].item << ")"<< " ";
        // }

        // cout << endl;

        // cout << "Solucion inicial: \n";
        // for (int i = 0; i < N; i++){
        //     cout << x[i] << " ";
        // }
        // cout << endl;

        // cout << "Solucion perturbada: \n";
        // for (int i = 0; i < N; i++){
        //     cout << x_prima[i] << " ";
        // }

        // cout << endl;

    // Criterio de aceptacion === Eval. function
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
    
    bool best_found = true;
    double optimal =  f(x_star);
    double prev_optimal = 0;

    while (best_found) {
        cout << "It-s PERTURBING TIME" << endl;
        auto x_prima = perturba_inador(x_star);
        auto x_prima_star = local_search(x_prima, weight, value, max_weight);

        prev_optimal = optimal;
        optimal =  f(x_prima_star);

        if (prev_optimal >= optimal) {
            best_found = false;
        } else{
            x_star = x_prima_star;
        }
    }
    return x_star;
}

int main () {

    // vector<int> knapsack_distribution = local_search(weight, value, MAX_WEIGHT);

    // double total_value = 0;

    // for (int i = 0; i < N; i++){
    //     if (knapsack_distribution[i] == 1) {
    //         total_value += value[i];
    //     }
    // }
    
    // cout << total_value << endl;
    // for (int i = 0; i < N; i++){
    //     if (knapsack_distribution[i] == 1) {
    //         cout << value[i] << " " << weight[i] << endl;
    //     }
    // }

    // cout << endl;
    parser();

    vector<int> knapsack_distribution = iterative_local_search(weight, value, max_weight);
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