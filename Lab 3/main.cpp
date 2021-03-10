// #include <condition_variable>
// #include <fstream>
// #include <iostream>
// #include <mutex>
// #include <pthread.h>

// #define NUM_THREADS 4
// #define N 4
// #define NUM_PHASES 5

// int square[N][N];
// pthread_mutex_t mutex_var;
// pthread_cond_t cond_var;
// int count = 0;
// int phase_num = 0;
// // std::mutex mtx;
// // std::condition_variable con_var;

// void *work(void *t) {
// 	int i = (int)t;

// 	for (auto j = 0; j < NUM_PHASES; j++) {
// 		pthread_mutex_lock(&mutex_var);
// 		if (count < 4){
// 			std::cout << phase_num << " " << count << "\n";
// 			count++;
// 			pthread_cond_wait(&cond_var, &mutex_var);
// 		} else {
// 			count = 0;
// 			pthread_cond_broadcast(&cond_var);
// 		}
// 		pthread_mutex_unlock(&mutex_var);
// 	}

// 	return NULL;
// }

// int main() {
// 	// 1. read array from file
// 	std::ifstream infile("input.txt");

// 	int i = 0, a, b, c, d;
// 	while (infile >> a >> b >> c >> d) {
// 		square[i][0] = a;
// 		square[i][1] = b;
// 		square[i][2] = c;
// 		square[i][3] = d;
// 		i++;
// 	}

// 	// 2. Print integers
// 	// for (auto row : square) {
// 	// 	for (int j = 0; j < 4; j++) {
// 	// 		std::cout << row[j] << " ";
// 	// 	}
// 	// 	std::cout << "\n";
// 	// }

// 	// 3. Init condition variables
// 	pthread_attr_t attr;

// 	/* Initialize mutex and condition variable objects */
// 	pthread_mutex_init(&mutex_var, NULL);
// 	pthread_cond_init (&cond_var, NULL);

// 	pthread_t threads[NUM_THREADS];


// 	// 4. Create threads
	
// 	int rc;
// 	for (auto t = 0; t < NUM_THREADS; t++) {
// 		rc = pthread_create(&threads[t], NULL, *work, (void *)t);
// 	}
	

// 	// 2. Print integers
// 	// std::cout << "\n";
// 	// for (auto row : square) {
// 	// 	for (int j = 0; j < 4; j++) {
// 	// 		std::cout << row[j] << " ";
// 	// 	}
// 	// 	std::cout << "\n";
// 	// }

// 	for (auto row : square) {
//         for (int j = 0; j < 4; j++) {
//             std::cout << row[j] << " ";
//         }
//         std::cout << "\n";
//     }
//     std::cout << "\n";

// 	return 0;
// }

//
// Created by aaron on 2021-03-09.
//
#include <cmath>
#include <fstream>
#include <iostream>
#include <pthread.h>
#include <unistd.h>

#define N 4

const double NUM_PHASES = log2(N * N) + 1;
volatile int square[N][N];
volatile int phase_num = 1;
volatile int threads_done = 0;
pthread_mutex_t mtx;
pthread_cond_t cndvr;
// pthread_barrier_t phase_bar;

// PRINT ARRAY
void printArray() {
    for (auto row : square) {
        for (int j = 0; j < 4; j++) {
            std::cout << row[j] << " ";
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}

void swap(volatile int *a, volatile int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

void sort(int i, int j) {
	if (j % 2 != 0) { // Row sort
		if (i % 2 == 0) { // Increasing order
			bool swapped;
			for (int k = 0; k < N - 1; k++) {
				swapped = false;

				for (int l = 0; l < N - k - 1; l++) {
					if (square[i][l] > square[i][l + 1]) {
						swap(&square[i][l], &square[i][l + 1]);
						swapped = true;
					}
				}

				// If no two elements were swapped by inner loop, then break
				if (!swapped)
					break;
			}
		} else { // Decreasing order
			bool swapped;
			for (int k = 0; k < N - 1; k++) {
				swapped = false;

				for (int l = 0; l < N - k - 1; l++) {
					if (square[i][l] < square[i][l + 1]) {
						swap(&square[i][l], &square[i][l + 1]);
						swapped = true;
					}
				}

				// If no two elements were swapped by inner loop, then break
				if (!swapped)
					break;
			}
		}
	} else { // Col sort
		bool swapped;
		for (int k = 0; k < N - 1; k++) {
			swapped = false;

			for (int l = 0; l < N - k - 1; l++) {
				if (square[l][i] > square[l + 1][i]) {
					swap(&square[l][i], &square[l + 1][i]);
					swapped = true;
				}
			}

				// If no two elements were swapped by inner loop, then break
			if (!swapped)
				break;
		}
	}
}

// index is the row or col for the thread to work on
void *thread_work(void *index) {
	int i = *((int *) index);

	while (phase_num <= NUM_PHASES) {		
		sort(i, phase_num);

		pthread_mutex_lock(&mtx);
		if (++threads_done > 3) {
			phase_num++;
			threads_done = 0;
			printArray();
			pthread_cond_broadcast(&cndvr);
		} else if (threads_done > 0) {
			pthread_cond_wait(&cndvr, &mtx);	
		} pthread_mutex_unlock(&mtx);
	}

	return NULL;
}

int main() {
    // 1. read array from file
    std::ifstream infile("input.txt");

    int line = 0, a, b, c, d;
    while (infile >> a >> b >> c >> d) {
        square[line][0] = a;
        square[line][1] = b;
        square[line][2] = c;
        square[line][3] = d;
        line++;
    }

    // 2. Print integers
	printArray();


    // 3. Init condition variables
    pthread_mutex_init(&mtx, nullptr);
    pthread_cond_init(&cndvr, nullptr);
    // pthread_barrier_init(&phase_bar, nullptr, N);

    // 4. Create threads
    pthread_t threads[N];
    for (int j = 0; j < N; j++) {
        int *arg = static_cast<int *>(malloc(sizeof(*arg)));

        if (arg == nullptr) {
            std::cerr << "Couldn't allocate memory for thread arg.\n";
            exit(EXIT_FAILURE);
        }

        *arg = j;
        pthread_create(&threads[j], nullptr, thread_work, arg);
    }
    // pthread_cond_signal(&cndvr);

    // Wait for all phases
    for (pthread_t thread : threads) {
        pthread_join(thread, nullptr);
    }

    // Print final array
    // printArray();

    // Cleanup
    pthread_mutex_destroy(&mtx);
    pthread_cond_destroy(&cndvr);
    // pthread_barrier_destroy(&phase_bar);
    return 0;
}