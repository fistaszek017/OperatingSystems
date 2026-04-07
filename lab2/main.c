#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Wywołanie: %s default|mask|ignore|handle\n", argv[0]);
        return 1;
    }


    int typ;
    if      (strcmp(argv[1], "default") == 0) typ = 1;
    else if (strcmp(argv[1], "mask")    == 0) typ = 2;
    else if (strcmp(argv[1], "ignore")  == 0) typ = 3;
    else if (strcmp(argv[1], "handle")  == 0) typ = 4;
    else {
        fprintf(stderr, "Wywołanie: %s default|mask|ignore|handle\n", argv[0]);
        return 1;
    }

    //tworze proces potomny
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        //child
        execl("./child", "child", NULL);
        perror("execl");
        return 1;
    } else {
        //pauza
        sleep(1);

        union sigval val;
        val.sival_int = typ;
        sigqueue(pid, SIGUSR2, val);

        //czekam az dziecko skonczy
        wait(NULL);
    }

    return 0;
}
