#ifndef _MY_PTHREAD_UTILS_HPP_
#define _MY_PTHREAD_UTILS_HPP_

void barrier_init(pthread_barrier_t *barrier, int nr_threads);

void barrier_destroy(pthread_barrier_t *barrier);

void mutex_init(pthread_mutex_t *mutex);

void mutex_destroy(pthread_mutex_t *mutex);

void mutex_lock(pthread_mutex_t *mutex);

void mutex_unlock(pthread_mutex_t *mutex);

void thread_create(pthread_t *thread,
				   void *(*thread_function) (void *),
				   void *args, int id);

void thread_join(pthread_t *thread, int id);

#endif /* _MY_PTHREAD_UTILS_HPP_ */
