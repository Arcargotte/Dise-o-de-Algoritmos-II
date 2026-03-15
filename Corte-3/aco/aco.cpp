#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <unordered_map>
#include <map>
#include "../conmons.h"


using namespace std;

int ant_amount = 25;
int ticks = 60;
double rho = 0.50;
int alpha = 1;
int beta = 1;
mt19937_64 rng(chrono::steady_clock::now().time_since_epoch().count());

struct edge {
    int x;
    int y;
    bool operator<(const edge& other) const {
        if (x != other.x) return x < other.x;
        return y < other.y;
    }
};

struct item_prob{
    int index;
    double probability;
};

struct solution{
    unordered_map<int, int> items_used;
    double profit;
};

int move(unordered_map<int, int> &items_used, int last_item_added, double &max_weight, map<edge, double> &edges_used, vector<double> &value, vector<double> &weight, double & actual_weight, int N){

    vector<item> moves;
    vector<item_prob> acc_density_vector; 
    double acc_density = 0.0;

    for(int i = 0; i < N; i++){
        int exists = items_used.count(i);
        if(exists == 0 && weight[i] + actual_weight <= max_weight){
            double new_density = value[i]/weight[i];
            if (last_item_added == -1){
                acc_density += new_density;
                acc_density_vector.push_back({i, acc_density});
            } else {    
                edge first_edge = {i, last_item_added};
                edge second_edge = {last_item_added, i};
                int edge_exists = edges_used.count(first_edge);
                int edge_exists_prima = edges_used.count(second_edge);
                if(edge_exists == 0 && edge_exists_prima == 0){
                    acc_density_vector.push_back({i, acc_density + new_density * 1});
                    acc_density += new_density * 1;
                } else if(edge_exists != 0){
                    acc_density_vector.push_back({i, acc_density + new_density * edges_used[first_edge]});
                    acc_density += new_density * edges_used[first_edge];
                } else if(edge_exists_prima != 0){
                    acc_density_vector.push_back({i, acc_density + new_density * edges_used[second_edge]});
                    acc_density += new_density * edges_used[second_edge];
                }
            }
            item new_item = {new_density, i};
            moves.push_back(new_item);
        }
    } 

    if(moves.size() == 0){
        return -1;
    }

    uniform_real_distribution<double> dist(0.0, acc_density);
    double random_number = dist(rng);

    bool not_found = true;
    int i = 0;
    while(not_found){
        if(random_number < acc_density_vector[i].probability){
            not_found = false;
        }
        i++;
    }
    i--;

    return acc_density_vector[i].index;
}

double evaluation_fuction(unordered_map<int, int> &items_used, vector<double> &value){
    double acc_profit = 0.0;
    for (const auto& [key, _] : items_used) {
        acc_profit += value[key];
    }
    return acc_profit;
}


solution ant_road(vector<double> &value, vector<double> &weight, double &max_weight, map<edge, double> &edges_used, int N){

    map<edge, int> current_edges_used;
    unordered_map<int, int> items_used;

    double actual_weight = 0;
    bool road_ended = false;
    int last_item_added = -1; 
    while(!road_ended){
        int new_item = move(items_used, last_item_added, max_weight, edges_used, value, weight, actual_weight, N);

        if(new_item == -1){
            road_ended = true;
        } else {
            items_used[new_item] = 0;
            actual_weight += weight[new_item];
            edge first_edge = {new_item, last_item_added};
            edge second_edge = {last_item_added, new_item};
            int edge_exists = edges_used.count(first_edge);
            int edge_exists_prima = edges_used.count(second_edge);
            if(edge_exists == 0 && edge_exists_prima == 0){
                edges_used[{last_item_added, new_item}] = 1;
            }
            current_edges_used[{last_item_added, new_item}] = 0;
            last_item_added = new_item;
        }

    }

    double profit = evaluation_fuction(items_used, value);

    for (const auto& [key, value] : current_edges_used) {
        edges_used[key] += profit;
    }

    return {items_used, profit};

}

int main(int argc, char* argv[]){

    if (argc < 2) {
        cerr << "Uso: " << argv[0] << " <ruta_archivo>" << endl;
        return 1;
    }
    string ruta_instancia = argv[1];
    parser_w_filename(ruta_instancia);

    int ant_iterations = 0;
    int t = 0;
    map<edge, double> edges_used;
    solution best_ant_solution = {{}, -1.0};

    while(t < ticks){
        ant_iterations = 0;
        while(ant_iterations < ant_amount){
            solution new_solution = ant_road(value, weight, max_weight, edges_used, N);
            if(new_solution.profit > best_ant_solution.profit){
                best_ant_solution = new_solution;
            }
            ant_iterations++;
        }
        for (const auto& [key, value] : edges_used) {
            edges_used[key] = edges_used[key] * rho + 1;
        }
        t++;
    }

    // cout << best_ant_solution.profit << endl;
    // for (const auto& [key, _] : best_ant_solution.items_used) {
    //     cout << value[key] << " " << weight[key] << endl;
    // }

    ofstream out("../../temp_res.txt");
    out << best_ant_solution.profit;
    out.close();
}