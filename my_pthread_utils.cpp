#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "include/my_pthread_utils.hpp"

void barrier_init(pthread_barrier_t *barrier, int nr_threads) {
    int r = pthread_barrier_init(barrier, NULL, nr_threads);
	if (r) {
		printf("Eroare la crearea barierei\n");
		exit(-1);
	}
}

void barrier_destroy(pthread_barrier_t *barrier) {
    int r = pthread_barrier_destroy(barrier);
	if (r) {
		printf("Eroare la distrugerea barierei\n");
		exit(-1);
	}
}

void mutex_init(pthread_mutex_t *mutex) {
    int r = pthread_mutex_init(mutex, NULL);
	if (r) {
		printf("Eroare la crearea mutex-ului\n");
		exit(-1);
	}
}

void mutex_destroy(pthread_mutex_t *mutex) {
    int r = pthread_mutex_destroy(mutex);
	if (r) {
		printf("Eroare la distrugerea mutex-ului\n");
		exit(-1);
	}
}

void mutex_lock(pthread_mutex_t *mutex) {
    int r = pthread_mutex_lock(mutex);
    if (r) {
        printf("Eroare la blocarea mutex-ului\n");
        exit(-1);
    }
}

void mutex_unlock(pthread_mutex_t *mutex) {
    int r = pthread_mutex_unlock(mutex);
    if (r) {
        printf("Eroare la deblocarea mutex-ului\n");
        exit(-1);
    }
}

void thread_create(pthread_t *thread,
				   void *(*thread_function) (void *),
				   void *args, int id) {
	int r = pthread_create(thread, NULL, thread_function, args);
	if (r) {
		printf("Eroare la crearea thread-ului %d\n", id);
		exit(-1);
	}
}

void thread_join(pthread_t *thread, int id) {
	int r = pthread_join(*thread, NULL);
	if (r) {
		printf("Eroare la asteptarea thread-ului %d\n", id);
		exit(-1);
	}
}
