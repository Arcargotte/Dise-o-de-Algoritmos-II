#include <iostream>
#include <filesystem>
#include <string>
#include <chrono>
#include <unistd.h>    // Para fork() y _exit()
#include <sys/wait.h>  // Para waitpid()

#include "conmons.h"
// #include "memetic/memetic.cpp"
// #include "scatter_search/scatter_search.cpp"

using namespace std;

namespace fs = filesystem;

// extern void aco();

double ejecutar_con_timeout(int tipo, const string& path, int segundos_limite, double& resultado_heuristica) {
    auto t_inicio = chrono::steady_clock::now();
    
    pid_t pid = fork();

    if (pid < 0) {
        perror("Error en fork");
        return -1;
    }
    string programa;

    if (pid == 0) {
        // --- PROCESO HIJO ---
        if (tipo == 1) {
            // resultado_heuristica = memetic();
            // resultado_heuristica = 1;
            programa = "./memetic/output/memetic";
        }
            
        else if (tipo == 2) {
            // resultado_heuristica = scatter_search();
            // resultado_heuristica = 2;
            programa = "./scatter_search/output/scatter_search";
        }
        else if (tipo == 3) {
            // resultado_heuristica = aco();
            programa = "./aco/output/aco";
        }

        execl(programa.c_str(), programa.c_str(), path.c_str(), nullptr);

        // Si execl falla (ej. no encuentra el archivo)
        perror("Error en execl");
        _exit(1);
    } else {
        // --- PROCESO PADRE (Vigilante) ---
        int status;
        bool terminado = false;

        while (true) {
            // WNOHANG hace que waitpid no se bloquee. 
            // Retorna el PID si el hijo terminó, o 0 si sigue ejecutando.
            pid_t resultado = waitpid(pid, &status, WNOHANG);

            if (resultado == pid) {
                ifstream in("./temp_res.txt");
                in >> resultado_heuristica;
                terminado = true;
                break; 
            }

            // Comprobar si excedió el tiempo límite
            auto t_actual = chrono::steady_clock::now();
            auto segundos_transcurridos = chrono::duration_cast<chrono::seconds>(t_actual - t_inicio).count();

            if (segundos_transcurridos >= segundos_limite) {
                cout << "   [!] Timeout alcanzado (" << segundos_limite << "s). Matando proceso " << pid << "..." << endl;
                
                kill(pid, SIGKILL); // Enviar señal de terminación forzosa
                waitpid(pid, &status, 0); // Limpiar el proceso para que no quede como zombie
                return -1.0; // Retornamos un valor negativo para indicar fallo por tiempo
            }

            // Pequeño respiro de 50ms para no quemar un núcleo de CPU solo vigilando
            usleep(50000); 
        }

        auto t_fin = chrono::steady_clock::now();
        chrono::duration<double> total = t_fin - t_inicio;
        return total.count();
    }
}

void ejecutar_benchmark(string ruta_carpeta, int timeout) {
    for (const auto& entrada : fs::directory_iterator(ruta_carpeta)) {
        if (!entrada.is_regular_file()) continue;

        string archivo_path = entrada.path().string();
        cout << "\nInstancia: " << entrada.path().filename().string() << endl;

        for (int h = 1; h <= 3; ++h) {
            double resultado_heuristica;
            double tiempo = ejecutar_con_timeout(h, archivo_path, timeout, resultado_heuristica);
            
            if (tiempo >= 0) {
                cout << "   [Heuristica " << h << "] Terminada en: " << tiempo << "s" << endl;
                cout << "   [Resultado  "  << h << "] " << resultado_heuristica << endl;
            } else {
                cout << "   [Heuristica" << h << "] FALLIDA por Timeout" << endl;
            }
        }
    }
}

int main(){
    // auto inicio = chrono::high_resolution_clock::now();
    // iterar_archivos("./benchmark");
    // auto fin = chrono::high_resolution_clock::now();

    // chrono::duration<double> tiempo = fin - inicio;
    // cout << "Tardó: " << tiempo.count() << " segundos" << endl;

    string carpeta = "./benchmark"; 
    int tiempo_maximo = 130;
    
    ejecutar_benchmark(carpeta, tiempo_maximo);


    return 0;
}