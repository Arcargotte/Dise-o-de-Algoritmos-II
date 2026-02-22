#include <iostream>
#include <vector>
#include <numeric>     
#include <algorithm>    
#include <random>       
#include <chrono>       
#include "../conmons.h"
using namespace std;

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
    mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());

    // mezclar
    shuffle(perm.begin(), perm.end(), rng);

    return perm;
}

/**
 * @brief Aplica búsqueda local sobre una solución binaria.
 *
 * A partir de una solución inicial x, genera el mejor vecino posible
 * mediante:
 *   1) Intercambios 1-0 (swap entre posiciones activas e inactivas).
 *   2) Flip individual de cada bit.
 *
 * En cada iteración selecciona el mejor vecino.
 * El proceso termina cuando no se encuentra una mejora en la función objetivo.
 *
 * @param x Solución inicial (vector binario 0/1). Se modifica si se encuentran mejoras.
 * @param weight Vector de pesos de los ítems.
 * @param value Vector de valores de los ítems.
 * @param max_weight Capacidad máxima de la mochila.
 * @return Vector<int> Mejor solución local encontrada.
 */
vector<int> local_search_light (vector<int> &x, vector<double> &weight, vector<double> &value, double &max_weight){
    int N = x.size();

        /**
     * @brief Genera el mejor vecino de una solución binaria según nuestro esquema de vecindad.
     *
     * Dada una solución x, explora su vecindario definido por:
     *   1) Intercambios 1-0: reemplaza un ítem seleccionado por uno no seleccionado.
     *   2) Flip individual: invierte cada bit de la solución.
     *
     * Evalúa cada vecino utilizando la función objetivo f() y mantiene
     * el mejor encontrado. Si ningún vecino mejora la solución original,
     * retorna la misma solución.
     *
     * @param x Solución binaria actual (0/1).
     * @return Vector<int> Mejor solución vecina encontrada.
     */
    auto bestNeighbor = [N](vector<int> x){
        vector<int> vx;
        vector<int> arr_ones;
        vector<int> arr_zeros;
        vector<int> best_solution = x;
        double optimal = f(x);
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
                if(eval > optimal){
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
            if(eval > optimal){
                optimal = eval;
                best_solution = vx;
            }  
            vx[k] = temp;
        }

        return best_solution;
    };

    vector <int> var_best_neighbor;
    bool best_found = true;
    double optimal =  f(x);
    double prev_optimal = 0;
    while (best_found) {
        best_found = false;
        optimal =  f(x);
        prev_optimal = optimal;
        var_best_neighbor = bestNeighbor(x);
        optimal = f(var_best_neighbor);

        if (optimal > prev_optimal) {
            x = var_best_neighbor;
            best_found = true;
        }
    }
    return x;
}

/**
 * @brief Genera una solución factible aleatoria para el problema de la mochila.
 *
 * Recorre los ítems en un orden aleatorio (permutación) y los agrega
 * mientras no se exceda la capacidad máxima.
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
 * @brief Construye la Lista Restringida de Candidatos (RCL) para GRASP.
 *
 * Calcula la densidad valor/peso de cada ítem factible que aún no ha sido
 * seleccionado. Luego determina un umbral basado en el parámetro alpha:
 *
 *      criterio = max_density + alpha * (min_density - max_density)
 *
 * Los ítems cuya densidad sea mayor o igual al criterio se agregan a la RCL.
 *
 * @param current_weight Peso actual acumulado en la solución parcial.
 * @param alpha Parámetro de control de aleatorización (0 <= alpha <= 1).
 * @param chosen Vector con los índices ya seleccionados.
 * @return Vector<item> Lista restringida de candidatos factibles.
 */
vector<item> restricted_candidate_list (double current_weight, double alpha, vector<int> chosen){

    long N = weight.size();
    item density_arr [N];
    vector<int> knapsack_solution(N,0);
    vector<item> rcl;
    
    double min_density = -1;
    double max_density = -1;

    for(int i = 0; i < N; i++){
        double density = value[i] / weight[i];
        density_arr[i].density = density;
        density_arr[i].id = i;
        bool was_chosen = false;

        for(int c : chosen){
            if(c == i){
                was_chosen = true;
                break;
            }
        }

        if(current_weight + weight[i] <= max_weight && !was_chosen){
            if(max_density == -1 || density > max_density){
                max_density = density;
            }

            if(min_density == -1 || density < min_density && !was_chosen){
                min_density = density;
            }
        }

    }

    double criteria = max_density + alpha * (min_density - max_density);

    for(item i : density_arr){

        bool was_chosen = false;

        for(int c : chosen){
            if(c == i.id){
                was_chosen = true;
                break;
            }
        }

        if(current_weight + weight[i.id] <= max_weight && i.density >= criteria && !was_chosen){
            rcl.push_back(i);
        }
    }

    return rcl;
}

/**
 * @brief Construye una solución usando la fase constructiva de GRASP.
 *
 * Iterativamente:
 *   1) Genera la Lista Restringida de Candidatos (RCL).
 *   2) Selecciona aleatoriamente un elemento de la RCL.
 *   3) Lo agrega a la solución si es factible.
 *
 * El proceso termina cuando la RCL queda vacía.
 *
 * @param N Número total de ítems.
 * @return Vector<int> Solución binaria construida mediante greedy aleatorizado.
 */
vector<int> random_greedy_solution(int N){

    int i = 0;
    double current_weight = 0;
    vector<int> partial_solution(N, 0);
    vector<int> chosen;
    while (true){
        
        vector<item> rcl = restricted_candidate_list(current_weight, 0.75, chosen);

        if(rcl.empty()){
            break;
        }

        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        mt19937_64 engine(seed); // Using a Mersenne Twister engine

        // 2. Define the distribution for the desired range [min_num, max_num]
        uniform_int_distribution<int> dist(0, rcl.size() - 1);

        int random_number = dist(engine);

        item the = rcl[random_number];
        partial_solution[the.id] = 1;
        current_weight += weight[the.id];
        chosen.push_back(the.id);

        i++;
    }

    return partial_solution;

}

/**
 * @brief Implementa la metaheurística GRASP para el problema de la mochila.
 *
 * Ejecuta múltiples iteraciones compuestas por:
 *   1) Construcción de una solución greedy aleatoria.
 *   2) Mejora mediante búsqueda local (best-improvement).
 *   3) Actualización de la mejor solución global encontrada.
 *
 * @param N Número total de ítems.
 * @return Vector<int> Mejor solución encontrada tras las iteraciones de GRASP.
 */
vector<int> grasp(int N){

    vector<int> s = random_solution(N);
    int iteration = 0;
    while(iteration < 10){
        vector<int> s_prima = random_greedy_solution(N);
        vector<int> s_prima_star = local_search_light(s_prima, weight, value, max_weight);

        if(f(s_prima_star) > f(s)){
            s = s_prima_star;
        }
        iteration++;
    }

    return s;

}

int main(){
    parser();

    vector<int> grasp_solution = grasp(N);

    print_solution(grasp_solution);

    double grasp_function_cost = f(grasp_solution);

    ofstream Optfile("../graphState/output/grasp.csv");
    Optfile << "Solution,Value\n";
    Optfile << ULLRepresentation(grasp_solution) << "," << grasp_function_cost << "\n";
    Optfile.close();

} 