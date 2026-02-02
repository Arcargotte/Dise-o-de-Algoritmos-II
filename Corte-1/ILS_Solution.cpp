#include <iostream>
#include <vector>
#include <algorithm>
#include "conmons.h"

using namespace std;

/**
 * Realiza búsqueda local clásica sobre una solución X.
 * 
 * Se distingue de la búsqueda local clásica porque en la búsqueda local iterativa aplica búsqueda local sobre una solución que se le entrega como argumento y la otra solo requiere las variables del
 * problema.
 * 
 * A partir de una solución X al problema 1-0 Knapsack dado (que consigue a partir de un algoritmo voraz estándar), explora su vecindad Vx en búsqueda de un vecino estrictamente
 * mejor según la función de evaluación f, donde f viene definida por la siguiente ecuación
 * 
 * f(x) = ∑ (vᵢ * xᵢ) si  ∑ (wᵢ * xᵢ) <= W; f(x) = -1 si ∑ (wᵢ * xᵢ) > W
 * 
 * Con xi siendo una función booleana que evalúa a 1 si el i-ésimo objeto forma parte de la solución x. Resumidamente, la f(x) retorna el valor acumulado de los ítems en la solución x si el peso
 * no excede el límite W y retorna -1 en el caso contrario.
 * 
 * Sobre la vecindad usada en la búsqueda local: la vecindad de una solución x define una vecindad pesada que tiende a explorar todo el espacio de soluciones en la búsqueda de soluciones.
 * @param x Solución a partir de la cual se hace búsqueda local.
 * @param weight Vector del peso de los ítems, donde weight[i] corresponde al peso del i-ésimo ítem del problema.
 * @param value Vector del valor de los ítems, donde value[i] corresponde al valor del i-ésimo ítem del problema.
 * @param max_weight Número real que define el peso máximo que puede cargar el Knapsack. 
 */
vector<int> local_search (vector<int> &x, vector<double> &weight, vector<double> &value, double &max_weight){
    int N = x.size();
    //Generate neighborhood of initial solution x

    auto neighborhood = [N](vector<int> x){
        vector<int> vx;
        vector<int> arr_ones;
        vector<int> arr_zeros;
        vector<vector<int>> neighbors = {};
        for (int i = 0; i < N; i++){
            vx = x;
            (vx[i] == 1) ? arr_ones.push_back(i) : arr_zeros.push_back(i);

        }
        vector<int> banned_positions (vx.size(), 0);

        for (int i = 0; i < arr_ones.size(); i++){
            for (int j = 0; j < arr_zeros.size(); j++){
                int pos_ones = arr_ones[i];
                int pos_zeros = arr_zeros[j];

                for (int ones = 0; ones < arr_ones.size() && i != 0 && ones <= i; ones++){
                    banned_positions[arr_ones[ones]] = 1;
                }

                for (int zeros = 0; zeros < arr_zeros.size() && j != 0 && zeros <= j; zeros++){
                    banned_positions[arr_zeros[zeros]] = 1;
                }
                vx[pos_ones] = 0;
                vx[pos_zeros] = 1;

                neighbors.push_back(vx);

                for (int k = 0; k < vx.size(); k++){
                    if (banned_positions[k] == 0) {
                        int temp = vx[k];
                        if (vx[k] == 1) {
                            vx[k] = 0;
                        } else {
                            vx[k] = 1;
                        }
                        neighbors.push_back(vx);
                        vx[k] = temp;
                    }
                    
                }

                //Reset variable section
                for (int ones = 0; ones < arr_ones.size() && i != 0 && ones <= i; ones++){
                    banned_positions[arr_ones[ones]] = 0;
                }

                for (int zeros = 0; zeros < arr_zeros.size() && j != 0 && zeros <= j; zeros++){
                    banned_positions[arr_zeros[zeros]] = 0;
                }
                vx[pos_ones] = 1;
                vx[pos_zeros] = 0;
                //end
            }
        }

        return neighbors;
    };

    //Eval. function
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
    // end

    vector <vector <int>> neighbors = {};
    bool best_found = false;
    double optimal =  f(x);
    double prev_optimal = 0;
    while (!best_found) {
        optimal =  f(x);
        prev_optimal = optimal;
        neighbors = neighborhood(x);

        for (int i = 0; i < neighbors.size(); i++){
            if (f(neighbors[i]) > optimal){
                x = neighbors[i];
                optimal = f(x);
            }
        }

        if (prev_optimal >= optimal) {
            best_found = true;
        }

    }

    return x;

}

