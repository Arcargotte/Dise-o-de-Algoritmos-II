#include <iostream>
#include <vector>
#include <algorithm>
#include <math.h>
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

    auto bestNeighbor = [N, f, decimal2BinaryArray](vector<int> x, unsigned long long tabu_movement){
        vector<int> vx;
        vector<int> best_solution;
        double optimal = -2;
        vector<vector<int>> neighbors = {};
        
        unsigned long long numberRepresentation = ULLRepresentation(x);
        unsigned long long suc = numberRepresentation + 1;
        unsigned long long pre = numberRepresentation - 1;
        vector<int> numberSuc = decimal2BinaryArray(numberRepresentation + 1);
        vector<int> numberPre = decimal2BinaryArray(numberRepresentation - 1);

        double evalNumberPre;
        double evalNumberSuc;

        if(numberRepresentation == 0){
            evalNumberSuc = f(numberSuc);
            if(evalNumberSuc > optimal && suc != tabu_movement){
                optimal = evalNumberSuc;
                best_solution = numberSuc;
            }

        } else if (numberRepresentation == pow(2, N) - 1){

            evalNumberPre = f(numberPre);

            if(evalNumberPre > optimal && pre != tabu_movement){
                optimal = evalNumberPre;
                best_solution = numberPre;
            }

        } else {
            
            evalNumberPre = f(numberPre);
            evalNumberSuc = f(numberSuc);

            if(evalNumberPre > optimal && pre != tabu_movement){
                optimal = evalNumberPre;
                best_solution = numberPre;
            }
            
            if(evalNumberSuc > optimal && suc != tabu_movement){
                optimal = evalNumberSuc;
                best_solution = numberSuc;
            }
        }
        

        return best_solution;
    };

    ofstream file("tsbnn.csv");

    file << "Solution,Value\n";

    bool best_found = false;
    unsigned long long id = 0;
    double optimal =  f(x);
    id = ULLRepresentation(x);
    file << id << "," << optimal << "\n";
    double prev_optimal = optimal;
    unsigned long long iterations = 0;

    vector<int> heuristicSolution = x;
    vector<int> solution = x;
    
    // cout << "-----------------------------" << endl;
    // cout << "Solución de partida: " << id << endl;
    // cout << "-----------------------------" << endl;


    unsigned long long second_tabu_movement = 0;
    unsigned long long tabu_movement = 0;
    unsigned long long maxReachableID = pow(2, N) - 1;

    while (iterations < 1500) {
        iterations++;

        if(id == 0 || id == maxReachableID){
            break;
        }

        x = bestNeighbor(x, tabu_movement);
        tabu_movement = id;

        optimal =  f(x);
        id = ULLRepresentation(x);

        // Guardar el primer mejor vecino seleccionado
        if(iterations == 1){
            second_tabu_movement = id;
        }

        if (prev_optimal < optimal) {
            prev_optimal = optimal;
            solution = x;
        }

        
        // cout << "Nuevo vecino: " << endl;

        // cout << "[";
        // for(int i = 0; i < N; i++){
        //     if(i == N - 1){
        //         cout << x[i];
        //     } else{
        //         cout << x[i] << ", ";
        //     }
        // }
        // cout << "]" << endl;
        // cout << "Que es: " << ULLRepresentation(x) << endl; 

        // cout << "--------------------------------" << endl;
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