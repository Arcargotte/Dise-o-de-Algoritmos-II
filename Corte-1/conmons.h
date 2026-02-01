#ifndef CONMONS
#define CONMONS

#include <vector>
#include <algorithm>
#include <fstream>

using namespace std;


std::vector<double> weight;
std::vector<double> value;
int N;
double max_weight;

// Registro usado por Heuristics_Solution.cpp e ILS_Solution
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

void parser() 
{
    int num_items;

    ifstream inputFile("example.txt"); // Open the file named "example.txt"
    string word;

    if (!inputFile.is_open()) {
        cerr << "Error: Could not open the file." << endl;
    }
    
    if (inputFile >> word){
        num_items = stoi(word);
    }

    int banned = 0;
    int i= 0;
    // cout << "Numero de objetos: "<< num_items << endl;
    while (i < num_items * 3 && inputFile >> word) {
        if (banned == 1){
            value.push_back(stod(word));
        } else if (banned == 2){
            weight.push_back(stod(word));
        }

        banned = (banned + 1) % 3;

        i++;
    }
    
    if (inputFile >> word){
        max_weight = stod(word);
    }
    
    inputFile.close(); // Close the file stream

    N = num_items;
    
    // for (int i = 0; i < num_items; i++){
    //     cout << value[i] << " " << weight[i] << endl;
    // }
    // cout << "max_weight: " << max_weight << endl;
}

#endif