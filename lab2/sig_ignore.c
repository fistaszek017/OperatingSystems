#include <stdio.h>
#include <signal.h>

void sig_ignore(void) {
    printf("Wywołano funkcję 'sig_ignore()'\n");
    struct sigaction sa;
    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGUSR1, &sa, NULL);
}