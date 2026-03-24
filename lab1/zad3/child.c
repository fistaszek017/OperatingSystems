#include "definitions.h"
#include <fcntl.h>   // flock()
#include <sys/file.h>


int zmiennaGlobalna = 2;




int main(int argc, char *argv[]) {
    if (argc < 2) return 1;

    int M = atoi(argv[1]);

    for (int i = 0; i < M; i++) {
        FILE *f = fopen(OUTPUT_FILE, "a");  //append
        if (!f) { perror("fopen"); return 1; }

        flock(fileno(f), LOCK_EX);  // blokada pliku

        fprintf(f, "Potomek (PID: %d)\n", getpid());

        flock(fileno(f), LOCK_UN);  // koniec blokady
        fclose(f);

        usleep(250000);
    }
    return 0;
}
