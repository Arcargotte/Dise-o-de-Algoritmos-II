#include <iostream>
#include <vector>
#include <algorithm>
#include "conmons.h"

using namespace std;

// /**
//  * Genera determinísticamente una solución X al problema 0-1 Knapsack
//  * 
//  * Produce un arreglo de densidades vi / wi para el i-ésimo ítem del problema y los ordena de forma descendente e ir incluyendo en el "knapsack" aquellos ítems de mayor a menor densidad
//  * tales que cumplan con la condición wi + current_weight < max_weight; es decir, que el peso de los ítems en el knapsack no excedan el límite establecido por el problema.
//  * 
//  * @param weight Vector del peso de los ítems, donde weight[i] corresponde al peso del i-ésimo ítem del problema.
//  * @param value Vector del valor de los ítems, donde value[i] corresponde al valor del i-ésimo ítem del problema.
//  * @param max_weight Número real que define el peso máximo que puede cargar el Knapsack. 
//  */

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

    return 0;
}