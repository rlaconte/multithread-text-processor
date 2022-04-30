#include <iostream>
#include <fstream>

#include "HashMapConcurrente.hpp"
#include "CargarArchivos.hpp"
#include "time.h"

#define BILLION  1000000000L;

int main(int argc, char **argv) {
    std::string exp = "exp";
    if (argv[1] == exp){
        std::vector<std::string> filePaths = {};
        double tpo_carga[8];

        std::ofstream fsalida;
        fsalida.open("exp1.txt", fsalida.out);

        // Calculo de tiempos de carga

        for (int k = 0; k < 4; k++) {
            std::cout << "#archivos: " << k+1 << std::endl;
            filePaths.push_back("data/texto-"+std::to_string(k)+".txt");
        } 

        for (size_t i = 1; i <= 8; i++){
            std::cout << "#threads: " << i << std::endl;
            tpo_carga[i-1] = 0;
            for (size_t j = 0; j < 20; j++){
                std::cout << "iteracion: " << j << std::endl;
                HashMapConcurrente hashMap = HashMapConcurrente();
                struct timespec start_carga = {0,0};
                struct timespec stop_carga = {0,0};

                clock_gettime(CLOCK_REALTIME, &start_carga);
                cargarMultiplesArchivos(hashMap, i, filePaths);
                clock_gettime(CLOCK_REALTIME, &stop_carga);

                double aux = tpo_carga[i-1];
                tpo_carga[i-1] += ( stop_carga.tv_sec - start_carga.tv_sec )
                                + (double)( stop_carga.tv_nsec - start_carga.tv_nsec )
                                / (double)BILLION;
                fsalida << tpo_carga[i-1]-aux << std::endl;

            }
            tpo_carga[i-1] = tpo_carga[i-1]/20;
            std::cout << "tpo carga: " << tpo_carga[i-1] << std::endl;            
        }

        // Calculo de tiempos de maximoParalelo

        double tpo_max;
        std::vector<std::string> aux;
        aux.push_back("data/texto-9.txt");
        HashMapConcurrente hashMap = HashMapConcurrente();
        cargarMultiplesArchivos(hashMap, 1, aux);

        for (size_t i = 1; i <= 26; i++){
            std::cout << "#threads: " << i << std::endl;

            tpo_max = 0;
            for (size_t j = 0; j < 10; j++){
                std::cout << "iteracion: " << j << std::endl;
                struct timespec start_carga = {0,0};
                struct timespec stop_carga = {0,0};

                clock_gettime(CLOCK_REALTIME, &start_carga);
                //hashMap.maximoParalelo(i);
                clock_gettime(CLOCK_REALTIME, &stop_carga);
                tpo_max += ( stop_carga.tv_sec - start_carga.tv_sec )
                                + (double)( stop_carga.tv_nsec - start_carga.tv_nsec )
                                / (double)BILLION;

                std::cout << "tpo max: " << tpo_max << std::endl;            
            }
            tpo_max = tpo_max/10;
            fsalida << tpo_max << std::endl;
        }
    } else {
        if (argc < 4) {
            std::cout << "Error: faltan argumentos." << std::endl;
            std::cout << std::endl;
            std::cout << "Modo de uso: " << argv[0] << " <threads_lectura> <threads_maximo>" << std::endl;
            std::cout << "    " << "<archivo1> [<archivo2>...]" << std::endl;
            std::cout << std::endl;
            std::cout << "    threads_lectura: "
                << "Cantidad de threads a usar para leer archivos." << std::endl;
            std::cout << "    threads_maximo: "
                << "Cantidad de threads a usar para computar máximo." << std::endl;
            std::cout << "    archivo1, archivo2...: "
                << "Archivos a procesar." << std::endl;
            return 1;
        }
        int cantThreadsLectura = std::stoi(argv[1]);
        int cantThreadsMaximo = std::stoi(argv[2]);

        std::vector<std::string> filePaths = {};
        for (int i = 3; i < argc; i++) {
            filePaths.push_back(argv[i]);
        }

        HashMapConcurrente hashMap = HashMapConcurrente();
        cargarMultiplesArchivos(hashMap, cantThreadsLectura, filePaths);
        auto maximo = hashMap.maximoParalelo(cantThreadsMaximo);

        std::cout << maximo.first << " " << maximo.second << std::endl;
    }
    return 0;
}
