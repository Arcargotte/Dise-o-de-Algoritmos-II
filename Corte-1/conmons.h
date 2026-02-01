#ifndef CONMONS
#define CONMONS

#include <vector>
#include <algorithm>

using namespace std;


std::vector<double> weight = {1, 2, 5, 6, 7};
std::vector<double> value = {1, 6, 18, 22, 28};
int N = weight.size();
double max_weight = 11;

// Registro usado por Heuristics_Solution.cpp
struct item {
    double density;
    int id;
};
// end

/**
 * Genera determinísticamente una solución X al problema 0-1 Knapsack
 * 
 * Produce un arreglo de densidades vi / wi para el i-ésimo ítem del problema y los ordena de forma descendente e ir incluyendo en el "knapsack" aquellos ítems de mayor a menor densidad
 * tales que cumplan con la condición wi + current_weight < max_weight; es decir, que el peso de los ítems en el knapsack no excedan el límite establecido por el problema.
 * 
 * @param weight Vector del peso de los ítems, donde weight[i] corresponde al peso del i-ésimo ítem del problema.
 * @param value Vector del valor de los ítems, donde value[i] corresponde al valor del i-ésimo ítem del problema.
 * @param max_weight Número real que define el peso máximo que puede cargar el Knapsack. 
 */
vector<int> standard_greedy_algorithm (vector<double> &weight, vector<double> &value, double &max_weight)
{    
    long N = weight.size();
    item density_arr [N];
    double knapsack_total_weight = 0;
    vector<int> knapsack_distribution (N,0);
    
    for(int i = 0; i < N; i++){
        density_arr[i].density = value[i] / weight[i];
        density_arr[i].id = i;
    }

    sort(density_arr, density_arr + N, [](const item &a, const item &b){
        return a.density > b.density; 
    });

    double total_weight = 0;
    double total_value = 0;

    for (int i = 0; i < N; i++){
        if (weight[density_arr[i].id] + total_weight <= max_weight){
            total_weight += weight[density_arr[i].id];
            total_value += value[density_arr[i].id];
            knapsack_distribution[density_arr[i].id] = 1;
        }
    }
    
    return knapsack_distribution;
}

#endif