#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#define FIFO_TO_CALC   "/tmp/fifo_to_calc"    // program1 -> program2
#define FIFO_TO_RESULT "/tmp/fifo_to_result"  // program2 -> program1

double f(double x) {
    return 4.0 / (x * x + 1.0);
}

double integrate(double a, double b, double dx) {
    double sum = 0.0;
    for (double x = a; x < b; x += dx) {
        sum += f(x) * dx;
    }
    return sum;
}

int main(void) {
    //tworzymy fifo w razie W
    mkfifo(FIFO_TO_CALC,   0666);
    mkfifo(FIFO_TO_RESULT, 0666);

    printf("Program 2 uruchomiony. Czekam na dane od programu 1...\n");

    // otwieram odczyt fifo1
    int fd_in = open(FIFO_TO_CALC, O_RDONLY);
    if (fd_in < 0) {
        perror("open FIFO_TO_CALC (odczyt)");
        return 1;
    }

    double a, b, dx;
    read(fd_in, &a,  sizeof(double));
    read(fd_in, &b,  sizeof(double));
    read(fd_in, &dx, sizeof(double));
    close(fd_in);

    printf("Otrzymałem: a=%.6f, b=%.6f, dx=%.10f\n", a, b, dx);
    printf("Liczę całkę...\n");

    double result = integrate(a, b, dx);

    printf("Wynik: %.10f\n", result);
    printf("Odsyłam wynik do programu 1...\n");

    //odsylam wynik
    int fd_out = open(FIFO_TO_RESULT, O_WRONLY);
    if (fd_out < 0) {
        perror("open FIFO_TO_RESULT (zapis)");
        return 1;
    }

    write(fd_out, &result, sizeof(double));
    close(fd_out);

    printf("Gotowe.\n");

    return 0;
}