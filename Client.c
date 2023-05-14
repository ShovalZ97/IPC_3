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
#include "Client.h"

#define _GNU_SOURCE
#define CHUNK_SIZE 104857600 // 100 MB
#define MY_SERVER_IP "::1"

//?????????????  security the code
void generate_data(unsigned char *data, size_t size)
{
    if (RAND_bytes(data, size) != 1)
    {
        fprintf(stderr, "Failed to generate random data\n");
        exit(1);
    }
}

void calculate_checksum(unsigned char *data, size_t size, unsigned char *checksum)
{
    SHA256(data, size, checksum);
}
void create_file()
{
    FILE *fp;
    char *filename = "file.txt";
    long filesize = 100000000; // 100MB in bytes
    char *buffer = (char*) malloc(filesize); // allocate memory for file content
    if (buffer == NULL) {
        printf("Error: failed to allocate memory\n");
        // return 1;
    }
    fp = fopen(filename, "wb");
    if (fp == NULL) {
        printf("Error: failed to create file\n");
        // return 1;
    }
    fwrite(buffer, filesize, 1, fp); // write the content to file
    fclose(fp); // close the file
    free(buffer); // free the allocated memory
    // return 0;
}

void ipv4_tcp(char *ip_address, int port)
{
    printf( "ipv4_tcp\n");
    struct timeval end, start;
    int sockfd, fdFile;
    struct sockaddr_in servaddr_ipv4;
    struct stat file_stat;
    unsigned char *data, checksum[SHA256_DIGEST_LENGTH];
    size_t data_size = CHUNK_SIZE;
    long elapsed_time;

    // printf("ipv4_tcp 89:");

    fdFile = open("file.txt", O_RDONLY);
    if (fdFile == -1)
    {
        fprintf(stderr, "Failed to open file\n");
        exit(1);
    }
    if (fstat(fdFile, &file_stat) == -1)
    {
        fprintf(stderr, "Failed to stat file\n");
        exit(1);
    }

    data_size = file_stat.st_size;

    // allocate memory for data buffer
    data = (unsigned char *)malloc(data_size);
    generate_data(data, data_size);

    // read the file contents
    ssize_t bytes_read = read(fdFile, data, data_size);
    if (bytes_read == -1)
    {
        fprintf(stderr, "Failed to read file\n");
        exit(1);
    }

    // calculate checksum of the file contents
    calculate_checksum(data, bytes_read, checksum);

    // socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        fprintf(stderr, "Failed to create socket\n");
        exit(1);
    }

    // ipv4
    memset(&servaddr_ipv4, 0, sizeof(servaddr_ipv4));
    servaddr_ipv4.sin_family = AF_INET;
    servaddr_ipv4.sin_port = htons(port);
    if (inet_pton(AF_INET, ip_address, &servaddr_ipv4.sin_addr) <= 0)
    {
        fprintf(stderr, "Invalid IP address\n");
        exit(1);
    }

    if (connect(sockfd, (struct sockaddr *)&servaddr_ipv4, sizeof(servaddr_ipv4)) == -1)
    {
        fprintf(stderr, "Failed to connect to server\n");
        exit(1);
    }
    // Start timer
    gettimeofday(&start, NULL);

    if (write(sockfd, data, bytes_read) == -1)
    {
        fprintf(stderr, "Failed to write to socket\n");
        exit(1);
    }
    // Calculate elapsed time
    gettimeofday(&end, NULL);
    elapsed_time = ((end.tv_sec - start.tv_sec) * 1000.0) + ((end.tv_usec - start.tv_usec) / 1000.0);

    // print the calculated checksum
    printf("Checksum of the file: ");
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        printf("%02x", checksum[i]);
    }
    printf("\n");

    printf("time:   %.2ld\n", elapsed_time);
    close(sockfd);
    close(fdFile);
    // free memory
    free(data);
}
void ipv4_udp(char *ip_address, int port)
{
    struct timeval start, end;
    long elapsed_time;
    int sockfd, fdFile, bytes_sent, bytes_read;
    struct sockaddr_in serv_addr;
    struct stat file_stat;
    char bufData[1024];

    printf("ipv4_udp :");
    // Open the file for reading
    fdFile = open("file.txt", O_RDONLY);
    if (fdFile == -1)
    {
        printf("Failed to open file\n");
        exit(1);
    }

    if (fstat(fdFile, &file_stat) == -1)
    {
        fprintf(stderr, "Failed to stat file\n");
        exit(1);
    }

    sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd < 0)
    {
        perror("Failed to create socket");
        exit(1);
    }

    // ipv4
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip_address, &serv_addr.sin_addr) <= 0)
    {
        fprintf(stderr, "Invalid IP address\n");
        exit(1);
    }

    // start time
    gettimeofday(&start, NULL);

    while ((bytes_read = read(fdFile, bufData, sizeof(bufData))) > 0)
    {
        bytes_sent = sendto(sockfd, bufData, bytes_read, 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

        if (bytes_sent < 0)
        {
            fprintf(stderr, "Failed to read data\n");
            exit(1);
        }
        else if (bytes_sent != bytes_read)
        {
            fprintf(stderr, "sendto: Sent %d bytes instead of %d bytes\n", bytes_sent, bytes_read);
        }
    }

    gettimeofday(&end, NULL);

    close(fdFile);
    close(sockfd);
    elapsed_time = ((end.tv_sec - start.tv_sec) * 1000.0) + ((end.tv_usec - start.tv_usec) / 1000.0);
    printf("%.2ld\n", elapsed_time);
}

