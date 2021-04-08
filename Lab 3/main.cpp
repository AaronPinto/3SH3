//
// Created by aaron on 2021-03-09.
//
#include <cmath>
#include <fstream>
#include <iostream>
#include <pthread.h>

#define N 4

const double NUM_PHASES = log2(N * N) + 1;
volatile int square[N][N];
pthread_mutex_t mtx;
pthread_barrier_t phase_bar;

// Print array
void print_array() {
    for (auto row : square) {
        for (int j = 0; j < N; j++) {
            std::cout << row[j] << " ";
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}

// Swap 2 elements
void swap(volatile int *a, volatile int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// index is the row or col for the thread to work on
void *thread_work(void *index) {
    int i = *((int *) index);

    for (int j = 1; j <= NUM_PHASES; j++) {
        pthread_mutex_lock(&mtx);

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

//        std::cout << i << " " << j << "\n" << std::endl;

        pthread_mutex_unlock(&mtx);

        // 5. Wait for threads to finish phase
        int ret = pthread_barrier_wait(&phase_bar);

        // 6. Print the array and go to next phase
        if (ret == PTHREAD_BARRIER_SERIAL_THREAD) {
            std::cout << "After Phase " << j << std::endl;
            print_array();
        }

        pthread_barrier_wait(&phase_bar); // Wait for print
    }

    return nullptr;
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
    std::cout << "Initial array" << std::endl;
    print_array();

    // 3. Init condition variables
    pthread_mutex_init(&mtx, nullptr);
    pthread_barrier_init(&phase_bar, nullptr, N);

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
    pthread_barrier_destroy(&phase_bar);
    return 0;
}