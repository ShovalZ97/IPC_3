#ifndef MYHEADER_H
#define MYHEADER_H

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <openssl/rand.h>
#include <openssl/sha.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/un.h>
#include <time.h>
#include <unistd.h>

#define SOCK_PATH_C "echo_socket"
#define _GNU_SOURCE
#define CHUNK_SIZE 104857600 // 100 MB
#define FIFO_FILE "myFif"

void generate_data(unsigned char *data, size_t size);
void calculate_checksum(unsigned char *data, size_t size, unsigned char *checksum);
void create_file();//const char *filename
void ipv4_tcp(char* ip_address , int port);
void ipv4_udp(char* ip_address, int port);
void ipv6_tcp(char* ip_address, int port);
void ipv6_udp(char *ip_address, int port);
void uds_dgram();
void uds_stream();
void mmap_Func(char *ip_address, int port);
void pipe_Func();
void sendFile(char* type, char* param , char* ip_address, int port);
int client_partB(int argc, char *argv[]);

// int client_test(int argc, char *argv[]);
#endif



// char* generate_data(size_t size);

