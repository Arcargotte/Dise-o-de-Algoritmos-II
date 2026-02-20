#include <iostream>
#include <vector>
#include "conmons.h"
using namespace std;

unsigned long long ULLRepresentation(vector<int> knapsack_distribution){
    unsigned long long id = 0;
    
    for (unsigned long long i = 0; i < knapsack_distribution.size(); i++) {
        id = (id << 1) | knapsack_distribution[i];
    }

    return id;
}



/**
 * Realiza búsqueda local clásica sobre una solución X.
 * 
 * A partir de una solución X al problema 1-0 Knapsack dado (que consigue a partir de un algoritmo voraz estándar), explora su vecindad Vx en búsqueda de un vecino estrictamente
 * mejor según la función de evaluación f, donde f viene definida por la siguiente ecuación
 * 
 * f(x) = ∑ (vᵢ * xᵢ) si  ∑ (wᵢ * xᵢ) <= W; f(x) = -1 si ∑ (wᵢ * xᵢ) > W
 * 
 * Con xi siendo una función booleana que evalúa a 1 si el i-ésimo objeto forma parte de la solución x. Resumidamente, la f(x) retorna el valor acumulado de los ítems en la solución x si el peso
 * no excede el límite W y retorna -1 en el caso contrario. 
 * 
 *  * Sobre la vecindad usada en la búsqueda local: la vecindad de una solución x define una vecindad ligera cuyos únicos vecinos son aquellas posibles soluciones que mantienen un número constante
 * de ítems en el Knapsack.
 * 
 * @param weight Vector del peso de los ítems, donde weight[i] corresponde al peso del i-ésimo ítem del problema.
 * @param value Vector del valor de los ítems, donde value[i] corresponde al valor del i-ésimo ítem del problema.
 * @param max_weight Número real que define el peso máximo que puede cargar el Knapsack.
 * @return arreglo binario X[0..N] donde X[i] = 1 indica que el i-ésimo ítem está en el knapsack y X[i] = 0 que no.
 */
vector<int> local_search (vector<double> &weight, vector<double> &value, double &max_weight){

    vector<int> x = standard_greedy_algorithm(weight, value, max_weight);
    int N = x.size();


    auto decimal2BinaryArray = [N](unsigned long long n){
        vector<int> binary_array(N, 0);
        if (n == 0){
            return binary_array;
        }

        int i = N - 1;
        while (n > 0) {
            binary_array[i] = n % 2;
            n = n / 2;
            i -= 1;       
        }

        return binary_array;
    };

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

    auto bestNeighbor = [N, f, decimal2BinaryArray](vector<int> x){
        vector<int> vx;
        vector<int> arr_ones;
        vector<int> arr_zeros;
        vector<int> best_solution;
        double optimal = -2;
        vector<vector<int>> neighbors = {};
        
        unsigned long long numberRepresentation = ULLRepresentation(x);
        vector<int> numberSuc = decimal2BinaryArray(numberRepresentation + 1);
        vector<int> numberPre = decimal2BinaryArray(numberRepresentation - 1);

        double evalNumberPre = f(numberPre);
        double evalNumberSuc = f(numberSuc);

        if(evalNumberPre > optimal){
            optimal = evalNumberPre;
            best_solution = numberPre;
        }
        
        if(evalNumberSuc > optimal){
            optimal = evalNumberSuc;
            best_solution = numberSuc;
        }

        return best_solution;
    };

    ofstream file("lsbnn.csv");

    file << "Solution,Value\n";

    vector<int> varBestNeighbor;
    bool best_found = false;
    unsigned long long id = 0;
    double optimal =  f(x);
    id = ULLRepresentation(x);
    file << id << "," << optimal << "\n";
    double prev_optimal = 0;
    while (!best_found) {
        prev_optimal = optimal;
        optimal =  f(x);
        varBestNeighbor = bestNeighbor(x);

        if (prev_optimal >= optimal) {
            best_found = true;
            id = ULLRepresentation(x);
            file << id << "," << optimal << "\n";
        }

    }
    file.close();
    return x;

}



int main () {
    parser();

    vector<int> knapsack_distribution = local_search(weight, value, max_weight);

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