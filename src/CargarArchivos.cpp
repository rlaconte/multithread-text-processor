#ifndef CHM_CPP
#define CHM_CPP

#include <vector>
#include <iostream>
#include <fstream>
#include <pthread.h>

#include "CargarArchivos.hpp"

int cargarArchivo(
    HashMapConcurrente &hashMap,
    std::string filePath
) {
    std::fstream file;
    int cant = 0;
    std::string palabraActual;

    // Abro el archivo.
    file.open(filePath, file.in);
    if (!file.is_open()) {
        std::cerr << "Error al abrir el archivo '" << filePath << "'" << std::endl;
        return -1;
    }
    while (file >> palabraActual) {
        // Completar (Ejercicio 4)
        hashMap.incrementar(palabraActual);
        cant++;
    }
    // Cierro el archivo.
    if (!file.eof()) {
        std::cerr << "Error al leer el archivo" << std::endl;
        file.close();
        return -1;
    }
    file.close();
    return cant;
}

struct thread_args {
	std::atomic<int> &n;
	std::atomic<bool> &marcas;
	HashMapConcurrente &hashMap;
    std::vector<std::string> &filePaths;
};


void *file_thread(void *arg){
    thread_args args_struct = *((thread_args *) arg);
	std::string archivo;
    unsigned int longitud = (&args_struct.filePaths)->size();
  	while (std::atomic_fetch_sub(&args_struct.n, 1) > 0) {
		for (unsigned int i = 0; i < longitud; i++){
			bool exp = false;
			if (std::atomic_compare_exchange_strong(&((&args_struct.marcas)[i]), &exp, true)){
				archivo = (*(&args_struct.filePaths))[i];
				break;
			}
		}	
        cargarArchivo(args_struct.hashMap, archivo);	
	}
    
 	return nullptr;
}

void cargarMultiplesArchivos(
    HashMapConcurrente &hashMap,
    unsigned int cantThreads,
    std::vector<std::string> filePaths
) {
    // Completar (Ejercicio 4)
    pthread_t tid[cantThreads];
    std::atomic<int> n(filePaths.size());
    std::atomic<bool> marcas[filePaths.size()];
    for (size_t i = 0; i < filePaths.size(); i++){
		std::atomic_init(&marcas[i], false);
	}

    thread_args args = {
		n, 
		marcas[0], 
		hashMap,
        filePaths
	};

    for (unsigned int i = 0; i < cantThreads; i++) {
		// args: ptr a donde almaceno tid, attr de cómo se crea thread,
		//       ptr a funcion de thread, args de esa funcion
		pthread_create(&tid[i], NULL, file_thread, &args);
	}

    for (unsigned int i = 0; i < cantThreads; i++){
		pthread_join(tid[i], NULL);
	}
}

#endif
