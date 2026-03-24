//
// Created by sfist on 11.03.2026.
//

#include <stdio.h>
#include <stdlib.h>


#define M 5

int zmiennaGlobalna = 2;


int main (int argc, char *argv[]) {
    int N = atoi(argv[1]);
    for (int i = 0; i < N; i++) {
        pid_t pid = fork();
        if (pid < 0) {
            return 1; //blad
        }

        if (pid == 0) { //potomek
            zmiennaGlobalna++;

            for (int j = 0; j < M; j++) {
                printf("Potomek (%d)\n", getpid());
            }
            exit(0); //dodaje zeby rodzic nie wpadal w potomka
        }
    }

    for (int i = 0; i < N; i++) {
        wait(NULL);
    }

    printf("Rodzic  (%d) zmiennaGlobalna=%d\n", getpid(), zmiennaGlobalna);
}