void ipv6_tcp(char *ip_address, int port)
{
    sleep(2);

     printf("ipv6_tcp,");

    struct timeval end, start;
    int sockfd, fdFile ;//,nbytes;
    struct sockaddr_in6 servaddr_ipv6;
    struct stat file_stat;
    unsigned char *data, checksum[SHA256_DIGEST_LENGTH];//,buffer[1024];
    size_t data_size = CHUNK_SIZE;
    long elapsed_time;

    // Open the file for reading
    fdFile = open("file.txt", O_RDONLY);
    if (fdFile < 0)
    {
        perror("open");
        exit(EXIT_FAILURE);
    }
    if (fstat(fdFile, &file_stat) == -1)
    {
        fprintf(stderr, "Failed to stat file\n");
        exit(1);
    }
    data_size = file_stat.st_size;
    // allocate memory for data buffer
    data = (unsigned char *)malloc(data_size);
    generate_data(data, data_size);
     // read the file contents
    ssize_t bytes_read = read(fdFile, data, data_size);
    if (bytes_read == -1)
    {
        fprintf(stderr, "Failed to read file\n");
        exit(1);
    }
    // calculate checksum of the file contents
    calculate_checksum(data, bytes_read, checksum);

    // Create a socket for the client
    sockfd = socket(AF_INET6, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Set up the server address
    memset(&servaddr_ipv6, 0, sizeof(servaddr_ipv6));
    servaddr_ipv6.sin6_family = AF_INET6;
    servaddr_ipv6.sin6_port = htons(port);
    
    if (inet_pton(AF_INET6, MY_SERVER_IP, &servaddr_ipv6.sin6_addr) <= 0)
    {
        perror("inet_pton");
        exit(EXIT_FAILURE);
    }

    // Connect to the server
    if (connect(sockfd, (struct sockaddr *)&servaddr_ipv6, sizeof(servaddr_ipv6)) < 0)
    {
        perror("connect");
        exit(EXIT_FAILURE);
    }
    // Start timer
    gettimeofday(&start, NULL);

    if (write(sockfd, data, bytes_read) == -1)
    {
        fprintf(stderr, "Failed to write to socket\n");
        exit(1);
    }
    // Calculate elapsed time
    gettimeofday(&end, NULL);
    elapsed_time = ((end.tv_sec - start.tv_sec) * 1000.0) + ((end.tv_usec - start.tv_usec) / 1000.0);

    // Calculate elapsed time
    gettimeofday(&end, NULL);
    elapsed_time = ((end.tv_sec - start.tv_sec) * 1000.0) + ((end.tv_usec - start.tv_usec) / 1000.0);
    printf("%.2ld\n", elapsed_time);
    // print the calculated checksum
    printf("Checksum of the file: ");
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        printf("%02x", checksum[i]);
    }
    printf("\n");

    
    close(sockfd);
    close(fdFile);
    // free memory
    free(data);
    
}

