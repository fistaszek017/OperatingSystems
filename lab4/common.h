#ifndef COMMON_H
#define COMMON_H

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SERVER_KEY  12345
#define MAX_CLIENTS 10
#define MAX_MSG_LEN 256

#define MSG_INIT    1
#define MSG_CHAT    2
#define MSG_REPLY   3
//struktura komunikatu
struct msgbuf {
    long  mtype;                //typ
    int   client_id;
    key_t client_key;
    char  text[MAX_MSG_LEN];
};

#endif