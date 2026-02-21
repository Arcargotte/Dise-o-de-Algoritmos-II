#include <iostream>
#include <vector>
#include <random>       
#include <chrono>   
#include <cmath>
#include "conmons.h"

using namespace std;

double temp_ini = 1000;
double alpha_t = 0.9;

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
            //end
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


bool check_probability(vector<int> x, double best_f, double t){

    double e = exp(1.0);
    double sustr = best_f - f(x);

    double probability;

    if(sustr < 0){
        probability = 1 / exp(sustr / t);
    } else {
        probability = 1;
    }

    int upper_limit = floor(probability * 100);

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    mt19937_64 engine(seed);

    uniform_int_distribution<int> dist(0, 100);

    int random_number = dist(engine);

    return random_number <= upper_limit;

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


vector<int> simulated_annealing(){

    vector<int> s_prima = random_solution(N);
    vector<int> s_star = s_prima;
    double iter_f = f(s_prima);
    double best_f = iter_f;
    double t = temp_ini;
    int iterations = 0;
    while(iterations < 1200){
        int cooldown = 0;
        while(cooldown < 10){
            vector<vector<int>> neighbors = neighborhood(s_prima);
            
            mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
            shuffle(neighbors.begin(), neighbors.end(), rng);

            for(vector<int> s_prima_prima : neighbors){
                if(check_probability(s_prima_prima, iter_f, t)){
                    // cout << "s_prima_prima: " << endl;
                    print_vector(s_prima_prima);

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

    double total_value = 0;

    for (int i = 0; i < N; i++){
        if (solution[i] == 1) {
            total_value += value[i];
        }
    }
    
    cout << total_value << endl;
    for (int i = 0; i < N; i++){
        if (solution[i] == 1) {
            cout << value[i] << " " << weight[i] << endl;
        }
    }

    cout << endl;

}