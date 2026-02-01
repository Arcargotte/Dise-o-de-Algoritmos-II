#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <conmons.h>

using namespace std;

void parser() 
{
    int num_items;

    ifstream inputFile("example.txt"); // Open the file named "example.txt"
    string word;

    if (!inputFile.is_open()) {
        cerr << "Error: Could not open the file." << endl;
    }
    
    if (inputFile >> word){
        num_items = stoi(word);
    }

    int banned = 0;
    int i= 0;
    cout << "Numero de objetos: "<< num_items << endl;
    while (i < num_items * 3 && inputFile >> word) {
        if (banned == 1){
            value.push_back(stod(word));
        } else if (banned == 2){
            weight.push_back(stod(word));
        }

        banned = (banned + 1) % 3;

        i++;
    }
    
    if (inputFile >> word){
        max_weight = stod(word);
    }
    
    inputFile.close(); // Close the file stream
    
    for (int i = 0; i < num_items; i++){
        cout << value[i] << " " << weight[i] << endl;
    }
    cout << "max_weight: " << max_weight << endl;
}