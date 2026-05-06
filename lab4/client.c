#include "common.h"
#include <signal.h>

static int client_qid = -1;
static key_t client_key;

static void cleanup(int sig)
{
    (void)sig;
    if (client_qid != -1) {
        msgctl(client_qid, IPC_RMID, NULL);
    }
    exit(0);
}

int main(void)
{
    struct msgbuf msg;

    signal(SIGINT,  cleanup);
    signal(SIGTERM, cleanup);

    client_key = ftok("/tmp", getpid() & 0xFF);

    client_qid = msgget(client_key, IPC_CREAT | IPC_EXCL | 0666);
    if (client_qid < 0) {
        perror("msgget (client)");
        exit(1);
    }
    printf("[klient] Moja kolejka: qid=%d, key=0x%x\n",
           client_qid, (unsigned)client_key);

    int server_qid = msgget(SERVER_KEY, 0666);
    if (server_qid < 0) {
        perror("msgget (server)");
        cleanup(0);
    }

    msg.mtype      = MSG_INIT;
    msg.client_id  = 0;
    msg.client_key = client_key;
    msg.text[0]    = '\0';

    if (msgsnd(server_qid, &msg, sizeof(msg) - sizeof(long), 0) < 0) {
        perror("msgsnd (INIT)");
        cleanup(0);
    }

    if (msgrcv(client_qid, &msg, sizeof(msg) - sizeof(long),
               MSG_REPLY, 0) < 0) {
        perror("msgrcv (INIT reply)");
        cleanup(0);
    }
    int my_id = msg.client_id;
    printf("[klient] Zarejestrowany. %s\n", msg.text);

    pid_t child_pid = fork();
    if (child_pid < 0) {
        perror("fork");
        cleanup(0);
    }

    if (child_pid == 0) {
        for (;;) {
            if (msgrcv(client_qid, &msg, sizeof(msg) - sizeof(long),
                       MSG_REPLY, 0) < 0) {
                break;
            }
            printf("%s\n", msg.text);
            fflush(stdout);
        }
        exit(0);
    }

    char line[MAX_MSG_LEN];
    while (fgets(line, sizeof(line), stdin) != NULL) {
        line[strcspn(line, "\n")] = '\0';
        if (line[0] == '\0') continue;

        msg.mtype      = MSG_CHAT;
        msg.client_id  = my_id;
        msg.client_key = 0;
        strncpy(msg.text, line, MAX_MSG_LEN - 1);
        msg.text[MAX_MSG_LEN - 1] = '\0';

        if (msgsnd(server_qid, &msg, sizeof(msg) - sizeof(long), 0) < 0) {
            perror("msgsnd (CHAT)");
            break;
        }
    }

    printf("[klient %d] Rozłączam się.\n", my_id);

    kill(child_pid, SIGTERM);

    cleanup(0);
    return 0;
}