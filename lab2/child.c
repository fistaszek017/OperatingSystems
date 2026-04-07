#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#ifdef DYNAMIC
#include <dlfcn.h>
#endif

void sig_unblock(void);
void sig_default(void);
void sig_mask(void);
void sig_ignore(void);
void sig_handle(void);

void usr2_handler(int signum, siginfo_t *info, void *context) {
    int typ = info->si_value.sival_int;

#ifdef DYNAMIC
    // dynamiczne
    void *lib = dlopen("./libsig.so", RTLD_LAZY);
    if (!lib) { fprintf(stderr, "%s\n", dlerror()); return; }

    char nazwa[32];
    if      (typ == 1) snprintf(nazwa, sizeof(nazwa), "sig_default");
    else if (typ == 2) snprintf(nazwa, sizeof(nazwa), "sig_mask");
    else if (typ == 3) snprintf(nazwa, sizeof(nazwa), "sig_ignore");
    else if (typ == 4) snprintf(nazwa, sizeof(nazwa), "sig_handle");

    void (*funkcja)(void) = dlsym(lib, nazwa);
    if (funkcja) funkcja();
    dlclose(lib);
#else
    if      (typ == 1) sig_default();
    else if (typ == 2) sig_mask();
    else if (typ == 3) sig_ignore();
    else if (typ == 4) sig_handle();
#endif
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