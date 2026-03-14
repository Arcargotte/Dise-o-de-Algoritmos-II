#include <iostream>
#include <vector>
#include <numeric>     
#include <algorithm>    
#include <random>       
#include <chrono>
#include <unordered_map>
#include "../conmons.h"

using namespace std;

int probability_to_combine_parents = 60;
int probability_to_mutate = 3;
int population_size = 10;
int parents_size = 6;
int children_size = 5;
mt19937_64 rng(chrono::steady_clock::now().time_since_epoch().count());

/**
 * @brief Genera una permutación aleatoria de los índices {0, ..., N-1}.
 * 
 * Construye un vector de tamaño N con valores consecutivos desde 0 hasta N-1
 * y posteriormente aplica una mezcla aleatoria uniforme (shuffle) utilizando
 * un generador pseudoaleatorio basado en el reloj del sistema.
 * 
 * Esta función se utiliza para producir órdenes aleatorios que permiten
 * diversificar la construcción de soluciones factibles.
 * 
 * @param N Número de elementos a permutar.
 * @return Vector con una permutación aleatoria de tamaño N.
 */
vector<int> genPermutation(int N) {
    vector<int> perm(N);

    // Llenar con 0,1,2,...,N-1
    iota(perm.begin(), perm.end(), 0);

    mt19937_64 rng(chrono::steady_clock::now().time_since_epoch().count());

    // Mezclar
    shuffle(perm.begin(), perm.end(), rng);

    return perm;
}

/**
 * @brief Genera una solución factible aleatoria para el problema 1-0 Knapsack.
 * 
 * A partir de una permutación aleatoria de los ítems, construye una solución
 * binaria x de tamaño N siguiendo un procedimiento voraz aleatorizado:
 * 
 * - Recorre los ítems según el orden aleatorio generado.
 * - Incluye el ítem i (x[i] = 1) si su incorporación no excede la capacidad máxima.
 * - En caso contrario, lo descarta.
 * 
 * El resultado es una solución factible (no sobrepasa max_weight),
 * aunque no necesariamente óptima.
 * 
 * @param N Número total de ítems del problema.
 * @return Vector binario x[0..N-1] representando una solución factible.
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


void print_vector(vector<int> vector){

    cout << "[";
    int i = 0;
    while(i < vector.size()){
        if(i != vector.size() - 1){
            cout << vector[i] << ", ";
        } else{
            cout << vector[i];
        }
        i++;
    }
    cout << "]" << endl;

}


/**
 * @brief Genera una población inicial de soluciones para un algoritmo evolutivo.
 * 
 * Construye un conjunto de tamaño population_size compuesto por soluciones
 * factibles generadas mediante random_solution.
 * 
 * La población inicial introduce diversidad en el espacio de búsqueda,
 * permitiendo que el algoritmo explore distintas regiones del espacio
 * de soluciones desde el inicio.
 * 
 * @param N Número total de ítems del problema.
 * @param population_size Número de individuos que conformarán la población inicial.
 * @return Vector de soluciones binarias que representa la población inicial.
 */
vector<vector<int>> generate_population(int N, int population_size, vector<double> & weight, vector<double> & value, double &max_weight){
    
    vector<vector<int>> population;
    unordered_map<unsigned long long, int> population_hash;

    // vector<int> heuristic_solution = standard_greedy_algorithm(weight, value, max_weight);
    // population.push_back(heuristic_solution);
    // print_vector(heuristic_solution);

    while(population.size() < population_size){
        vector<int> new_random_solution = random_solution(N);
        unsigned long long dec_rep = ULLRepresentation(new_random_solution);
        int exists = population_hash.count(dec_rep);
        if(exists == 0){
            population.push_back(new_random_solution);
            population_hash[dec_rep] = 0;
        }
    }

    return population;
}

