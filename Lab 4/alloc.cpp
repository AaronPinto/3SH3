//
// Created by aaron on 2021-03-24.
//

#include "lab4.h"
#include <fcntl.h>
#include <iostream>
#include <sys/mman.h>
#include <sys/stat.h>

int main() {
    unsigned char *addr;
    int fd, semid;
    struct stat sb {};

    // 0. Get semaphore
    semid = semget(key, 1, 0600);
    if (semid == -1) {
        handle_error("semget");
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

    // 4. Loop asking resources needed
    int resource_type = 0;
    unsigned char resources_needed;

    do {
        std::cout << "How many units of resource " << resource_type / type_idx_mult << " are needed?" << std::endl;
        std::cin >> resources_needed;

        // Wait for sem to be 0 then inc by 1
        if (semop(semid, wait_and_inc, 2) == -1) {
            handle_error("semop");
        }

        if (addr[resource_type + val_idx_off] >= resources_needed) {
            addr[resource_type + val_idx_off] -= resources_needed - '0';
            msync(addr, sb.st_size, MS_SYNC | MS_INVALIDATE);

            // Decrement by 1, back to 0
            if (semop(semid, &dec, 1) == -1) {
                handle_error("semop");
            }
        } else {
            // Decrement by 1, back to 0
            if (semop(semid, &dec, 1) == -1) {
                handle_error("semop");
            }

            std::cout << "Not enough resources of type " << resource_type / type_idx_mult << "!" << std::endl;
            break;
        }

        resource_type += type_idx_mult;

        if (resource_type >= type_idx_mult * num_types) {
            resource_type = 0;
        }
    } while (true);

    exit(EXIT_SUCCESS);
}
