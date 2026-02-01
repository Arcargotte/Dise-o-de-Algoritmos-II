

generator: generator.cpp conmons.h
	g++ -g -std=c++11 -O2 generator.cpp -o generatorExecutable && ./generatorExecutable > example.txt 
	 
knapsack: parser.o LS_Solution.o ILS_Solution.o Heuristics_Solution.o generator.o Exact_Solutions.o
	g++ -o knapsack parser.o LS_Solution.o ILS_Solution.o Heuristics_Solution.o generator.o Exact_Solutions.o

parser.o: parser.cpp conmons.h
	g++ -c parser.cpp

LS_Solution.o: LS_Solution.cpp 
	g++ -c LS_Solution.cpp

ILS_Solution.o: ILS_Solution.cpp 
	g++ -c ILS_Solution.cpp

Heuristics_Solution.o: Heuristics_Solution.cpp 
	g++ -c Heuristics_Solution.cpp

generator.o: generator.cpp 
	g++ -c generator.cpp

Exact_Solutions.o: Exact_Solutions.cpp
	g++ -c Exact_Solutions.cpp