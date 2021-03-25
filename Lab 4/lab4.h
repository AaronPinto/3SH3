//
// Created by aaron on 2021-03-24.
//

#ifndef INC_3SH3_LAB4_H
#define INC_3SH3_LAB4_H

#include <cstdio>
#include <cstdlib>
#include <sys/sem.h>

#define handle_error(msg)                                                                                                                  \
    do {                                                                                                                                   \
        perror(msg);                                                                                                                       \
        exit(EXIT_FAILURE);                                                                                                                \
    } while (0)

// Used in calls to semctl()
union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
#if defined(__linux__)
    struct seminfo *_buf;
#endif
};

const int type_idx_mult = 4, val_idx_off = 2, num_types = 3;

const key_t key = ftok("lab4", 'a');

struct sembuf wait_and_inc[2] = {{0, 0, 0}, {0, 1, 0}};

struct sembuf dec = {0, -1, 0};

#endif //INC_3SH3_LAB4_H
