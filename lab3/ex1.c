#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <math.h>

double f(double x) {
    return 4.0 / (x * x + 1.0);
}


double integrate_segment(double a, double b, double dx) {
    double sum = 0.0;
    for (double x = a; x < b; x += dx) {
        sum += f(x) * dx;
    }
    return sum;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Użycie: %s <szerokość_prostokąta> <n>\n", argv[0]);
        fprintf(stderr, "Przykład: %s 0.0000001 4\n", argv[0]);
        return 1;
    }

    double dx = atof(argv[1]);
    int n    = atoi(argv[2]);

    if (dx <= 0.0 || dx >= 1.0) {
        fprintf(stderr, "Błąd: szerokość prostokąta musi być z przedziału (0, 1).\n");
        return 1;
    }
    if (n < 1) {
        fprintf(stderr, "Błąd: n musi być >= 1.\n");
        return 1;
    }

    printf("Całkowana funkcja : f(x) = 4/(x^2+1) na [0,1]\n");
    printf("Szerokość prostokąta: %.10f\n", dx);
    printf("Wartość dokładna pi : %.10f\n\n", M_PI);
    printf("%-6s  %-20s  %-15s  %-15s\n",
           "k", "Wynik", "Czas [s]", "Błąd bezwzgl.");
    printf("--------------------------------------------------------------\n");

    for (int k = 1; k <= n; k++) {

        int (*pipes)[2] = malloc(k * sizeof(*pipes));
        if (!pipes) { perror("malloc"); return 1; }

        for (int i = 0; i < k; i++) {
            if (pipe(pipes[i]) == -1) { perror("pipe"); return 1; }
        }

        struct timeval t_start, t_end;
        gettimeofday(&t_start, NULL);

        //procesy potomne
        for (int i = 0; i < k; i++) {
            pid_t pid = fork();
            if (pid < 0) {
                perror("fork");
                return 1;
            }

            if (pid == 0) {
                //w procesie potomnym
                for (int j = 0; j < k; j++) {
                    close(pipes[j][0]);
                    if (j != i) close(pipes[j][1]);
                }

                double segment = 1.0 / k;
                double a_i = i * segment;
                double b_i = (i == k - 1) ? 1.0 : a_i + segment;

                double partial = integrate_segment(a_i, b_i, dx);

                //wysylam wynik przez potok
                write(pipes[i][1], &partial, sizeof(double));
                close(pipes[i][1]);

                free(pipes);
                exit(0);
            }
            close(pipes[i][1]);
        }

        //zbieram wynik
        double total = 0.0;
        for (int i = 0; i < k; i++) {
            double partial = 0.0;
            read(pipes[i][0], &partial, sizeof(double));
            close(pipes[i][0]);
            total += partial;
        }

        /* Poczekaj na wszystkie procesy potomne */
        for (int i = 0; i < k; i++) {
            wait(NULL);
        }

        gettimeofday(&t_end, NULL);
        double elapsed = (t_end.tv_sec  - t_start.tv_sec) +
                         (t_end.tv_usec - t_start.tv_usec) * 1e-6;

        printf("%-6d  %.10f          %-15.6f  %.10f\n",
               k, total, elapsed, fabs(total - M_PI));

        free(pipes);
    }

    return 0;
}