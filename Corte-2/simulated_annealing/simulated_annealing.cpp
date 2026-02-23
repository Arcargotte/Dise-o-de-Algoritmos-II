#include <iostream>
#include <vector>
#include <random>       
#include <chrono>   
#include <cmath>
#include "../conmons.h"

using namespace std;

double temp_ini = 100;
double alpha_t = 0.995;

/**
 * @brief Genera una permutación aleatoria de los enteros {0,1,...,N-1}.
 *
 * Construye un vector con los índices en orden natural y luego los
 * mezcla utilizando el algoritmo shuffle.
 *
 * @param N Tamaño de la permutación.
 * @return Vector<int> con una permutación aleatoria de tamaño N.
 */
vector<int> genPermutation(int N) {
    vector<int> perm(N);

    // llenar con 0,1,2,...,N-1
    iota(perm.begin(), perm.end(), 0);

    // generador aleatorio
    mt19937 rng(random_device{}());

    // mezclar
    shuffle(perm.begin(), perm.end(), rng);

    return perm;
}

/**
 * @brief Genera una solución inicial factible aleatoria para la mochila.
 *
 * Construye una permutación aleatoria de los índices y agrega ítems
 * en ese orden mientras no se exceda la capacidad máxima.
 *
 * @param N Número total de ítems.
 * @return Vector<int> Solución binaria factible generada aleatoriamente.
 */
vector<int> random_solution(int N){
    vector<int> perm = genPermutation(N);
    vector<int> x(N);
    double current_weight = 0;

    for(int i : perm){
        if(current_weight + weight[i] <= max_weight){
            x[i] = 1;
            current_weight += weight[i];
        }
    }

    return x;
}  
/**
 * @brief Genera el vecindario completo de una solución binaria.
 *
 * El vecindario está definido por:
 *   1) Intercambios 1-0 (swap entre un ítem seleccionado y uno no seleccionado).
 *   2) Flip individual de cada bit de la solución.
 *
 * Devuelve todas las soluciones vecinas generadas.
 *
 * @param x Solución binaria actual.
 * @return Vector<vector<int>> Conjunto de soluciones vecinas.
 */
auto neighborhood(vector<int> x){
    vector<int> vx;
    vector<int> arr_ones;
    vector<int> arr_zeros;
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

            neighbors.push_back(vx);

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
        neighbors.push_back(vx);
        vx[k] = temp; 
    }
    return neighbors;
}

/**
 * @brief Determina si se acepta una solución candidata según su probabilidad asignada.
 *
 * Si la solución candidata mejora la actual (delta >= 0), se acepta siempre.
 * Si empeora, se acepta con probabilidad:
 *
 *      exp((candidate_f - current_f) / t)
 *
 * donde t es la temperatura actual.
 *
 * @param current_f Valor de la función objetivo de la solución actual.
 * @param candidate_f Valor de la función objetivo de la solución candidata.
 * @param t Temperatura actual del algoritmo.
 * @return true si la solución es aceptada, false en caso contrario.
 */
bool check_probability(double current_f, double candidate_f, double t){

    double delta = candidate_f - current_f;

    if(delta >= 0)
        return true;

    double probability = exp(delta / t);  // delta negativo

    static mt19937 rng(random_device{}());
    uniform_real_distribution<double> dist(0.0, 1.0);

    return dist(rng) < probability;
}

/**
 * @brief Implementa la metaheurística Simulated Annealing para la mochila.
 *
 * Procedimiento:
 *   1) Genera una solución inicial aleatoria.
 *   2) Mientras no se alcance el número máximo de iteraciones:
 *        - Genera el vecindario.
 *        - Evalúa vecinos en orden aleatorio.
 *        - Acepta un vecino según el criterio probabilístico.
 *        - Actualiza la mejor solución encontrada.
 *        - Reduce la temperatura mediante enfriamiento geométrico.
 *
 * Utiliza el esquema de enfriamiento:
 *      t = t * alpha_t
 *
 * @return Vector<int> Mejor solución encontrada durante el proceso.
 */
vector<int> simulated_annealing(){

    vector<int> s_prima = random_solution(N);
    vector<int> s_star = s_prima;
    double iter_f = f(s_prima);
    double best_f = iter_f;
    double t = temp_ini;
    int iterations = 0;
    while(iterations < 50){
        int cooldown = 0;
        while(cooldown < 5){
            vector<vector<int>> neighbors = neighborhood(s_prima);
            
            mt19937 rng(random_device{}());
            shuffle(neighbors.begin(), neighbors.end(), rng);

            for(vector<int> s_prima_prima : neighbors){
                if(check_probability(iter_f, f(s_prima_prima), t)){
                    s_prima = s_prima_prima;
                    iter_f = f(s_prima);
                    double new_f = f(s_prima_prima);
                    if(new_f > best_f){
                        s_star = s_prima_prima;
                        best_f = new_f;
                    }
                    break;
                }
            }
            cooldown++;

        }
        t = t * alpha_t;
        iterations++;
    }

    return s_star;

}


int main(){

    parser();

    vector<int> solution = simulated_annealing();

    print_solution(solution);

    double solution_function_cost = f(solution);

    ofstream Optfile("../graphState/output/simulated_annealing.csv");
    Optfile << "Solution,Value\n";
    Optfile << ULLRepresentation(solution) << "," << solution_function_cost << "\n";
    Optfile.close();

}