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
 * @return arreglo binario X[0..N] donde X[i] = 1 indica que el i-ésimo ítem está en el knapsack y X[i] = 0 que no.
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

/**
 *  Lee el archivo example.txt y actualiza las variables globales weight, value, N y max_weight.
 * 
 * Actualiza el arreglo de pesos, weight, el de valores, value, la cantidad de ítems de la instancia del problema N y el límite de peso max_weight, leyendo el archivo de 
 * texto plano example.txt que está especialmente formateado para que puedan inicializarse las variables mencionadas. Este formato consiste en:
 * 
 * <number of items>
 * <item 1> <value 1> <weight 1>
 * <item 2> <value 2> <weight 2>
 * ...
 * <item n> <value n> <weight n>
 * <capacity or max. weight>
 * 
 * Esta función se apoya del ejecutable generator.cpp y que a través del archivo makefile crea una instancia del problema 1-0 Knapsack en example.txt
 */
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
}

#endif