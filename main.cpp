#include <iostream>
#include <fstream>
#include <math.h>
#include <queue>
#include <string>
#include <unordered_set>
#include <pthread.h>

#include "include/my_pthread_utils.hpp"

using namespace std;

/**
 * Check for a number n if it is a perfect power and adds it
 * to the according partial list.
*/
void perfect_powers(long long n, int nr_reducers, int mapper_id,
                     unordered_set<int> **partial_lists) {
    
    /* only n > 0 is checked */
    if (n <= 0) {
        return;
    }

    /* 1 is perfect power for every exponent */
    if (n == 1) {
        for (int i = 2; i <= nr_reducers + 1; i++) {
            partial_lists[mapper_id][i - 2].insert(1);
        }

        return;
    }

    /* check every possible exponent using binary search algorithm */
    for (int i = 2; i <= nr_reducers + 1; i++) {
        int start = 1;
        int end = sqrt(n) + 1;

        while (start <= end) {
            int mid = (start + end) / 2;
            
            long long power = pow(mid, i);

            if (power == n) {
                partial_lists[mapper_id][i - 2].insert(n);
                break;
            } else if (power < n) {
                start = mid + 1;
            } else {
                end = mid - 1;
            }
        }
    }
}

/**
 * Structure used to parse all required arguments for a mapper.
*/
typedef struct {
    int id;
    int nr_reducers;
    queue<string> *files_q;
    unordered_set<int> **partial_lists;

    pthread_barrier_t *barrier;
    pthread_mutex_t *mutex;

} mapper_struct;

/**
 * Structure used to parse all required arguments for a reducer.
*/
typedef struct {
    int id;
    int nr_mappers;
    unordered_set<int> **partial_lists;

    pthread_barrier_t *barrier;

} reducer_struct;

/**
 * Map function. Accesses a file and add every number to the according
 * partial list.
*/
void *mapper_thread(void *arg) {

    pthread_barrier_t *barrier = ((mapper_struct*)arg)->barrier;
    pthread_mutex_t *mutex = ((mapper_struct*)arg)->mutex;

    queue<string> *files_q = ((mapper_struct*)arg)->files_q;
    unordered_set<int> **partial_lists = ((mapper_struct*)arg)->partial_lists;
    int mapper_id = ((mapper_struct*)arg)->id;
    int nr_reducers = ((mapper_struct*)arg)->nr_reducers;

    string file;

    while (1) {
        mutex_lock(mutex);

        /* If the files queue is empty, it means all files have been processed,
           or are being processed by other mappers, so this mapper is done. */
        if ((*files_q).empty()) {
            mutex_unlock(mutex);
            break;
        }
        file = (*files_q).front();
        (*files_q).pop();

        mutex_unlock(mutex);

        /* The file is processed - every number is added in the according
           partial lists, if there is the case. */
        ifstream fin(file);
        int n, ans;
        fin >> n;
        for (int i = 0; i < n; i++) {
            fin >> ans;
            perfect_powers(ans, nr_reducers, mapper_id, partial_lists);
        }

        fin.close();
    }
    
    pthread_barrier_wait(barrier);

    pthread_exit(NULL);
}

/**
 * Reduce function. Takes an exponent and aggregates the lists from
 * every mapper for that exponent. The final list size is then printed
 * on the according output file.
*/
void *reducer_thread(void *arg) {

    pthread_barrier_t *barrier = ((reducer_struct*)arg)->barrier;
    unordered_set<int> **partial_lists = ((reducer_struct*)arg)->partial_lists;
    int reducer_id = ((reducer_struct*)arg)->id;
    int nr_mappers = ((reducer_struct*)arg)->nr_mappers;

    pthread_barrier_wait(barrier);

    unordered_set<int> aggregate_list;
    for (int i = 0; i < nr_mappers; i++) {
        for (auto it = partial_lists[i][reducer_id].begin();
             it != partial_lists[i][reducer_id].end(); ++it)
        {
            aggregate_list.insert(*it);
        }
    }

    ofstream fout("out" + to_string(reducer_id + 2) + ".txt");
    fout << aggregate_list.size();
    fout.close();

    pthread_exit(NULL);
}

/**
 * The input file is read and every file is added in a files queue,
 * which will be used for an efficient load balance among mappers.
*/
void initialize_files_queue(string input_file_name, queue<string> &files_q) {
    int nr_files;
    ifstream fin(input_file_name);

    fin >> nr_files;
    for (int i = 0; i < nr_files; i++) {
        string name;
        fin >> name;
        files_q.push(name);
    }

    fin.close();
}

/**
 * Function used to free dynamically allocated memory.
*/
void free_memory(int nr_mappers, mapper_struct *mapper_arguments,
                 reducer_struct *reducer_arguments,
                 unordered_set<int> **partial_lists)
{
    free(mapper_arguments);
    free(reducer_arguments);
    for (int i = 0; i < nr_mappers; ++i)
        delete [] partial_lists[i];
    delete [] partial_lists;
}

int main(int argc, char *argv[])
{
    int nr_mappers          = atoi(argv[1]);
    int nr_reducers         = atoi(argv[2]);
    string input_file_name  = argv[3];

    queue<string> files_q;
    initialize_files_queue(input_file_name, files_q);

    pthread_t mappers[nr_mappers], reducers[nr_reducers];
    mapper_struct *mapper_arguments =
            (mapper_struct*)malloc(sizeof(mapper_struct) * nr_mappers);
    reducer_struct *reducer_arguments =
            (reducer_struct*)malloc(sizeof(reducer_struct) * nr_reducers);

    /* a barrier will be used to ensure the syncronization
       of the mapper and reducer threads */
    pthread_barrier_t barrier;
    barrier_init(&barrier, nr_mappers + nr_reducers);

    /* a mutex will be used to ensure that no more than one thread 
       accesses and modifies the files queue at one time */
    pthread_mutex_t mutex;
    mutex_init(&mutex);

    unordered_set<int> **partial_lists = new unordered_set<int>*[nr_mappers];
    for (int i = 0; i < nr_mappers; i++)
        partial_lists[i] = new unordered_set<int>[nr_reducers];


    for (int i = 0; i < nr_mappers; i++) {
        mapper_arguments[i].barrier = &barrier;
        mapper_arguments[i].mutex = &mutex;
        mapper_arguments[i].files_q = &files_q;
        mapper_arguments[i].id = i;
        mapper_arguments[i].nr_reducers = nr_reducers;
        mapper_arguments[i].partial_lists = partial_lists;

        thread_create(&mappers[i], mapper_thread,
                      &mapper_arguments[i], i);
    }

    for (int i = 0; i < nr_reducers; i++) {
        reducer_arguments[i].barrier = &barrier;
        reducer_arguments[i].id = i;
        reducer_arguments[i].nr_mappers = nr_mappers;
        reducer_arguments[i].partial_lists = partial_lists;

        thread_create(&reducers[i], reducer_thread,
                      &reducer_arguments[i], nr_mappers + i);
    }

    for (int i = 0; i < nr_mappers; i++) {
        thread_join(&mappers[i], i);
	}

    for (int i = 0; i < nr_reducers; i++) {
		thread_join(&reducers[i], nr_mappers + i);
	}


    barrier_destroy(&barrier);
    mutex_destroy(&mutex);

    free_memory(nr_mappers, mapper_arguments,
                reducer_arguments, partial_lists);

	return 0;
}
