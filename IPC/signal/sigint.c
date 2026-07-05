//
// Created by Peter on 2026/6/28.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <pthread.h>
void sigint_handler(int sig) {
    const char msg[] = "Ahhh! SIGINT!\n";
    write(0, msg, sizeof(msg));
}

int main(void)
{
    char s[200];
    struct sigaction sa = {
            .sa_handler = sigint_handler,
            .sa_flags = SA_RESTART, // or SA_RESTART
    };
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }
    printf("Enter a string:\n");
    if (fgets(s, sizeof s, stdin) == NULL)
        perror("fgets");
    else
        printf("You entered: %s\n", s);

    return 0;
}




