#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

void signal_handler(int signum) {
    printf("Wywołano handler dla sygnału %d\n", signum);
}

void sig_default(void) {
    printf("Wywołano funkcję 'sig_default()'\n");
    struct sigaction sa;
    sa.sa_handler = SIG_DFL;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGUSR1, &sa, NULL);
}

void sig_mask(void) {
    printf("Wywołano funkcję 'sig_mask()'\n");
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    sigprocmask(SIG_BLOCK, &mask, NULL);
}

void sig_ignore(void) {
    printf("Wywołano funkcję 'sig_ignore()'\n");
    struct sigaction sa;
    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGUSR1, &sa, NULL);
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

void usr2_handler(int signum, siginfo_t *info, void *context) {
    int typ = info->si_value.sival_int;

    if      (typ == 1) sig_default();
    else if (typ == 2) sig_mask();
    else if (typ == 3) sig_ignore();
    else if (typ == 4) sig_handle();
}

int main(void) {
    struct sigaction sa2;
    sa2.sa_sigaction = usr2_handler;
    sigemptyset(&sa2.sa_mask);
    sa2.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR2, &sa2, NULL);

    pause();

    for (int i = 1; i <= 20; i++) {
        printf("%d\n", i);
        fflush(stdout);

        if (i == 5 || i == 15) {
            printf("Wysyłam sygnał USR1\n");
            fflush(stdout);
            raise(SIGUSR1);
        }

        if (i == 10) {
            sigset_t pending;
            sigpending(&pending);
            if (sigismember(&pending, SIGUSR1)) {
                printf("Odblokowuję USR1\n");
                fflush(stdout);
                sig_unblock();
            }
        }

        sleep(1);
    }

    printf("Pętla została wykonana w całości\n");
    return 0;
}