#include <iostream>
#include <vector>
#include <numeric>     
#include <algorithm>    
#include <random>       
#include <chrono>   
#include "conmons.h"

using namespace std;

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

vector<vector<int>> generate_population(int N, int population_size){
    
    vector<vector<int>> population;

    while(population.size() < population_size){
        vector<int> new_random_solution = random_solution(N);
        population.push_back(new_random_solution);
    }

    return population;

}

double f(vector<int> &solution){
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

void print_population(vector<vector<int>> &population) {
    
    cout << "[" << endl;
    for(vector<int> p : population){
        cout << "[";
        for(int i : p){
            cout << i << ",";
        }
        cout << "]" << endl;
    }
    cout << "]" << endl;

}

void print_vector(vector<int> vector){

    int i = 0;
    cout << "[";
    int size = vector.size();
    while(i < size){
        
        if(i == size - 1){
            cout << vector[i];
        } else {
            cout << vector[i] << ", ";
        }
        
        i++;
    }
    cout << "]" << endl;

}

vector<int> select_parent(vector<vector<int>> population, int population_size){

    double total_cost_function = 0;
    vector<int> population_function_cost(population_size, 0);
    
    int i = 0;
    for(vector<int> p : population){
        double cost = f(p);
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
        if(random_number < pfc){
            selected_parent = population[i];
            break; //Gracias
        }
        i++;
    }

    // cout << "Valor random: " << random_number << endl;
    // cout << "Valor total: " << total_cost_function << endl;
    // cout << "Padre seleccionado: [";
    // for(int sp : selected_parent){
    //     cout << sp << ",";
    // }
    // cout << "]" << endl;

    return selected_parent;
}

vector<vector<int>> generate_parents(vector<vector<int>> population, int population_size, int parents_size){
    
    vector<vector<int>> parents;

    while(parents.size() < parents_size){
        vector<int> new_parent = select_parent(population, population_size);
        parents.push_back(new_parent);
    }

    return parents;

}

int main(){

    int population_size = 10;
    int parents_size = 6;
    parser();
    vector<vector<int>> population = generate_population(N, population_size);

    vector<vector<int>> parents = generate_parents(population, population_size, parents_size);

    print_population(parents);

    // vector<int> selected_parent = select_parent(population, population_size);

    // print_vector(selected_parent);

    // print_population(population);

}