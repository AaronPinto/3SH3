//
// Created by aaron on 2021-03-24.
//

#include "lab4.h"
#include <fcntl.h>
#include <iostream>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

int main() {
    unsigned char *addr;
    int fd, semid;
    union semun arg {};
    union semun dummy {};
    struct stat sb {};

    // 0. Init sem to a value of 0
    semid = semget(key, 1, IPC_CREAT | 0600);
    if (semid == -1) {
        handle_error("semget");
    }

    arg.val = 0;
    if (semctl(semid, 0, SETVAL, arg) == -1) {
        handle_error("semctl");
    }

    // 1. Open file
    fd = open("res.txt", O_RDWR);

    if (fd == -1) {
        handle_error("open");
    }

    // 2. Obtain file size
    if (fstat(fd, &sb) == -1) {
        handle_error("fstat");
    }

    // 3. Memory map it
    addr = static_cast<unsigned char *>(mmap(nullptr, sb.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0));

    if (addr == MAP_FAILED) {
        handle_error("mmap");
    }

    // 4. Fork child
    pid_t child_pid;
    if ((child_pid = fork()) == -1) {
        handle_error("fork");
    }

    if (child_pid == 0) { // Child
        ssize_t s;
        unsigned char mincr[1];

        while (true) {
            std::cout << "System page size: " << getpagesize() << std::endl;
            std::cout << "Current resources: " << std::endl;

            // Wait for sem to be 0 then inc by 1
            if (semop(semid, wait_and_inc, 2) == -1) {
                handle_error("semop");
            }

            s = write(STDOUT_FILENO, addr, sb.st_size);

            // Decrement by 1, back to 0
            if (semop(semid, &dec, 1) == -1) {
                handle_error("semop");
            }

            std::cout << std::endl;

            if (s != sb.st_size) {
                if (s == -1) {
                    handle_error("write");
                }
                fprintf(stderr, "partial write");
                exit(EXIT_FAILURE);
            }

            if (mincore(addr, sb.st_size, mincr) == -1) {
                handle_error("mincore");
            }

            std::cout << "Page in RAM: " << (mincr[0] & 1) << std::endl;

            sleep(10);
        }
    } else { // Parent
        // 5. Loop asking if resources added
        bool want_to_add = false;
        int resource_type = 0;
        unsigned char resources_needed;

        do {
            std::cout << "Do you want to add resources? (1 for Yes / 0 for No)" << std::endl;
            std::cin >> want_to_add;

            if (want_to_add) {
                std::cout << "Enter the type and quantity separated by a space:" << std::endl;
                std::cin >> resource_type >> resources_needed;

                // Wait for sem to be 0 then inc by 1
                if (semop(semid, wait_and_inc, 2) == -1) {
                    handle_error("semop");
                }

                addr[type_idx_mult * resource_type + val_idx_off] += resources_needed - '0';
                msync(addr, sb.st_size, MS_SYNC | MS_INVALIDATE);

                // Decrement by 1, back to 0
                if (semop(semid, &dec, 1) == -1) {
                    handle_error("semop");
                }
            } else {
                std::cout << "No more resources will be added." << std::endl;
                break;
            }
        } while (true);

        // Cleanup
        if (munmap(addr, sb.st_size) == -1) {
            handle_error("unmap");
        }

        if (semctl(semid, 0, IPC_RMID, dummy) == -1) {
            handle_error("semctl");
        }
    }
}
