#include <iostream>
#include <vector>
#include <numeric>     
#include <algorithm>    
#include <random>       
#include <chrono>
#include <unordered_map>
#include "../conmons.h"

using namespace std;

int population_size = 30;
int max_population_size = 50;
int orca_hunger = 20;
mt19937_64 rng(chrono::steady_clock::now().time_since_epoch().count());

enum Gender {
    MALE, 
    FEMALE
};

struct Penguin{
    vector<int> solution;
    Gender gender;

    bool operator==(const Penguin& other) const {
        return solution == other.solution && gender == other.gender;
    }
};

struct Couple{
    Penguin he;
    Penguin she;
};

struct solution{
    vector<int> gen_rep;
    double f_eval;
};

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
vector<Penguin> generate_population(int N, int population_size, vector<double> & weight, vector<double> & value, double &max_weight){
    
    vector<Penguin> population;
    unordered_map<unsigned long long, int> population_hash;

    // vector<int> heuristic_solution = standard_greedy_algorithm(weight, value, max_weight);
    // population.push_back(heuristic_solution);
    // print_vector(heuristic_solution);
    int counter = 0;
    int population_counter = 0;
    bool stuck = false;
    while(population.size() < population_size){
        vector<int> new_random_solution = random_solution(N);

        Penguin new_penguin = {new_random_solution, (population_counter % 2 == 0) ? MALE : FEMALE};

        unsigned long long dec_rep = ULLRepresentation(new_random_solution);
        int exists = population_hash.count(dec_rep);
        if(exists == 0){
            population.push_back(new_penguin);
            population_hash[dec_rep] = 0;
            population_counter++;
        }
        if(counter == 2 * population_size){
            stuck = true;
            break;
        }
        counter++;
    }

    if(stuck){
        while(population.size() < population_size){
            vector<int> new_random_solution = random_solution(N);
            Penguin new_penguin = {new_random_solution, (population_counter % 2 == 0) ? MALE : FEMALE};
            population.push_back(new_penguin);
            population_counter++;
        }
    }
    
    return population;
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


vector<Couple> make_couples(vector<Penguin> &population){

    vector<Penguin> male_penguins;
    vector<Penguin> female_penguins;
    vector<Couple> couples;

    for(Penguin p : population){
        if(p.gender == MALE){
            male_penguins.push_back(p);
        } else {
            female_penguins.push_back(p);
        }
    }

    sort(male_penguins.begin(), male_penguins.end(), [](Penguin &a, Penguin &b){
        return f(a.solution) > f(b.solution); 
    });

    sort(female_penguins.begin(), female_penguins.end(), [](Penguin &a, Penguin &b){
        return f(a.solution) > f(b.solution); 
    });

    // cout << "Pingüinos: " << endl;
    
    // for(Penguin p : male_penguins){
    //     print_vector(p.solution);
    //     cout << "Eval: " << f(p.solution) << endl;
    // }

    // cout << "Pingüinas: " << endl;

    // for(Penguin p : female_penguins){
    //     print_vector(p.solution);
    //     cout << "Eval: " << f(p.solution) << endl;
    // }

    int index = 0;
    if(male_penguins.size() < female_penguins.size()){
        for(Penguin p : male_penguins){
            couples.push_back({p, female_penguins[index]});
            index++;
        }
    } else {
        for(Penguin p : female_penguins){
            couples.push_back({p, male_penguins[index]});
            index++;
        }
    }
    

    // for(Couple c : couples){
    //     cout << "He:" << endl;
    //     print_vector(c.he.solution);
    //     cout << "She:" << endl;
    //     print_vector(c.she.solution);
    // }

    return couples;

}

auto w(vector<int> &solution)
{
    double w = 0;
    for (int i = 0; i < N; i++){
        if (solution[i] == 1){
            w += weight[i];
        }
    }

    return w;
};

solution bestNeighbor(vector<int> &x, int &N, vector<double> &value, vector<double> &weight, double &max_weight){
    vector<int> vx;
    vector<int> arr_ones;
    vector<int> arr_zeros;
    vector<int> best_solution = x;
    double optimal = f(x);
    double actual_f = optimal;
    double actual_w = w(x);
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

            double eval = 0;
            if(actual_w - weight[pos_ones] + weight[pos_zeros] <= max_weight){
                eval = actual_f - value[pos_ones] + value[pos_zeros];
            } else {
                eval = -1;
            }

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
        double eval;
        if (vx[k] == 1) {
            vx[k] = 0;
            if(actual_w - weight[k] <= max_weight){
                eval = actual_f - value[k];
            } else {
                eval = -1;
            }
        } else {
            vx[k] = 1;
            if(actual_w + weight[k] <= max_weight){
                eval = actual_f + value[k];
            } else {
                eval = -1;
            }
        }
        
        if(eval > optimal){
            optimal = eval;
            best_solution = vx;
        }  
        vx[k] = temp;
    }

    return {best_solution, optimal};
};


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
void local_search_light (vector<int> &x, vector<double> &weight, vector<double> &value, double &max_weight){
    int N = x.size();

    vector <int> var_best_neighbor;
    bool best_found = true;
    double prev_optimal = f(x);
    while (best_found) {
        best_found = false;
        solution best_solution = bestNeighbor(x, N, value, weight, max_weight);
        if (best_solution.f_eval > prev_optimal) {
            x = best_solution.gen_rep;
            best_found = true;
            prev_optimal = best_solution.f_eval;
        }
    }
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
bool female_food_search (vector<int> &x, vector<double> &weight, vector<double> &value, double &max_weight, int max_iterations){
    int N = x.size();

    vector <int> var_best_neighbor;
    bool improved = false;
    double prev_optimal = f(x);
    int iters = 0;
    bool best_found = true;
    while (iters < max_iterations && best_found) {
        solution best_solution = bestNeighbor(x, N, value, weight, max_weight);
        if (best_solution.f_eval > prev_optimal) {
            x = best_solution.gen_rep;
            improved = true;
            best_found = true;
            prev_optimal = best_solution.f_eval;
        }
        iters++;
    }

    return improved;
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
vector<int> random_greedy_solution(int N, double alpha){

    int i = 0;
    double current_weight = 0;
    vector<int> partial_solution(N, 0);
    vector<int> chosen;
    while (true){
        
        vector<item> rcl = restricted_candidate_list(current_weight, alpha, chosen);

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
vector<int> grasp(int N, double alpha){

    vector<int> s = random_solution(N);
    int iteration = 0;
    while(iteration < 10){
        vector<int> s_prima_star = random_greedy_solution(N, alpha);
        vector<int> s_prima = s_prima_star;

        local_search_light(s_prima_star, weight, value, max_weight);

        if(f(s_prima_star) > f(s)){
            s = s_prima_star;
        }
        iteration++;
    }

    return s;

}


vector<Penguin> penguin_genocide_by_orcas(vector<Penguin> population){

    sort(population.begin(), population.end(), [](Penguin &a, Penguin &b){
        return f(a.solution) > f(b.solution); 
    });
    vector<Penguin> new_population = {population.begin(), population.end() - orca_hunger};

    return new_population;

}


int main(int argc, char* argv[]){

    parser();

    vector<Penguin> population = generate_population(N, population_size, weight, value, max_weight);


    vector<int> best_solution;
    double best_f = -1;

    // for(Penguin p : population){
    //     print_vector(p.solution);
    //     cout << "Género: " << p.gender << endl;
    // }

    int iterations = 0;
    while(iterations < 50){

        if(population.size() == 0){
            break;
        }

        vector<Couple> couples = make_couples(population);

        if(couples.size() == 0){
            break;
        }

        double apt_best_male = f(couples[0].he.solution);
        double apt_best_female = f(couples[0].she.solution);

        double new_best_f;
        Gender chosen;

        if(apt_best_male > apt_best_female){
            new_best_f = apt_best_male;
            chosen = MALE;
        } else {
            new_best_f = apt_best_female;
            chosen = FEMALE;
        }

        if(new_best_f > best_f){
            best_f = new_best_f;
            best_solution = chosen == MALE ? couples[0].he.solution : couples[0].she.solution;
        }

        vector<Penguin> children;
        vector<Penguin> survivors;
        int iter_couples = 0;
        for(Couple &couple : couples){
            bool death = false;
            Penguin female = couple.she;
            // cout << "Previous: " << endl; 
            vector<int> female_solution = female.solution;
            // print_vector(female_solution);

            // vector<int> previous_female_solution = female_solution;
            death = !(female_food_search(female_solution, weight, value, max_weight, 1));
            
            // cout << "Death: " << death << endl;

            // cout << "After: " << endl; 
            // print_vector(female_solution);

            double apt_female_solution = f(female_solution);

            Penguin male = couple.he;

            double apt = f(male.solution) + apt_female_solution;
            double alpha = apt / (apt_best_male + apt_best_female);

            children.push_back({grasp(N, alpha), iter_couples % 2 == 0 ? MALE : FEMALE});

            // cout << "Alpha: " << alpha << endl;

            if(!death){
                survivors.push_back(couple.she);
                survivors.push_back(couple.he);
            }

            iter_couples++;
        }

        population = survivors;

        for(Penguin child : children){
            population.push_back(child);
        }

        if(population.size() > max_population_size){
            population = penguin_genocide_by_orcas(population);
        }

        cout << "Población: " << population.size() << endl;

        iterations++;
    }

    print_solution(best_solution);

}