#include "common.h"
#include <signal.h>

//tablica zarejestrowanych
static int  client_queues[MAX_CLIENTS];
static int  client_count = 0;
static int  server_qid   = -1;

static void cleanup(int sig)
{
    (void)sig;
    if (server_qid != -1) {
        msgctl(server_qid, IPC_RMID, NULL);
        printf("\n[serwer] Kolejka usunięta. Do widzenia.\n");
    }
    exit(0);
}

int main(void)
{
    struct msgbuf msg;
    struct msgbuf reply;

    signal(SIGINT,  cleanup);
    signal(SIGTERM, cleanup);

    //kolejka serwera
    server_qid = msgget(SERVER_KEY, IPC_CREAT | IPC_EXCL | 0666);
    if (server_qid < 0) {
        perror("msgget (server)");
        fprintf(stderr, "Wskazówka: usuń starą kolejkę\n");
        exit(1);
    }
    printf("[serwer] Uruchomiony. ID kolejki = %d\n", server_qid);

    for (;;) {
        if (msgrcv(server_qid, &msg, sizeof(msg) - sizeof(long), 0, 0) < 0) {
            perror("msgrcv");
            continue;
        }

        //init
        if (msg.mtype == MSG_INIT) {
            if (client_count >= MAX_CLIENTS) {
                fprintf(stderr, "[serwer] Brak miejsca dla nowego klienta.\n");
                continue;
            }

            int cqid = msgget(msg.client_key, 0666);
            if (cqid < 0) {
                perror("msgget (client queue)");
                continue;
            }

            int new_id = client_count;
            client_queues[client_count++] = cqid;

            printf("[serwer] Nowy klient: ID=%d, qid=%d\n", new_id, cqid);

            reply.mtype     = MSG_REPLY;
            reply.client_id = new_id;
            reply.client_key = 0;
            snprintf(reply.text, MAX_MSG_LEN, "Twój ID: %d", new_id);

            if (msgsnd(cqid, &reply, sizeof(reply) - sizeof(long), 0) < 0)
                perror("msgsnd (INIT reply)");
        }

        //chat
        else if (msg.mtype == MSG_CHAT) {
            int sender = msg.client_id;
            printf("[klient %d] %s\n", sender, msg.text);

            reply.mtype     = MSG_REPLY;
            reply.client_id = sender;
            reply.client_key = 0;
            snprintf(reply.text, MAX_MSG_LEN, "[klient %d] %s", sender, msg.text);

            for (int i = 0; i < client_count; i++) {
                if (i == sender) continue;
                if (msgsnd(client_queues[i], &reply,
                           sizeof(reply) - sizeof(long), 0) < 0) {
                    printf("[serwer] Klient %d rozłączył się, usuwam.\n", i);
                    // usun klienta z tablicy — przesuń pozostałych w lewo
                    for (int j = i; j < client_count - 1; j++) {
                        client_queues[j] = client_queues[j + 1];
                    }
                    client_count--;
                    i--;  // cofnij indeks bo tablica się skurczyła
                }
            }
        }
    }

    return 0;
}