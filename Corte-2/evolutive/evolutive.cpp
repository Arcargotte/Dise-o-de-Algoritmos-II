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

vector<vector<int>> generate_parents(vector<vector<int>> population, int parents_size){
    
    vector<vector<int>> parents;

    while(parents.size() < parents_size){
        vector<int> new_parent = select_random(population);
        parents.push_back(new_parent);
    }

    return parents;

}

struct children{
    vector<int> first;
    vector<int> second;
};

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

vector<vector<int>> recombine_parents(vector<vector<int>> &parents, int children_size){

    // xd
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

    int population_size = 10;
    int parents_size = 6;
    int children_size = 5;
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