#include <iostream>
#include <vector>
#include <algorithm>
#include "../conmons.h"
using namespace std;

/**
 * @brief Implementa el algoritmo Tabu Search para el problema 1-0 Knapsack.
 * 
 * A partir de una solución inicial X obtenida mediante un algoritmo voraz estándar,
 * el procedimiento explora iterativamente la vecindad Vx con el objetivo de encontrar
 * soluciones de mayor calidad según la función de evaluación f.
 * 
 * La función de evaluación f está definida como:
 * 
 * f(x) = ∑ (vᵢ * xᵢ)  si  ∑ (wᵢ * xᵢ) <= W
 * f(x) = -1           si  ∑ (wᵢ * xᵢ) > W
 * 
 * Donde:
 * - vᵢ es el valor del i-ésimo ítem.
 * - wᵢ es el peso del i-ésimo ítem.
 * - W es la capacidad máxima del Knapsack.
 * - xᵢ ∈ {0,1} indica si el ítem i pertenece a la solución.
 * 
 * La vecindad utilizada es explicada en el código.
 * 
 * Para evitar ciclos y fomentar la exploración del espacio de búsqueda,
 * se emplea una lista tabu de tamaño fijo (50 posiciones) que almacena
 * representaciones binarias (unsigned long long) de soluciones recientemente
 * visitadas. Un movimiento es rechazado si conduce a una solución presente
 * en la lista tabu.
 * 
 * En cada iteración:
 * - Se selecciona el mejor vecino no tabu.
 * - Se actualiza la lista tabu de forma circular (FIFO).
 * - Se mantiene registro de la mejor solución global encontrada.
 * - Se guarda en archivo cada mejora global observada.
 * 
 * El algoritmo ejecuta un número fijo de iteraciones (1000).
 * 
 * @param weight Vector de pesos de los ítems, donde weight[i] corresponde al peso del i-ésimo ítem.
 * @param value Vector de valores de los ítems, donde value[i] corresponde al valor del i-ésimo ítem.
 * @param max_weight Capacidad máxima permitida del Knapsack.
 * @return Arreglo binario X[0..N-1] donde X[i] = 1 indica que el i-ésimo ítem está en el Knapsack y X[i] = 0 en caso contrario.
 */
vector<int> tabu_search (vector<double> &weight, vector<double> &value, double &max_weight){

    vector<unsigned long long> tabu_list(50, 0);
    
    // Algoritmo heurístico
    vector<int> x = standard_greedy_algorithm(weight, value, max_weight);
    int N = x.size();

    /**
     * @brief Obtiene el mejor vecino no tabu de una solución dada.
     * 
     * Dada una solución binaria x, explora su vecindad generando:
     * 
     * 1) Movimientos de intercambio (swap 1–0): reemplaza un ítem seleccionado
     *    por uno no seleccionado.
     * 2) Movimientos de flip: invierte individualmente cada posición.
     * 
     * Para cada vecino generado:
     * - Se evalúa con la función f.
     * - Se descarta si su representación binaria está en la lista tabu.
     * - Se selecciona el de mayor valor válido.
     * 
     * @param x Solución actual.
     * @return Mejor vecino admisible según f y la lista tabu.
     */
    auto bestNeighbor = [N, &tabu_list](vector<int> x){
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

    vector<int> solution = x;
    unsigned long long id = 0;
    id = ULLRepresentation(x);
    double prev_optimal = 0;
    double optimal =  f(x);

    unsigned long long iterations = 0;
    
    // Esquema de búsqueda tabú
    int iterTabuList = 0;
    while (iterations < 1000) {

        x = bestNeighbor(x);
        optimal =  f(x);
        id = ULLRepresentation(x);
        tabu_list[iterTabuList] = id;

        if (prev_optimal < optimal) {
            solution = x;
            prev_optimal = optimal;
        }

        iterations++;
        iterTabuList = (iterTabuList + 1) % 50;
    }
    return solution;

}


int main () {
    parser();

    vector<int> tabu_solution = tabu_search(weight, value, max_weight);

    print_solution(tabu_solution);

    double tabu_function_cost = f(tabu_solution);

    ofstream Optfile("../graphState/output/ts.csv");
    Optfile << "Solution,Value\n";
    Optfile << ULLRepresentation(tabu_solution) << "," << tabu_function_cost << "\n";
    Optfile.close();

    return 0;
}