/**
 * @brief Selecciona un individuo mediante selección proporcional (ruleta).
 * 
 * Implementa un mecanismo de selección basado en aptitud:
 * 
 * 1) Se calcula la suma acumulada de valores f(p) para todos los individuos,
 *    considerando únicamente valores no negativos.
 * 2) Se genera un número aleatorio en el intervalo [0, suma_total].
 * 3) Se retorna el primer individuo cuya suma acumulada supere
 *    el valor aleatorio generado.
 * 
 * Este método favorece la selección de individuos con mayor aptitud,
 * manteniendo al mismo tiempo diversidad poblacional.
 * 
 * @param population Conjunto de soluciones candidatas.
 * @return Individuo seleccionado probabilísticamente según su aptitud.
 */
vector<int> select_random(vector<vector<int>> population){

    double total_cost_function = 0;
    vector<double> population_function_cost(population.size(), 0);

    for(int i = 0; i < population.size(); i++){
        double cost = max(0.0, f(population[i]));
        total_cost_function += cost;
        population_function_cost[i] = total_cost_function;
    }

    if(total_cost_function == 0){
        uniform_int_distribution<int> dist(0, population.size()-1);
        return population[dist(rng)];
    }

    uniform_real_distribution<double> dist(0.0, total_cost_function);
    double random_number = dist(rng);

    for(int i = 0; i < population.size(); i++){
        if(random_number <= population_function_cost[i])
            return population[i];
    }

    return population.back(); // fallback seguro
}

/**
 * @brief Selecciona un subconjunto de padres desde la población actual.
 * 
 * Genera un conjunto de tamaño parents_size utilizando un mecanismo
 * de selección probabilística (select_random), típicamente basado
 * en la aptitud (fitness) de cada individuo.
 * 
 * El procedimiento permite que individuos con mayor valor de f
 * tengan mayor probabilidad de ser elegidos como padres,
 * favoreciendo así la presión selectiva del algoritmo genético.
 * 
 * @param population Población actual de soluciones.
 * @param parents_size Número de padres a seleccionar.
 * @return Conjunto de soluciones seleccionadas como padres.
 */
vector<vector<int>> generate_parents(vector<vector<int>> population, int parents_size){
    
    vector<vector<int>> parents;

    while(parents.size() < parents_size){
        vector<int> new_parent = select_random(population);
        parents.push_back(new_parent);
    }

    return parents;

}

/**
 * @brief Estructura que representa un par de hijos generados por recombinación.
 * 
 * Contiene dos soluciones binarias resultantes del cruce
 * entre dos individuos padres.
 */
struct children{
    vector<int> first;
    vector<int> second;
    vector<int> third;
};

/**
 * @brief Realiza el operador de cruce (sex) entre dos padres.
 * 
 * Para cada posición i del cromosoma:
 * - Con probabilidad (100 - probability_to_combine_parents)%
 *   el hijo hereda el gen del mismo padre.
 * - En caso contrario, intercambia los genes entre padres.
 * 
 * El operador produce dos descendientes combinando
 * información genética de ambos padres.
 * 
 * @param first_parent Primer individuo padre.
 * @param second_parent Segundo individuo padre.
 * @return Estructura children con los dos descendientes generados.
 */
children sex(vector<int> first_parent, vector<int> second_parent, vector<int> third_parent){

    vector<int> first_child;
    vector<int> second_child;
    vector<int> third_child;

    for(int i = 0; i < first_parent.size(); i++){

        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        mt19937_64 engine(seed);

        uniform_int_distribution<int> dist(0, 100);

        int random_number = dist(engine);

        vector<int> bits = {first_parent[i], second_parent[i], third_parent[i]};

        if(random_number <= 33){
            first_child.push_back(bits[0]);
            bits.erase(bits.begin());
        } else if(random_number <= 66)  {
            first_child.push_back(bits[1]);
            bits.erase(bits.begin() + 1);
        } else if(random_number <= 100){
            first_child.push_back(bits[2]);
            bits.erase(bits.begin() + 2);
        }

        random_number = dist(engine);

        if(random_number <= 50){
            second_child.push_back(bits[0]);
            bits.erase(bits.begin());
        } else {
            second_child.push_back(bits[1]);
            bits.erase(bits.begin() + 1);
        }

        third_child.push_back(bits[0]);

    }

    return children({first_child, second_child, third_child});

} 

