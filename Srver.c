#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include <openssl/rand.h>
#include <openssl/sha.h>
#include <time.h>
#include <sys/time.h>
#include <fcntl.h>
#include <sys/mman.h>

#include <errno.h>
#include <netdb.h>
#include <poll.h>

#define CHUNK_SIZE 1024
#include "Server.h"

void calculate_checksum1(unsigned char *data, size_t size, unsigned char *checksum)
{
    SHA256(data, size, checksum);
}

void ipv4_tcp_srver(int port, int q)
{
    printf("ipv4_tcp,");

    int sockfd, newsockfd, fdFile;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t clilen;
    ssize_t n;
    unsigned char buffer[CHUNK_SIZE];

    if (q == 0)
    {
        printf("receive_file: listening on port %d\n", port);
    }
   
    // socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        fprintf(stderr, "Failed to create socket\n");
        // return 1;
        exit(1);
    }

    // ipv4
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(port);

    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1)
    {
        fprintf(stderr, "Failed to bind socket\n");
        // return 1;
        exit(1);
    }

    if (listen(sockfd, 5) == -1)
    {
        fprintf(stderr, "Failed to listen on socket\n");
        // return 1;
        exit(1);
    }

    clilen = sizeof(cliaddr);
    newsockfd = accept(sockfd, (struct sockaddr *)&cliaddr, &clilen);
    if (newsockfd == -1)
    {
        fprintf(stderr, "Failed to accept connection\n");
        // return 1;
        exit(1);
    }

    fdFile = open("received_file", O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    if (fdFile == -1)
    {
        fprintf(stderr, "Failed to open file\n");
        // return 1;
        exit(1);
    }
    struct timeval start, end;
    gettimeofday(&start, NULL);

    while ((n = read(newsockfd, buffer, CHUNK_SIZE)) > 0)
    {
        ssize_t bytes_written = write(fdFile, buffer, n);
        if (bytes_written == -1)
        {
            fprintf(stderr, "Failed to write to file\n");
            // return 1;
            exit(1);
        }
    }

    if (n == -1)
    {
        fprintf(stderr, "Failed to read from socket\n");
        // return 1;
        exit(1);
    }
    
    gettimeofday(&end, NULL);
    long elapsed_time = ((end.tv_sec - start.tv_sec) * 1000.0) + ((end.tv_usec - start.tv_usec) / 1000.0);
    printf("%.2ld\n", elapsed_time);

    close(newsockfd);
    close(sockfd);
    close(fdFile);
    if (q == 0)
    {
        printf("\nFile received successfully\n");
    }
    

    // return 0;
}
void ipv4_udp_srver(int port, int q)
{
    printf("ipv4_udp,");
    int sockfd, filefd, bytess, totBytesRec = 0;
    int timeout_ms = 3000;
    struct sockaddr_in servaddr, cliaddr;
    struct timeval tv;
    socklen_t cli_len;
    char buffer[BUFFER_SIZE * 1024];
    fd_set readfds;

    // ipv4
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(port);

    // Create a socket for the server
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        fprintf(stderr, "Failed to create socket\n");
        exit(1);
    }

    // Bind the socket to the server address
    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    // Set up select for waiting on the socket
    FD_ZERO(&readfds);
    FD_SET(sockfd, &readfds);

    // Open File write
    filefd = open("Rec_File", O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    if (filefd < 0)
    {
        perror("open");
        exit(1);
    }
    struct timeval start, end;
    gettimeofday(&start, NULL);
    // Receive the file
    while (totBytesRec < 100 * 1024 * 1024)
    {
        tv.tv_sec = timeout_ms / 1000;
        tv.tv_usec = (timeout_ms % 1000) * 1000;

        int selectret = select(sockfd + 1, &readfds, NULL, NULL, &tv);
        if (selectret < 0)
        {
            perror("select");
            exit(EXIT_FAILURE);
        }
        // Timeout
        else if (selectret == 0)
        {
            break;
        }
        else
        {
            cli_len = sizeof(cliaddr);
            bytess = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&cliaddr, &cli_len);
            if (bytess < 0)
            {
                perror("recvfrom");
                exit(1);
            }
            totBytesRec += bytess;
            if (write(filefd, buffer, bytess) != bytess)
            {
                perror("write");
                exit(1);
            }
        }
    }
    gettimeofday(&end, NULL);
    long elapsed_time = ((end.tv_sec - start.tv_sec) * 1000.0) + ((end.tv_usec - start.tv_usec) / 1000.0);
    printf("%.2ld\n", elapsed_time);
    close(filefd);
    close(sockfd);
    if (q == 0)
    {
        printf("File received successfully\n");
    }
}