/**
 * Realiza búsqueda local iterada sobre una solución X.
 * 
 * A partir de una solución X al problema 1-0 Knapsack generada por la heurística especializada (el algoritmo voraz), realizamos una búsqueda local sobre x y obtenemos una solución x* sobre la cual
 * se realiza la perturbación p (perturba-inador) con la que generamos la solución x' y sobre la cual se realiza otra búsqueda local con la que obtenemos la solución x'*. 
 * Teniendo las soluciones x* y x*', comparamos cuál es mejor entre ellas según nuestro criterio de aceptación (que es, sencillamente, aplicar nuestra función de evaluación). Si x* se mantiene
 * como la mejor solución, entonces aplicamos nuestro criterio de terminación y la retornamos como mejor solución; si x*' resulta mejor, lo guardamos en x* y entonces exploramos ahora su 
 * vecindad buscando una solución mejor, repitiendo el proceso descrito hasta ahora.
 * 
 * Criterio de aceptación: f(x*) < f(x'*), donde f(x) = ∑ (vᵢ * xᵢ) si  ∑ (wᵢ * xᵢ) <= W; f(x) = -1 si ∑ (wᵢ * xᵢ) > W, con xi siendo una función booleana que evalúa a 1 si el i-ésimo objeto 
 * forma parte de la solución x. Resumidamente, la f(x) retorna el valor acumulado de los ítems en la solución x si el peso no excede el límite W y retorna -1 en el caso contrario.
 * 
 * Criterio de terminación: x* es mejor que x*'.
 * 
 * @param weight Vector del peso de los ítems, donde weight[i] corresponde al peso del i-ésimo ítem del problema.
 * @param value Vector del valor de los ítems, donde value[i] corresponde al valor del i-ésimo ítem del problema.
 * @param max_weight Número real que define el peso máximo que puede cargar el Knapsack. 
 */
vector<int> iterative_local_search (vector<double> &weight, vector<double> &value, double &max_weight){
    // Create first solution
    vector<int> x = standard_greedy_algorithm(weight, value, max_weight);
    int N = x.size();
    vector<int> x_star = local_search(x, weight, value, max_weight);

    // end

    // Perturb function
    auto perturba_inador = [weight, value, max_weight, N](vector<int> x){
        int k = N / 4;
        vector<int> arr_ones;
        vector<int> arr_zeros;

        for (int i = 0; i < N; i++){
            if (x[i] == 1) {
                arr_ones.push_back(i);
            } else {
                arr_zeros.push_back(i);
            }
        }

        int ones_size = arr_ones.size();
        int zeros_size = arr_zeros.size();
        item in_items_density [ones_size];
        item out_items_density [zeros_size];


        for (int i = 0; i < ones_size; i++){
            in_items_density[i].density = value[arr_ones[i]] / weight[arr_ones[i]];
            in_items_density[i].id = arr_ones[i];
        }
        for (int i = 0; i < zeros_size; i++){
            out_items_density[i].density = value[arr_zeros[i]] / weight[arr_zeros[i]];
            out_items_density[i].id = arr_zeros[i];
        }

        sort(in_items_density, in_items_density + ones_size, [](const item &a, const item &b){
            return a.density > b.density; 
        });
        sort(out_items_density, out_items_density + zeros_size, [](const item &a, const item &b){
            return a.density < b.density; 
        });

        //Eliminate k densest items

        auto x_prima = x;

        if (k > ones_size){
            k = ones_size / 2;
        }

        for (int i = 0; i < k; i++){
            x_prima[in_items_density[i].id] = 0;
        }

        double current_weight = 0;
        for (int i = 0; i < k; i++){
            if (x_prima[i] == 1) {
                current_weight += weight[i];
            }
        }

        for (int i = 0; i < zeros_size; i++){
            if (current_weight + weight[out_items_density[i].id] <= max_weight){
                x_prima[out_items_density[i].id] = 1;
                current_weight += weight[out_items_density[i].id];
            }
        }

        return x_prima;
    };
    

    // Criterio de aceptacion === Eval. function
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
    // end
    
    bool best_found = true;
    double optimal =  f(x_star);
    double prev_optimal = 0;

    while (best_found) {
        // cout << "It-s PERTURBING TIME" << endl;
        auto x_prima = perturba_inador(x_star);
        auto x_prima_star = local_search(x_prima, weight, value, max_weight);

        prev_optimal = optimal;
        optimal =  f(x_prima_star);

        if (prev_optimal >= optimal) {
            best_found = false;
        } else{
            x_star = x_prima_star;
        }
    }
    return x_star;
}

int main () {

    parser();

    vector<int> knapsack_distribution = iterative_local_search(weight, value, max_weight);
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