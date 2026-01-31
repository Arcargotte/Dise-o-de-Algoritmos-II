// implementacion de la solucion exacta de knapsack problem

// el problema consiste en colocar n elementos que consisten de p (profit) y w (peso) en una mochila
// de tamaño W (capacidad)


// vamos a crear una matriz para los elementos 


#include <iostream>
#include <vector>

using namespace std;

void Print2DMtx(vector<vector<int>> m, int f, int c){
    for (int i = 0; i < f; i++)
    {
        for (int j = 0; j < c; j++)
        {
            cout << m[i][j] << '\t';
        }
        cout << "\n";
    }
    cout << "\n";
}

vector<int> ExactSolution(int n, int k, vector<int> profit, vector<int> weight){

    // 0 es el peso | 1 es el profit
    vector<vector<int>> elements = {weight, profit};
    vector<vector<int>> knapsack (n, vector<int>(k,0));


    // aplicamos el algoritmo exacto
    for (int i = 0; i < n; i++)
    {
        int v = elements[0][i];
        int w = elements[1][i];

        for (int j = 0; j < k; j++)
        {
            int jj = j - w;
            int ii = i - 1;
            if (jj < 0){
                jj = 0;
            }
            if (ii < 0){
                ii = 0;
            }
            
            if (i == 0){
                if (w <= j){
                    knapsack[i][j] = knapsack[i][j] + v;
                }
            }
            else if(j-w < 0){
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

    int l = n-1;
    int o = k-1;
    vector<int> sol (n, 0); 

    while (l != 0)
    {
        int v = elements[0][l];
        int w = elements[1][l];

        if(knapsack[l][o] != knapsack[l-1][o]){
            o = o - w;
            sol[l]= 1;
            l--;
        }
        else{
            l--;
        }
    }
    return sol;
}

int main(){
    int n = 5;
    int k = 12;
    vector<int> p = {1,2,5,6,7};
    vector<int> w = {1, 6, 18, 22, 28};

    
    vector<int> result = ExactSolution(n, k, p, w);
    for (int i = 0; i < result.size(); i++)
    {
        cout << result[i] << " ";
    }
    cout << "\n";

    return 0;
}