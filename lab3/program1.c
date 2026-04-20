#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#define FIFO_TO_CALC   "/tmp/fifo_to_calc"    // program1 -> program2
#define FIFO_TO_RESULT "/tmp/fifo_to_result"  // program2 -> program1

int main(void) {
    mkfifo(FIFO_TO_CALC,   0666);
    mkfifo(FIFO_TO_RESULT, 0666);

    double a, b, dx;

    printf("Podaj lewy kraniec przedziału (a): ");
    scanf("%lf", &a);

    printf("Podaj prawy kraniec przedziału (b): ");
    scanf("%lf", &b);

    printf("Podaj szerokość prostokąta (dx, np. 0.0000001): ");
    scanf("%lf", &dx);

    if (a >= b) {
        fprintf(stderr, "Błąd: a musi być mniejsze od b.\n");
        return 1;
    }
    if (dx <= 0.0) {
        fprintf(stderr, "Błąd: dx musi być dodatnie.\n");
        return 1;
    }

    printf("\nWysyłam przedział [%.6f, %.6f] z dx=%.10f do programu 2...\n",
           a, b, dx);

    //otwieram FIFO
    int fd_out = open(FIFO_TO_CALC, O_WRONLY);
    if (fd_out < 0) {
        perror("open FIFO_TO_CALC (zapis)");
        return 1;
    }

    //wysylam
    write(fd_out, &a,  sizeof(double));
    write(fd_out, &b,  sizeof(double));
    write(fd_out, &dx, sizeof(double));
    close(fd_out);

    printf("Dane wysłane. Czekam na wynik...\n");

    //otwieram fifo2, czekam na wynik od programu 2
    int fd_in = open(FIFO_TO_RESULT, O_RDONLY);
    if (fd_in < 0) {
        perror("open FIFO_TO_RESULT (odczyt)");
        return 1;
    }

    double result = 0.0;
    read(fd_in, &result, sizeof(double));
    close(fd_in);

    //wynik
    printf("\n=== WYNIK ===\n");
    printf("Całka z f(x) = 4/(x²+1) na [%.6f, %.6f] = %.10f\n", a, b, result);

    return 0;
}