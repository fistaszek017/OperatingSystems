#include <stdio.h>
#include <signal.h>
#include <stddef.h>

void signal_handler(int signum) {
    printf("Wywołano handler dla sygnału %d\n", signum);
}

void sig_handle(void) {
    printf("Wywołano funkcję 'sig_handle()'\n");
    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGUSR1, &sa, NULL);
}

void sig_unblock(void) {
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    sigprocmask(SIG_UNBLOCK, &mask, NULL);
}