void ipv6_tcp_srver(int port, int q)
{
    printf("ipv6_tcp,");
    int sockfd, connfd, fdFile, countbytes;
    socklen_t cli_len;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in6 servaddr_ipv6, cliaddr_ipv6;
    unsigned char *data, checksum[SHA256_DIGEST_LENGTH];
    ssize_t  total_bytes_read = 0;//bytes_read,
    size_t data_size = CHUNK_SIZE;
    long elapsed_time;

    if (q == 0)
    {
        printf("ipv6_tcp_srver\n");
    }
    // Create a socket for the server
    sockfd = socket(AF_INET6, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Set up the server address
    memset(&servaddr_ipv6, 0, sizeof(servaddr_ipv6));
    servaddr_ipv6.sin6_family = AF_INET6;
    servaddr_ipv6.sin6_addr = in6addr_any;
    servaddr_ipv6.sin6_port = htons(port);

    // Bind the socket to the server address
    if (bind(sockfd, (struct sockaddr *)&servaddr_ipv6, sizeof(servaddr_ipv6)) < 0)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(sockfd, 5) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // Accept a connection from a client
    cli_len = sizeof(cliaddr_ipv6);
    connfd = accept(sockfd, (struct sockaddr *)&cliaddr_ipv6, &cli_len);
    if (connfd < 0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    // Open a file for writing
    fdFile = open("receive_tcp_ip6.txt", O_CREAT | O_WRONLY, 0644);
    if (fdFile < 0)
    {
        perror("open");
        exit(EXIT_FAILURE);
    }
    //     // allocate memory for data buffer
    data = (unsigned char *)malloc(data_size);

    // receive the file contents from the client
    struct timeval start, end;
    gettimeofday(&start, NULL);

    while ((countbytes = read(connfd, buffer, sizeof(buffer))) > 0)
    {
        total_bytes_read += total_bytes_read;
        if (write(fdFile, buffer, countbytes) != countbytes)
        {
            fprintf(stderr, "Failed to write to file\n");
            exit(1);
        }
    }

    gettimeofday(&end, NULL);
    elapsed_time = ((end.tv_sec - start.tv_sec) * 1000.0) + ((end.tv_usec - start.tv_usec) / 1000.0);

    printf("%.2ld\n", elapsed_time);
    
    // print the calculated checksum and elapsed time

    // Close
    close(fdFile);
    close(connfd);
    close(sockfd);
    if (q == 0)
    {
    // calculate checksum of the received data
    calculate_checksum1(data, total_bytes_read, checksum);

        printf("Checksum of the received file: ");
        for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
        {
            printf("%02x", checksum[i]);
        }
        printf("\n");
    }
}
void ipv6_udp_srver(int port, int q)
{
    printf("ipv6_udp,");
    int sockfd, filefd, bytess, totBytesRec = 0;
    int timeout_ms = 3000;
    struct sockaddr_in6 servaddr, cliaddr;
    struct timeval tv;
    socklen_t cli_len;
    char buffer[BUFFER_SIZE * 1024];
    fd_set readfds;

    // ipv6
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin6_family = AF_INET6;
    servaddr.sin6_addr = in6addr_any;
    servaddr.sin6_port = htons(port);

    // Create a socket for the server
    sockfd = socket(AF_INET6, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        fprintf(stderr, "Failed to create socket\n");
        exit(1);
    }

    // Bind the socket to the server address
    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    // Set up select for waiting on the socket
    FD_ZERO(&readfds);
    FD_SET(sockfd, &readfds);

    // Open a file for writing
    filefd = open("received_file", O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    if (filefd < 0)
    {
        perror("open");
        exit(1);
    }
    struct timeval start, end;
    gettimeofday(&start, NULL);
    // Receive the file
    while (totBytesRec < 100 * 1024 * 1024)
    {
        tv.tv_sec = timeout_ms / 1000;
        tv.tv_usec = (timeout_ms % 1000) * 1000;

        int selectret = select(sockfd + 1, &readfds, NULL, NULL, &tv);
        if (selectret < 0)
        {
            perror("select");
            exit(EXIT_FAILURE);
        }
        // Timeout
        else if (selectret == 0)
        {
            break;
        }
        else
        {
            cli_len = sizeof(cliaddr);
            bytess = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&cliaddr, &cli_len);
            if (bytess < 0)
            {
                perror("recvfrom");
                exit(1);
            }
            totBytesRec += bytess;
            if (write(filefd, buffer, bytess) != bytess)
            {
                perror("write");
                exit(1);
            }
        }
    }

    gettimeofday(&end, NULL);
    long elapsed_time = ((end.tv_sec - start.tv_sec) * 1000.0) + ((end.tv_usec - start.tv_usec) / 1000.0);
    printf("%.2ld\n", elapsed_time);

    close(filefd);
    close(sockfd);
    if(q==0){
        printf("File received successfully\n");
    }
}
#define _GNU_SOURCE

void uds_dgram_srver(int q)
{
    printf("uds_dgram,");

    int sockfd;
    int len;
    int fdFile;
    int file_size = 0;
    int total = 0, countbyte = 0;
    struct sockaddr_un servaddr_un = {.sun_family = AF_UNIX};
    char dataa[1024] = {0};

    sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sockfd == -1)
    {
        perror("socket");
        goto exit_failure;
    }

    strcpy(servaddr_un.sun_path, SOCK_PATH);
    unlink(servaddr_un.sun_path);
    len = strlen(servaddr_un.sun_path) + sizeof(servaddr_un.sun_family);
    if (bind(sockfd, (struct sockaddr *)&servaddr_un, len) == -1)
    {
        perror("bind");
        goto exit_failure;
    }

    socklen_t slen = sizeof(servaddr_un);
    if (recvfrom(sockfd, &file_size, sizeof(file_size), 0,
                 (struct sockaddr *)&servaddr_un, &slen) == -1)
    {
        perror("recvfrom");
        goto exit_failure;
    }

    fdFile = open("receive_uds_d.txt", O_CREAT | O_WRONLY, 0644);
    if (fdFile == -1)
    {
        perror("open");
        goto exit_failure;
    }
    struct timeval start, end;
    gettimeofday(&start, NULL);
    while (total < file_size)
    {
        socklen_t slen = sizeof(servaddr_un);
        countbyte = recvfrom(sockfd, dataa, sizeof(dataa), 0, (struct sockaddr *)&servaddr_un, &slen);
        if (countbyte <= 0)
        {
            if (countbyte == -1)
            {
                perror("recvfrom");
            }
            goto exit_failure;
        }

        if (write(fdFile, dataa, countbyte) == -1)
        {
            perror("write");
            goto exit_failure;
        }

        total += countbyte;
    }

    gettimeofday(&end, NULL);
    long elapsed_time = ((end.tv_sec - start.tv_sec) * 1000.0) + ((end.tv_usec - start.tv_usec) / 1000.0);
    printf("%.2ld\n", elapsed_time);

    close(fdFile);
    close(sockfd);

    if (total != file_size)
    {
        fprintf(stderr, "File transfer failed.\n");
        goto exit_failure;
    }
    if (q == 0)
    {
        printf("arivve %d byte.\n", total);
        printf("file size: %d .\n", file_size);
    }

    // return 0;

exit_failure:
    if (fdFile != -1)
    {
        close(fdFile);
    }
    if (sockfd != -1)
    {
        close(sockfd);
    }
    // return 1;
}
void uds_stream_srver(int q)
{
    printf("uds_stream,");
    int file_size;
    int sockfd, len, fdFile, accept_s;
    struct sockaddr_un cli_un, servaddr_un = {
                                   .sun_family = AF_UNIX,
                               };
    char dataa[1024];

    if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
    {
        perror("socket");
        exit(1);
    }

    strcpy(servaddr_un.sun_path, SOCK_PATH);
    unlink(servaddr_un.sun_path);
    len = strlen(servaddr_un.sun_path) + sizeof(servaddr_un.sun_family);
    if (bind(sockfd, (struct sockaddr *)&servaddr_un, len) == -1)
    {
        perror("bind");
        exit(1);
    }
    // Set the socket to listen for incoming connections
    if (listen(sockfd, 2) == -1)
    {
        perror("listen");
        exit(1);
    }

    socklen_t slen = sizeof(cli_un);
    if ((accept_s = accept(sockfd, (struct sockaddr *)&cli_un, &slen)) == -1)
    {
        perror("accept");
        exit(1);
    }

    struct timeval start, end;
    gettimeofday(&start, NULL);
    // file size
    if (recv(accept_s, &file_size, sizeof(file_size), 0) < 0)
    {
        perror("recv");
        exit(1);
    }
    if ((fdFile = open("receive_uds_s.txt", O_CREAT | O_WRONLY, 0644)) < 0)
    {
        perror("open");
        exit(1);
    }

    int totalbyte = 0, countbyte = 0;
    while (totalbyte < file_size)
    {
        countbyte = recv(accept_s, dataa, sizeof(dataa), 0);
        if (countbyte < 0)
            perror("countbyte = recv");
        if (countbyte == 0)
        {

            break;
        }

        if (write(fdFile, dataa, countbyte) < 0)
        {
            perror("write");
            break;
        }

        totalbyte += countbyte;
        // printf("total%d += %d n\n",total , n);
    }
    // printf("after while\n");

    gettimeofday(&end, NULL);
    long elapsed_time = ((end.tv_sec - start.tv_sec) * 1000.0) + ((end.tv_usec - start.tv_usec) / 1000.0);
    printf("%.2ld\n", elapsed_time);


    close(fdFile);
    close(accept_s);

    if (totalbyte != file_size)
    {
        error("totalbyte != file_size.\n");
    }
}
void mmap_filenam_srver(int port, int q)
{
    printf("mmap_filenam,");
    const size_t chunk_size = CHUNK_SIZE;
    char *data;

    int fdFile = open("File.txt", O_RDWR | O_CREAT | O_TRUNC, (mode_t)0600);
    if (fdFile == -1)
    {
        perror("open");
        exit(1);
    }

    struct timeval start, end;
    gettimeofday(&start, NULL);
    if (ftruncate(fdFile, chunk_size) == -1)
    {
        perror("ftruncate");
        exit(1);
    }

    // Map the shared memory object into the address space of the calling process
    data = (char *)mmap(NULL, chunk_size, PROT_WRITE, MAP_SHARED, fdFile, 0);
    if (data == MAP_FAILED)
    {
        perror("mmap");
        exit(1);
    }

    // Wait for the client to finish writing to the shared memory object
    sleep(2);

    // Print the contents of the shared memory object
    if(q==0){
        printf("Contents of shared memory object:\n%s\n", data);
    }
    // Unmap the shared memory object
    if (munmap(data, chunk_size) == -1)
    {
        perror("munmap");
        exit(1);
    }

    // Remove the shared memory object
    if (unlink("File.txt") == -1)
    {
        perror("unlink");
        exit(1);
    }
  
    gettimeofday(&end, NULL);
    long elapsed_time = ((end.tv_sec - start.tv_sec) * 1000.0) + ((end.tv_usec - start.tv_usec) / 1000.0);
    printf("%.2ld\n", elapsed_time);
    // Close the file descriptor
    close(fdFile);
    if(q==0){
    printf("File received successfully\n");
    }
}
void pipe_filename_srver(int q)
{
    printf("pipe_filename,");
    int fdPipe, fdFile;
    ssize_t bytesRead, bytesWritten;
    char buffer[1024];
    size_t totBytesRead = 0;

    struct timeval start, end;
    gettimeofday(&start, NULL);
    // Create the named pipe
    mkfifo(FIFO_FILE, 0666);

    // Open the named pipe for reading
    fdPipe = open(FIFO_FILE, O_RDONLY);
    if (fdPipe == -1)
    {
        perror("Failed to open named pipe");
        exit(EXIT_FAILURE);
    }

    // Open the file to write the received data
    fdFile = open("received_file.txt", O_WRONLY | O_CREAT | O_TRUNC, (mode_t)0600);
    if (fdFile == -1)
    {
        perror("Failed to open file for writing");
        exit(1);
    }

    // Read data from the named pipe and write it to the file
    while ((bytesRead = read(fdPipe, buffer, 1024)) > 0)
    {
        bytesWritten = write(fdFile, buffer, bytesRead);
        if (bytesWritten != bytesRead)
        {
            perror("Failed to write to file");
            exit(1);
        }
        totBytesRead += bytesRead;
    }
    if (bytesRead == -1)
    {
        perror("Failed to read from named pipe");
        exit(EXIT_FAILURE);
    }

    gettimeofday(&end, NULL);
    long elapsed_time = ((end.tv_sec - start.tv_sec) * 1000.0) + ((end.tv_usec - start.tv_usec) / 1000.0);
    printf("%.2ld\n", elapsed_time);
    if(q==0){
        printf("Data received successfully. Total bytes received: %zu\n", totBytesRead);
        printf("File received successfully\n");
    }

    // Close the file and named pipe
    close(fdFile);
    close(fdPipe);

    // Remove the named pipe file
    if (unlink(FIFO_FILE) == -1)
    {
        perror("Failed to remove named pipe");
        exit(1);
    }
}
int server_partB(int argc, char *argv[])
{
    int q1 = 0;
    if (argc >= 5)
    {
        if (strcmp(argv[4], "-q") == 0)
        {
            q1 = 1;
        }
    }
    // printf("argv[0] %s\n",argv[0]);
    // printf("argv[1] %s\n",argv[1]);
    // printf("argv[2] %s\n",argv[2]);
    // printf("argv[3] %s\n",argv[3]);
    // printf("argv4[] %s\n",argv[4]);
    // printf("argv[5] %s\n",argv[5]);
    // printf("argv[6] %s\n",argv[6]);
    int port = atoi(argv[2]);

    // Create a socket for the server
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        perror("socket() failed");
        exit(EXIT_FAILURE);
    }

    // Set up the address of the server to bind to
    struct sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port + 1);

    // Bind the socket to the server address
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("bind() failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_socket, 5) == -1)
    {
        perror("listen() failed");
        exit(EXIT_FAILURE);
    }

    // Accept a connection from a client
    struct sockaddr_in client_addr = {0};
    socklen_t client_addr_len = sizeof(client_addr);

    int client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
    if (client_socket == -1)
    {
        perror("accept() failed");
        exit(EXIT_FAILURE);
    }

    // Receive the first message from the client and save it in a variable
    char type[256];
    int message1_len = recv(client_socket, type, sizeof(type), 0);
    if (message1_len == -1)
    {
        perror("recv() failed");
        exit(EXIT_FAILURE);
    }
    type[message1_len] = '\0';
    

    // Receive the second message from the client and save it in a variable
    char param[256];
    int message2_len = recv(client_socket, param, sizeof(param), 0);
    if (message2_len == -1)
    {
        perror("recv() failed");
        exit(EXIT_FAILURE);
    }
    param[message2_len] = '\0';
    if(q1==0){
            printf("Received message 2: %s\n", param); // param
            printf("Received message 1: %s\n", type); // type

    }

    // Close the client socket and server socket, and exit
    close(client_socket);
    close(server_socket);

    
    if (strcmp(type, "ipv4") == 0 && (strcmp(param, "tcp")) == 0)
    {
        ipv4_tcp_srver(port, q1);
    }
    else if (strcmp(type, "ipv4") == 0 && (strcmp(param, "udp")) == 0)
    {
        ipv4_udp_srver(port, q1);
    }
    else if (strcmp(type, "ipv6") == 0 && (strcmp(param, "tcp")) == 0)
    {

        ipv6_tcp_srver(port, q1);
    }
    else if (strcmp(type, "ipv6") == 0 && (strcmp(param, "udp")) == 0)
    {

        ipv6_udp_srver(port, q1);
    }

    else if (strcmp(type, "mmap") == 0 )//&& (strcmp(param, "filename")) == 0)
    {
// printf("mmap_filenam,");
        mmap_filenam_srver(port, q1);
    }
    else if (strcmp(type, "pipe") == 0)// && (strcmp(param, "filename")) == 0)
    {

        pipe_filename_srver(q1);
    }
    else if (strcmp(type, "uds") == 0 && (strcmp(param, "dgram")) == 0)
    {

        uds_dgram_srver(q1);
    }
    else if (strcmp(type, "uds") == 0 && (strcmp(param, "stream")) == 0)
    {

        uds_stream_srver(q1);
    }

    return 0;
}