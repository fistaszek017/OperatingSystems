#include "definitions.h"

int main(int argc, char *argv[]) {

    int N = atoi(argv[1]);
    char *M = argv[2];

    remove(OUTPUT_FILE); //usuwam poprzedni plik z danymi
    for (int i = 0; i < N; i++) {
        pid_t pid = fork();
        if (pid < 0) {
            return 1;
        }

        if (pid == 0) {
            execl("./child", "child", M, NULL);
            _exit(2);
        }
    }

    for (int i = 0; i < N; i++) {
        wait(NULL);
    }

    printf("Rodzic  (PID: %d)\n", getpid());
    return 0;
}