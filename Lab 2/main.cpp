//
// Created by aaron on 2021-02-22.
//

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <numeric>
#include <unistd.h>
#include <vector>
#include <sys/wait.h>

int main() {
    int pipe_ctp[2];
    int pipe_ptc[2];
    pid_t child_pid;
    std::vector<char> in_buf;

    pipe(pipe_ctp);
    pipe(pipe_ptc);

    if ((child_pid = fork()) == -1) {
        perror("fork");
        exit(0);
    }

    if (child_pid == 0) { // Child
        short num; // short, to handle the -1 properly cuz char doesn't

        // Read character from keyboard input, and write it to child -> parent pipe
        do {
            close(pipe_ctp[0]);
            std::cin >> num;
            write(pipe_ctp[1], &num, 1);
        } while (num != -1);

        // Read sum from parent -> child pipe, print it and terminate
        long sum = 0;
        close(pipe_ptc[1]);
        read(pipe_ptc[0], &sum, sizeof(sum));
        std::cout << "printing sum of buffer: " << sum << std::endl;

        exit(0);
    } else { // Parent
        signed char num; // specify signed, because on ARM, char is unsigned by default

        // Read character from child -> parent pipe, and add it to buffer
        do {
            close(pipe_ctp[1]);
            read(pipe_ctp[0], &num, 1);
            in_buf.push_back(num);
        } while (num != -1);

        // Calculate sum of elements (exclude -1) in the buffer, and write it to parent -> child pipe
        long sum_of_elems = std::accumulate(in_buf.begin(), in_buf.end() - 1, decltype(in_buf)::value_type(0));
        close(pipe_ptc[0]);
        write(pipe_ptc[1], &sum_of_elems, sizeof(sum_of_elems));

        waitpid(child_pid, nullptr, 0);
    }

    return 0;
}