void ipv6_udp(char *ip_address, int port)
{
    struct timeval start, end;
    long elapsed_time;
    int sockfd, fdFile, bytes_sent, bytes_read;
    struct sockaddr_in6 serv_addr;
    struct stat file_stat;
    char bufData[1024];

    printf("ipv6_udp: ");
    // Open the file for reading
    fdFile = open("file.txt", O_RDONLY);
    if (fdFile == -1)
    {
        printf("Failed to open file\n");
        exit(1);
    }

    if (fstat(fdFile, &file_stat) == -1)
    {
        fprintf(stderr, "Failed to stat file\n");
        exit(1);
    }

    sockfd = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd < 0)
    {
        perror("Failed to create socket");
        exit(1);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin6_family = AF_INET6;
    serv_addr.sin6_addr = in6addr_any;
    serv_addr.sin6_port = htons(port);

    // start time
    gettimeofday(&start, NULL);

    while ((bytes_read = read(fdFile, bufData, sizeof(bufData))) > 0)
    {
        bytes_sent = sendto(sockfd, bufData, bytes_read, 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

        if (bytes_sent < 0)
        {
            fprintf(stderr, "Failed to read data\n");
            exit(1);
        }
        else if (bytes_sent != bytes_read)
        {
            fprintf(stderr, "sendto: Sent %d bytes instead of %d bytes\n", bytes_sent, bytes_read);
        }
    }
    gettimeofday(&end, NULL);
    
    close(fdFile);
    close(sockfd);
    elapsed_time = ((end.tv_sec - start.tv_sec) * 1000.0) + ((end.tv_usec - start.tv_usec) / 1000.0);
    printf("%.2ld\n", elapsed_time);
}

void uds_dgram()
{

     printf("uds_dgram,");
    struct timeval start, end;
    long elapsed_time;
        char dataa[1024];
    int nread;

    struct sockaddr_un servaddr_un = {
        .sun_family = AF_UNIX,
        .sun_path = SOCK_PATH_C};

    int sockfd= socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sockfd  == -1)
    {
        perror("socket");
        exit(1);
    }

    int fdFile;
    if ((fdFile = open("file.txt", O_RDONLY)) < 0)
    {
        perror("open");
        exit(1);
    }

    off_t file_size = lseek(fdFile, 0, SEEK_END);
    lseek(fdFile, 0, SEEK_SET);
    /*start time*/
    gettimeofday(&start, NULL); 
    if (sendto(sockfd, &file_size, sizeof(file_size), 0, (struct sockaddr *)&servaddr_un, sizeof(servaddr_un)) < 0)
    {
        perror("sendto");
        exit(1);
    }

    while ((nread = read(fdFile, dataa, sizeof(dataa))) > 0)
    {
        if (sendto(sockfd, dataa, nread, 0, (struct sockaddr *)&servaddr_un, sizeof(servaddr_un)) < 0)
        {
            perror("sendto");
            exit(1);
        }
    } 
    gettimeofday(&end, NULL);
    elapsed_time = ((end.tv_sec - start.tv_sec) * 1000.0) + ((end.tv_usec - start.tv_usec) / 1000.0);
    printf("%.2ld\n", elapsed_time);
    /*end time*/
    close(fdFile);
    close(sockfd);
    

}

