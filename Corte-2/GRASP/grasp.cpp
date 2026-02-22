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
vector<int> local_search_light (vector<int> &x, vector<double> &weight, vector<double> &value, double &max_weight){
    int N = x.size();

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

    // cout << "Criteria: " << criteria << endl;

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