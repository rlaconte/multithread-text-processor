#ifndef CHM_CPP
#define CHM_CPP

#include <iostream>
#include <fstream>
#include <pthread.h>
#include <pthread.h>   // POSIX threads
#include <semaphore.h> // POSIX semaphores

#include "HashMapConcurrente.hpp"

pthread_mutex_t sem_lista[26]; //creamos un mutex para cada letra

HashMapConcurrente::HashMapConcurrente() {
	for (unsigned int i = 0; i < HashMapConcurrente::cantLetras; i++) {
		tabla[i] = new ListaAtomica<hashMapPair>();
	}
	for (int i = 0; i < 26; ++i){
		pthread_mutex_t mutex;
		pthread_mutex_init(&mutex, NULL);
		sem_lista[i] = mutex;
	}
}

unsigned int HashMapConcurrente::hashIndex(std::string clave) {
	return (unsigned int)(clave[0] - 'a');
}

void HashMapConcurrente::incrementar(std::string clave) {
	// Completar (Ejercicio 2)
	// Tomo el mutex
	pthread_mutex_lock(&sem_lista[hashIndex(clave)]);

	//buscamos la clave en el HashMap
	bool encontre = false;
	ListaAtomica<hashMapPair>::Iterador it = tabla[hashIndex(clave)]->crearIt();
	while (it.haySiguiente()){
		hashMapPair& actual = it.siguiente();
		if(actual.first == clave){
			encontre = true;
			actual.second = actual.second + 1;
			break;
		}
		it.avanzar();
	}
	if(!encontre){
		hashMapPair nueva = std::make_pair(clave, 1);
		tabla[hashIndex(clave)]->insertar(nueva);
	}
	// Libero el mutex
	pthread_mutex_unlock(&sem_lista[hashIndex(clave)]);
}

std::vector<std::string> HashMapConcurrente::claves() {
	// Completar (Ejercicio 2)
	// no bloqueante y libre de espera
	std::vector<std::string> res;
	for (size_t i = 0; i < HashMapConcurrente::cantLetras; i++){
		ListaAtomica<hashMapPair>::Iterador it = tabla[i]->crearIt();
		while (it.haySiguiente()){
			res.push_back(it.siguiente().first);
			it.avanzar();
		}
	}
	return res;
}

unsigned int HashMapConcurrente::valor(std::string clave) {
	// Completar (Ejercicio 2)
	unsigned int res = 0;
	ListaAtomica<hashMapPair>::Iterador it = tabla[hashIndex(clave)]->crearIt();
	while (it.haySiguiente()){
		hashMapPair& actual = it.siguiente();
		if(actual.first == clave){
			// Para q sea concurrente con incrementar necesitariamos q el int de la
			// HashMapPair sea atomico, o algo asi
			res = actual.second;
			break;
		}
		it.avanzar();
	}
	return res;
}

hashMapPair HashMapConcurrente::maximo() {
	hashMapPair *max = new hashMapPair();
	max->second = 0;
	// maximo() y incrementar() no son mas concurrentes
	// Bloqueamos las listas
	for (size_t i = 0; i < HashMapConcurrente::cantLetras; i++){
		pthread_mutex_lock(&sem_lista[i]);
	}
	for (unsigned int index = 0; index < HashMapConcurrente::cantLetras; index++) {
		for (auto it = tabla[index]->crearIt(); it.haySiguiente(); it.avanzar()) {
			if (it.siguiente().second > max->second) {
				max->first = it.siguiente().first;
				max->second = it.siguiente().second;
			}
		}
	}
	// Desbloqueamos las listas
	for (size_t i = 0; i < HashMapConcurrente::cantLetras; i++){
		pthread_mutex_unlock(&sem_lista[i]);
	}
	return *max;
}

struct thread_args {
	std::atomic<int> &n;
	std::atomic<bool> &marcas;
	hashMapPair &maximos;
	HashMapConcurrente* hash;
};

// Función que ejecuta cada thread
void *HashMapConcurrente::max_thread(void *arg) {
	// Casteo el puntero a void y me guardo una copia de los parámetros
  	thread_args args_struct = *((thread_args *) arg);
	unsigned int fila;
  	while (std::atomic_fetch_sub(&args_struct.n, 1) > 0) {
		for (size_t i = 0; i < HashMapConcurrente::cantLetras; i++){
			bool exp = false;
			if (std::atomic_compare_exchange_strong(&((&args_struct.marcas)[i]), &exp, true)){
				fila = i;
				break;
			}
		}
		// buscar maximo en la i-esima lista
		hashMapPair *max = new hashMapPair();
		max->second = 0;
		// pasamos this por los parametros del thread
		for (auto it = (args_struct.hash)->tabla[fila]->crearIt(); it.haySiguiente(); it.avanzar()) {
			if (it.siguiente().second > max->second) {
				max->first = it.siguiente().first;
				max->second = it.siguiente().second;
			}
		}
		(&args_struct.maximos)[fila] = *max;
	}
 	return nullptr;
}

hashMapPair HashMapConcurrente::maximoParalelo(unsigned int cantThreads) {
	// Completar (Ejercicio 3)
	// Creamos un array para guardar los TIDs.
	pthread_t tid[cantThreads];

	// Un n para chequear si los threads tienen q seguir trabajando
	// array de bools para chequear q listas quedan por recorrer (false es q no se recorrio)
	std::atomic<int> n(HashMapConcurrente::cantLetras);
	std::atomic<bool> marcas[HashMapConcurrente::cantLetras];
	for (size_t i = 0; i < HashMapConcurrente::cantLetras; i++){
		std::atomic_init(&marcas[i], false);
	}
	hashMapPair maximos[HashMapConcurrente::cantLetras];

	thread_args args = {
		n, 
		marcas[0], 
		maximos[0],
		this
	};

	// Bloqueamos las listas
	for (size_t i = 0; i < HashMapConcurrente::cantLetras; i++){
		pthread_mutex_lock(&sem_lista[i]);
	}

	// Creamos los 'cantThreads' threads.
	for (unsigned int i = 0; i < cantThreads; i++) {
		// args: ptr a donde almaceno tid, attr de cómo se crea thread,
		//       ptr a funcion de thread, args de esa funcion
		pthread_create(&tid[i], NULL, HashMapConcurrente::max_thread, &args);
	}

	for (unsigned int i = 0; i < cantThreads; i++){
		pthread_join(tid[i], NULL);
	}

	hashMapPair *res = new hashMapPair();
	res->second = 0;
	for (unsigned int i = 0; i < HashMapConcurrente::cantLetras; i++){
		if(maximos[i].second > res->second){
			res->first = maximos[i].first;
			res->second = maximos[i].second;
		}
	}

	// Desbloqueamos las listas
	for (size_t i = 0; i < HashMapConcurrente::cantLetras; i++){
		pthread_mutex_unlock(&sem_lista[i]);
	}
	return *res;
}

#endif
