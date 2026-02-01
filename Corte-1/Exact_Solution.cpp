// implementacion de la solucion exacta de knapsack problem

// el problema consiste en colocar n elementos que consisten de p (profit) y w (peso) en una mochila
// de tamaño W (capacidad)


// vamos a crear una matriz para los elementos 


#include <iostream>
#include <vector>
#include "conmons.h"

using namespace std;

void Print2DMtx(vector<vector<double>> m, double f, double c){
    for (double i = 0; i < f; i++)
    {
        for (double j = 0; j <= c; j++)
        {
            cout << m[i][j] << '\t';
        }
        cout << "\n";
    }
    cout << "\n";
}

vector<double> ExactSolution(long long n, long long k, vector<double> profit, vector<double> weight){

    // 0 es el peso | 1 es el profit
    vector<vector<double>> elements = {weight, profit};
    vector<vector<double>> knapsack (n, vector<double>(k + 1,0));


    // aplicamos el algoritmo exacto
    for (int i = 0; i < n; i++)
    {
        double v = elements[1][i];
        double w = elements[0][i];

        for (int j = 0; j <= k; j++)
        {
            long long jj = j - w;
            long long ii = i - 1;
            
            if (i == 0){
                if (w <= j){
                    knapsack[i][j] = v;
                }
            }
            else if(jj < 0){
                knapsack[i][j] = knapsack[ii][j];
            }
            else{
                if(knapsack[ii][j] < knapsack[ii][jj]+v){
                    knapsack[i][j] = knapsack[ii][jj]+v;
                }
                else{
                    knapsack[i][j] = knapsack[ii][j];
                }
            }  
        }
    }
    
    // Print2DMtx(knapsack, n, k);

    long long l = n-1;
    long long o = k;
    vector<double> sol (n, 0); 

    while (l >= 0)
    {
        double w = elements[0][l];

        if (l == 0 && knapsack[l][o] != 0) {
            sol[l] = 1;
        }else if(l != 0 && knapsack[l][o] != knapsack[l-1][o]){

            o = o - w;
            sol[l]= 1;
        }
        l--;
    }

    return sol;
}

int main(){
    parser();

    
    vector<double> result = ExactSolution(N, max_weight, value, weight);
    // for (int i = 0; i < result.size(); i++)
    // {
    //     cout << result[i] << " ";
    // }
    // cout << "\n";
    
    double total_value = 0;

    for (int i = 0; i < N; i++){
        if (result[i] == 1) {
            total_value += value[i];
        }
    }
    
    cout << total_value << endl;
    for (int i = 0; i < N; i++){
        if (result[i] == 1) {
            cout << value[i] << " " << weight[i] << endl;
        }
    }

    return 0;
}