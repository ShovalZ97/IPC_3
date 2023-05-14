#ifndef MYHEADER_server_H
#define MYHEADER_server_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/un.h>
#include <errno.h>
#include <sys/un.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <netdb.h>
#include <poll.h>




#define MAX_FILENAME_LEN 256
#define MAX_FILE_SIZE 1024 * 1024
#define SOCK_PATH_C "echo_socket"
#define BUFFER_SIZE 1024
#define SOCK_PATH "echo_socket"
#define FIFO_NAME "myfifo"
#define FIFO_FILE "myFif"


void error(const char *msg);
void ipv4_tcp_srver(int port, int q);
void ipv4_udp_srver(int port, int q);
void ipv6_tcp_srver(int port, int q);
void ipv6_udp_srver(int port, int q);
void uds_stream_srver(int q);
void uds_dgram_srver(int q);
void mmap_filenam_srvere(int port, int q);
void pipe_filename_srver(int q);

int server_partB(int argc, char *argv[]);

// int server_main_test(int argc, char *argv[], int q);
void checksum(const char *filename, int q);

#endif
