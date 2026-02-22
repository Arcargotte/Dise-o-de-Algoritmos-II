#include <iostream>
#include <vector>
#include <numeric>     
#include <algorithm>    
#include <random>       
#include <chrono>
#include "../conmons.h"

using namespace std;

int probability_to_combine_parents = 60;
int probability_to_mutate = 5;
int population_size = 10;
int parents_size = 6;
int children_size = 5;

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
    // Generador aleatorio
    mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
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
vector<vector<int>> generate_population(int N, int population_size){
    
    vector<vector<int>> population;

    while(population.size() < population_size){
        vector<int> new_random_solution = random_solution(N);
        population.push_back(new_random_solution);
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
    vector<int> population_function_cost(population.size(), 0);
    
    int i = 0;
    for(vector<int> p : population){
        double cost = max(0.0, f(p));
        total_cost_function += cost;
        population_function_cost[i] = total_cost_function;
        i++;
    }

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    mt19937_64 engine(seed);

    uniform_int_distribution<int> dist(0, total_cost_function);

    int random_number = dist(engine);

    vector<int> selected_parent;
    i = 0;
    for(int pfc : population_function_cost){
        if(random_number <= pfc){
            selected_parent = population[i];
            break; //Gracias
        }
        
        i++;
    }

    return selected_parent;
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
children sex(vector<int> first_parent, vector<int> second_parent){

    vector<int> first_child;
    vector<int> second_child;

    for(int i = 0; i < first_parent.size(); i++){

        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        mt19937_64 engine(seed);

        uniform_int_distribution<int> dist(0, 100);

        int random_number = dist(engine);

        if(random_number >= probability_to_combine_parents){
            first_child.push_back(first_parent[i]);
            second_child.push_back(second_parent[i]);
        } else {
            first_child.push_back(second_parent[i]);
            second_child.push_back(first_parent[i]);
        }

    }

    return children({first_child, second_child});

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
            children new_children = sex(parents[i], parents[j]);
            childrens.push_back(new_children.first);
            childrens.push_back(new_children.second);
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


int main(){

    parser();
    vector<vector<int>> population = generate_population(N, population_size);

    int iterations = 0;

    while(iterations < 150){

        vector<vector<int>> parents = generate_parents(population, parents_size);

        vector<vector<int>> children = recombine_parents(parents, children_size);

        mutate_children(children);

        vector<int> the_best = select_the_best(population);
        
        // Juntar hijos con padres
        for(vector<int> c : children){
            population.push_back(c);
        }

        population = generate_parents(population, population_size - 1);
        population.push_back(the_best);

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