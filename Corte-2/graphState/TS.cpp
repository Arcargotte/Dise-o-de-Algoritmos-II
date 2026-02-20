#include <iostream>
#include <vector>
#include <algorithm>
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

    vector<unsigned long long> tabu_list(50, 0);
    
    vector<int> x = standard_greedy_algorithm(weight, value, max_weight);
    int N = x.size();


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

    auto bestNeighbor = [N, f, &tabu_list](vector<int> x){
        vector<int> vx;
        vector<int> arr_ones;
        vector<int> arr_zeros;
        vector<int> best_solution;
        double optimal = -2;
        vector<vector<int>> neighbors = {};
        for (int i = 0; i < N; i++){
            vx = x;
            (vx[i] == 1) ? arr_ones.push_back(i) : arr_zeros.push_back(i);
        }

        for (int i = 0; i < arr_ones.size(); i++){
            for (int j = 0; j < arr_zeros.size(); j++){
                int pos_ones = arr_ones[i];
                int pos_zeros = arr_zeros[j];

                vx[pos_ones] = 0;
                vx[pos_zeros] = 1;

                double eval = f(vx);
                unsigned long long representationVx = ULLRepresentation(vx);

                if(eval > optimal && find(tabu_list.begin(), tabu_list.end(), representationVx) == tabu_list.end()){
                    optimal = eval;
                    best_solution = vx;
                }

                vx[pos_ones] = 1;
                vx[pos_zeros] = 0;
            }
        }
        for (int k = 0; k < vx.size(); k++){
            int temp = vx[k];
            if (vx[k] == 1) {
                vx[k] = 0;
            } else {
                vx[k] = 1;
            }
            double eval = f(vx);
            unsigned long long representationVx = ULLRepresentation(vx);

            if(eval > optimal && find(tabu_list.begin(), tabu_list.end(), representationVx) == tabu_list.end()){
                optimal = eval;
                best_solution = vx;
            }  
            vx[k] = temp;
        }

        return best_solution;
    };

    ofstream file("ts.csv");

    file << "Solution,Value\n";

    // vector <vector <int>> neighbors = {};
    vector<int> solution = x;
    unsigned long long id = 0;
    id = ULLRepresentation(x);
    double prev_optimal = 0;
    double optimal =  f(x);
    file << id << "," << optimal << "\n";

    unsigned long long iterations = 0;
    
    int iterTabuList = 0;
    while (iterations < 1000) {
        // cout << "tabu_list: [";
        // for(int i = 0; i < tabu_list.size(); i++){
        //     if(i == tabu_list.size() - 1){
        //         cout << tabu_list[i];
        //     } else{
        //         cout << tabu_list[i] << ", ";
        //     }
        // }
        // cout << "]" << endl;

        x = bestNeighbor(x);
        optimal =  f(x);
        id = ULLRepresentation(x);
        // cout << "Vecino nuevo: " << id << endl;
        tabu_list[iterTabuList] = id;

        if (prev_optimal < optimal) {
            file << id << "," << optimal << "\n";
            solution = x;
            prev_optimal = optimal;
        }

        iterations++;
        iterTabuList = (iterTabuList + 1) % 50;
    }
    file.close();
    return solution;

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