void uds_stream()
{   

     printf("uds_stream,");
    struct timeval start, end;
    char dataa[1024];
    int len;
    int fdFile;
    int totalbyte = 0, countbyte;
    struct sockaddr_un servaddr_un = {
        .sun_family = AF_UNIX,
        .sun_path = SOCK_PATH_C,
    };
    
    int sockfd=  socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("socket");
        exit(1);
    }
    len = strlen(servaddr_un.sun_path) + sizeof(servaddr_un.sun_family);
    if (connect(sockfd, (struct sockaddr *)&servaddr_un, len) == -1)
    {
        perror("connect");
        exit(1);
    }

    //Open file   
    if ((fdFile = open("file.txt", O_RDONLY)) < 0)
    {
        perror("open 719");
        exit(1);
    }
    int file_size = lseek(fdFile, 0, SEEK_END);
    lseek(fdFile, 0, SEEK_SET);

    gettimeofday(&start, NULL); 
    if (send(sockfd, &file_size, sizeof(file_size), 0) < 0)
    {
        perror("send");
        exit(1);
    }

    while (totalbyte < file_size)
    {
        countbyte = read(fdFile, dataa, sizeof(dataa));
        if (countbyte < 0)
        {
            perror("read");
            exit(1);
        }

        if (send(sockfd, dataa, countbyte, 0) < 0)
        {
            perror("send");
            exit(1);
        }

        totalbyte += countbyte;
    }
    gettimeofday(&end, NULL);
    long elapsed_time = ((end.tv_sec - start.tv_sec) * 1000.0) + ((end.tv_usec - start.tv_usec) / 1000.0);
    printf("%.2ld\n", elapsed_time);
    close(fdFile);
    close(sockfd);
}

void mmap_Func(char *ip_address, int port)
{
     printf("mmap_Func,");
    sleep(2);
    const size_t chunk_size = CHUNK_SIZE;
    char *data;
    int fdFile;
    struct timeval end, start;
    long elapsed_time;

    static char generate_data[CHUNK_SIZE] = "Init"; // declare and initialize generate_data

    fdFile = open("File.txt", O_RDWR | O_CREAT | O_TRUNC, (mode_t)0600);
    if (fdFile == -1)
    {
        perror("open");
        exit(1);
    }

    if (ftruncate(fdFile, chunk_size) == -1)
    {
        perror("ftruncate");
        exit(1);
    }

    gettimeofday(&start, NULL);

    data = (char *)mmap(NULL, chunk_size, PROT_WRITE, MAP_SHARED, fdFile, 0);
    if (data == MAP_FAILED)
    {
        perror("mmap");
        exit(1);
    }

    memcpy(data, generate_data, chunk_size);

    if (munmap(data, chunk_size) == -1)
    {
        perror("munmap");
        exit(1);
    }

    gettimeofday(&end, NULL);
    elapsed_time = ((end.tv_sec - start.tv_sec) * 1000.0) + ((end.tv_usec - start.tv_usec) / 1000.0);
    printf("%.2ld\n", elapsed_time);
    printf("Data written successfully. Total bytes written: %zu\n", chunk_size);
    close(fdFile);
}

