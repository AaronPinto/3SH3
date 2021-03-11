#include <cmath>
#include <fstream>
#include <iostream>
#include <pthread.h>

#define N 4

const double NUM_PHASES = log2(N * N) + 1;
volatile int square[N][N];
volatile int phase_num = 1;
volatile int threads_done = 0;
pthread_mutex_t mtx;
pthread_cond_t cndvr;

// print array
void printArray() {
    for (auto row : square) {
        for (int j = 0; j < 4; j++) {
            std::cout << row[j] << " ";
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}

// swapping for bubble sort
void swap(volatile int *a, volatile int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// bubble sort
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
		sort(i, phase_num); // sorts the row/column

		pthread_mutex_lock(&mtx);
		if (++threads_done > 3) { // checks if the thread is the last thread completed in the phase
			phase_num++; // increment phase number
			threads_done = 0; // reset number of threads completed for next phase
			printArray(); // print array at end of phase
			pthread_cond_broadcast(&cndvr); // signal other threads to wake
		} else if (threads_done > 0) { // if not the last thread
			pthread_cond_wait(&cndvr, &mtx); // wait for pthread_cond_broadcast
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

    // Wait for all phases
    for (pthread_t thread : threads) {
        pthread_join(thread, nullptr);
    }

    // Cleanup
    pthread_mutex_destroy(&mtx);
    pthread_cond_destroy(&cndvr);
    return 0;
}
