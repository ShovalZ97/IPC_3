#ifndef MYCHAT_H
#define MYCHAT_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <poll.h>


#define BUFFER_SIZE 1024

int server_chat(int argc, char *argv[]);
int client_chat(int argc, char *argv[]);


#endif