void pipe_Func()
{
     printf("pipe_Func,");
    int fdPipe, fdFile;
    ssize_t bytesRead, bytesWritten;
    char buffer[1024];
    size_t totalBytesWritten = 0;
    int p_flag = 1;
    struct timeval end, start;
    long elapsed_time;

    // Open the named pipe for writing
    fdPipe = open(FIFO_FILE, O_WRONLY);
    if (fdPipe == -1)
    {
        if (p_flag <= 1)
            perror("Failed to open named pipe");
        exit(EXIT_FAILURE);
    }

    // Open the file to read the data
    fdFile = open("file.txt", O_RDONLY);
    if (fdFile == -1)
    {
        if (p_flag <= 1)
            perror("Failed to open file for reading");
        exit(EXIT_FAILURE);
    }
    gettimeofday(&start, NULL);
    // Read data from the file and write it to the named pipe
    while ((bytesRead = read(fdFile, buffer, 1024)) > 0)
    {
        bytesWritten = write(fdPipe, buffer, bytesRead);
        if (bytesWritten != bytesRead)
        {
            if (p_flag <= 1)
                perror("Failed to write to named pipe");
            exit(EXIT_FAILURE);
        }
        totalBytesWritten += bytesWritten;
    }

    if (bytesRead == -1)
    {
        if (p_flag <= 1)
            perror("Failed to read from file");
        exit(EXIT_FAILURE);
    }

    
    
    gettimeofday(&end, NULL);
    elapsed_time = ((end.tv_sec - start.tv_sec) * 1000.0) + ((end.tv_usec - start.tv_usec) / 1000.0);
    printf("Time of pipe:%.2zd\n", elapsed_time);

    printf("Data sent successfully. Total bytes sent: %zu\n", totalBytesWritten);

    // Close the file and named pipe
    close(fdFile);
    close(fdPipe);
}


void sendFile(char *type, char *param, char *ip_address, int port)
{
    if (strcmp(type, "ipv4") == 0 && (strcmp(param, "tcp")) == 0)
    {
        // printf( "ipv4_tcp\n");
        ipv4_tcp(ip_address, port);
    }
    else if (strcmp(type, "ipv4") == 0 && (strcmp(param, "udp")) == 0)
    {
        ipv4_udp(ip_address, port);
    }
    else if (strcmp(type, "ipv6") == 0 && (strcmp(param, "tcp")) == 0)
    {
        ipv6_tcp(ip_address, port);
    }
    else if (strcmp(type, "ipv6") == 0 && (strcmp(param, "udp")) == 0)
    {
        ipv6_udp(ip_address, port);
    }
    else if (strcmp(type, "uds") == 0 && (strcmp(param, "dgram")) == 0)
    {
        uds_dgram();
    }
    else if (strcmp(type, "uds") == 0 && (strcmp(param, "stream")) == 0)
    {
        uds_stream();
    }
    else if (strcmp(type, "mmap") == 0 )//&& (strcmp(param, "filename")) == 0)
    {
        mmap_Func(ip_address, port);
    }
    else if (strcmp(type, "pipe") == 0)// && (strcmp(param, "filename")) == 0)
    {
        // printf("pipe\n");
        pipe_Func();
    }
}

int client_partB(int argc, char *argv[])
{
    if (argc != 7)
    {
        fprintf(stderr, "Usage: %s -c IP PORT -p TYPE PARAM\n", argv[0]);
        exit(1);
    }
    create_file();

    char SERVER_ADDRESS[1024];
    int port;
    strcpy(SERVER_ADDRESS, argv[2]);
    port = atoi(argv[3]);

    char type[1024];
    char param[1024];
    strcpy(type, argv[5]);
    strcpy(param, argv[6]);



    // Create a socket for the client
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("socket() failed");
        exit(EXIT_FAILURE);
    }
    
    // Set up the address of the server to connect to
    struct sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port+1);
    if (inet_pton(AF_INET, SERVER_ADDRESS, &server_addr.sin_addr) <= 0) {
        perror("inet_pton() failed");
        exit(EXIT_FAILURE);
    }
    
    // Connect to the server
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect() failed");
        exit(EXIT_FAILURE);
    }
    

    printf("type: %s\n", type);
    if (send(client_socket, type, strlen(type), 0) == -1) {
        perror("send() failed");
        exit(EXIT_FAILURE);
    }
    sleep(2);
    printf("param: %s\n", param);
 
    if (send(client_socket, param, strlen(param), 0) == -1) {
        perror("send() failed");
        exit(EXIT_FAILURE);
    }
    
    // Close the socket and exit
    close(client_socket);
    sleep(1);
     sendFile(type, param, SERVER_ADDRESS,  port);

    return 0;

}
