// implementacion de la solucion exacta de knapsack problem

// el problema consiste en colocar n elementos que consisten de p (profit) y w (peso) en una mochila
// de tamaño W (capacidad)


// vamos a crear una matriz para los elementos 


#include <iostream>

using namespace std;

int * ExactSolution(int n, int k){

    // 0 es el profit | 1 es el peso
    int elements[2][n] = {{1,6,18,22,28},{1,2,5,6,7}};
    int knapsack[n][k];

    // inicializamos la matriz en todo 0
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < k; j++)
        {
            knapsack[i][j] = 0;
        }
    }

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

    // PrintMtx(knapsack);
    for (int i = 0; i < n; i++)
    {
        
        for (int j = 0; j < k; j++)
        {
            cout << knapsack[i][j] << " ";
        }
        cout << "\n";
        
    }   

    return *knapsack;
}

int main(){
    int* pun = ExactSolution(5, 12);
    return 0;
}