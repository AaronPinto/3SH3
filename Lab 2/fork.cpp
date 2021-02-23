//
// Created by aaron on 2021-02-22.
//

#include <cstdio>
#include <unistd.h>

int main() {
    if (fork() == 0) {
        printf("This is the child process\n");
    } else {
        printf("I am the parent process\n");
    }

    printf("Both parent and child reach here.\n");

    return 0;
}