/**
 * @brief Genera descendencia a partir de un conjunto de padres.
 * 
 * Aplica el operador de cruce (sex) a todos los pares distintos
 * de padres (i < j), generando múltiples hijos.
 * 
 * Posteriormente, selecciona children_size individuos de entre
 * los descendientes generados utilizando selección probabilística,
 * conformando así la nueva generación parcial de hijos.
 * 
 * @param parents Conjunto de individuos seleccionados como padres.
 * @param children_size Número de hijos a conservar.
 * @return Vector con los hijos seleccionados.
 */
vector<vector<int>> recombine_parents(vector<vector<int>> &parents, int children_size){
    vector<vector<int>> childrens;

    for(int i = 0; i < parents.size(); i++){
        for(int j = i + 1; j < parents.size(); j++){
            for(int l = j + 1; l < parents.size(); l++){
                children new_children = sex(parents[i], parents[j], parents[l]);
                childrens.push_back(new_children.first);
                childrens.push_back(new_children.second);
                childrens.push_back(new_children.third);
            }
        }
    }

    vector<vector<int>> selected_children;

    while(selected_children.size() < children_size){
        vector<int> new_child = select_random(childrens);
        selected_children.push_back(new_child);
    }

    return selected_children;

}

/**
 * @brief Aplica el operador de mutación a un conjunto de hijos.
 * 
 * Para cada individuo:
 * - Con probabilidad probability_to_mutate% se realiza una mutación.
 * - La mutación consiste en seleccionar una posición aleatoria
 *   y alternar su valor binario (0 ↔ 1).
 * 
 * Este operador introduce variabilidad genética en la población,
 * ayudando a evitar convergencia prematura y a explorar nuevas
 * regiones del espacio de búsqueda.
 * 
 * @param children Conjunto de soluciones descendientes a mutar.
 */
void mutate_children(vector<vector<int>> &children){

    for(int i = 0; i < children.size(); i++){

        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        mt19937_64 engine(seed);

        uniform_int_distribution<int> dist(0, 100);

        int random_number = dist(engine);

        if(random_number < probability_to_mutate){

            uniform_int_distribution<int> second_dist(0, N - 1);

            int random_index = second_dist(engine);

            if (children[i][random_index] == 0){
                children[i][random_index] = 1;
            } else {
                children[i][random_index] = 0;
            }

        }

    }

    return;

}

/**
 * @brief Selecciona el mejor individuo de la población actual.
 * 
 * Recorre todos los individuos de la población y evalúa su aptitud
 * mediante la función f. Mantiene en memoria aquel con mayor valor
 * de evaluación encontrado durante el recorrido.
 * 
 * Este operador implementa una estrategia elitista, permitiendo
 * preservar el mejor individuo de una generación para evitar
 * pérdida de calidad en la evolución.
 * 
 * @param population Conjunto de soluciones binarias.
 * @return Individuo con mayor valor de f dentro de la población.
 */
