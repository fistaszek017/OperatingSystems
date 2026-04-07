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

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Wywołanie: %s default|mask|ignore|handle\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "default") == 0) {
        sig_default();
    } else if (strcmp(argv[1], "mask") == 0) {
        sig_mask();
    } else if (strcmp(argv[1], "ignore") == 0) {
        sig_ignore();
    } else if (strcmp(argv[1], "handle") == 0) {
        sig_handle();
    } else {
        fprintf(stderr, "Wywołanie: %s default|mask|ignore|handle\n", argv[0]);
        return 1;
    }


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