#include <cstdio>
#include <csignal>
#include <unistd.h>
#include <cstdlib>

// user-defined signal handler for alarm.
void alarm_handler(int signo) {
    if (signo == SIGALRM) {
        printf("Alarm\n");
    } else if (signo == SIGINT) {
        printf("CTRL+C pressed!\n");
    } else if (signo == SIGTSTP) {
        printf("CTRL+Z pressed!\n");
        exit(0);
    }
}

int main() {
    // register the signal handler for all signals that it handles
    if (signal(SIGALRM, alarm_handler) == SIG_ERR || signal(SIGINT, alarm_handler) == SIG_ERR ||
        signal(SIGTSTP, alarm_handler) == SIG_ERR) {

        printf("failed to register alarm handler.");
        exit(1);
    }

    // wait until alarm goes off
    while (1) {
        alarm(2); // set alarm to fire in 2 seconds.
        sleep(2);
    }
}