vector<int> select_the_best(vector<vector<int>> &population){

    double best_f = -2;
    vector<int> selected;

    for(vector<int> p : population){
        double new_f = f(p);
        if(new_f > best_f){
            selected = p;
            best_f = new_f;
        }
    }

    return selected;

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





void get_good(vector<vector<int>> &population, vector<double> &weight, vector<double> &value, double &max_weight){

    int i = 0;
    while(i < population.size()){
        population[i] = local_search_light(population[i], weight, value, max_weight);
        i++;
    }

}

int distance(vector<int> &solution_one, vector<int> &solution_two){

    int i = 0;
    int distance = 0;
    while(i < solution_one.size()){
        if(solution_one[i] != solution_two[i]){
            distance++;
        }
        i++;
    }

    return distance;
}

vector<unsigned long long> calculate_distances(vector<vector<int>> &population){

    vector<unsigned long long> distances_map;

    for(vector<int> p : population){
        unsigned long long distance_acc = 0;
        unsigned long long dec_rep_p = ULLRepresentation(p);

        for(vector<int> q : population){
            
            unsigned long long dec_rep_q = ULLRepresentation(q);

            if(p != q){
                distance_acc += distance(p, q);
            }

        }

        distances_map.push_back(distance_acc);
    }

    return distances_map;
}

vector<vector<int>> diversity(vector<vector<int>> &population, vector<unsigned long long> &distances_map, int population_size){

    unsigned long long total_distance_function = 0;
    vector<double> population_function_cost(population.size(), 0);

    for(int i = 0; i < population.size(); i++){
        unsigned long long cost = distances_map[i];
        total_distance_function += cost;
        population_function_cost[i] = total_distance_function;
    }

    uniform_int_distribution<unsigned long long> dist(0, total_distance_function);
    vector<vector<int>> new_population; 

    for(int i = 0; new_population.size() < population_size; i++){
        unsigned long long random_number = dist(rng);
        for(int i = 0; i < population.size(); i++){
            if(random_number <= population_function_cost[i]){
                new_population.push_back(population[i]);
                break;
            }
        }
    }

    return new_population;

}

vector<int> best_path_link(vector<vector<int>> &population, int index_1, int index_2){

    vector<int> individual_1 = population[index_1];
    vector<int> individual_2 = population[index_2];

    vector<int> best_individual = individual_1;
    double best_f = -2;

    vector<int> iter_individual = individual_1;
    int i = 0;
    int counter = 0;
    while(i < individual_1.size()){
        int target = individual_2[i];
        if(individual_1[i] != target){
            iter_individual[i] = target;
            counter++;
            double new_f = f(iter_individual);
            if(new_f > best_f){
                best_individual = iter_individual;
                best_f = new_f;
            }
        }
        i++;
    }

    if(counter == 0){
        return individual_1;
    } else{
        return best_individual;
    }

}

vector<int> path_linking(vector<vector<int>> &population){

    uniform_int_distribution<unsigned long long> dist(0, population.size() - 1);

    vector<int> index_used;
    bool used;
    int index_1;
    int index_2;

    vector<int> best_individual;
    double best_f = -2; 

    int i = 0;
    while(i < 2){
        used = true;
        while(used){
            index_1 = dist(rng);
            vector<int>::iterator it = find(index_used.begin(), index_used.end(), index_1);

            if (it == index_used.end()) {
                used = false;
                index_used.push_back(index_1);
            }
        }
        
        used = true;
        while(used){
            index_2 = dist(rng);
            vector<int>::iterator it = find(index_used.begin(), index_used.end(), index_2);

            if (it == index_used.end()) {
                used = false;
                index_used.push_back(index_2);
            }
        }

        vector<int> iter_individual = best_path_link(population, index_1, index_2);
        double iter_f = f(iter_individual);
        if(iter_f > best_f){
            best_individual = iter_individual;
            best_f = iter_f;
        }

        i++;
    } 
    
    return best_individual;
}

int main(){

    parser();
    vector<vector<int>> population = generate_population(N, population_size, weight, value, max_weight);

    int iterations = 0;
    while(iterations < 150){

        vector<vector<int>> parents = generate_parents(population, parents_size);
        vector<vector<int>> children = recombine_parents(parents, children_size);

        mutate_children(children);
        get_good(children, weight, value, max_weight);

        vector<int> the_best = select_the_best(population);

        // Juntar hijos con padres
        for(vector<int> c : children){
            population.push_back(c);
        }

        vector<unsigned long long> distances = calculate_distances(population);
        population = diversity(population, distances, population_size - 2);
        population.push_back(the_best);
        vector<int> best_path_link = path_linking(population);
        population.push_back(best_path_link);

        iterations++;
    }

    vector<int> solution = select_the_best(population);

    print_solution(solution);

    double function_cost = f(solution);

    ofstream Optfile("../graphState/output/evolutive.csv");
    Optfile << "Solution,Value\n";
    Optfile << ULLRepresentation(solution) << "," << function_cost << "\n";
    Optfile.close();

}