#include <stdio.h>
#include <signal.h>

void sig_default(void) {
    printf("Wywołano funkcję 'sig_default()'\n");
    struct sigaction sa;
    sa.sa_handler = SIG_DFL;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGUSR1, &sa, NULL);
}