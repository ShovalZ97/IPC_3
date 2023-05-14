#include "Chat.h"

int server_chat(int argc, char *argv[]) {
if (argc != 3) {
    fprintf(stderr, "Usage: %s -s PORT\n", argv[0]);
    exit(1);
}

int port = atoi(argv[2]);


    // Create socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket() error");
        exit(1);
    }

    // Bind socket to port
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1) {
        perror("bind() error");
        exit(1);
    }

    // Listen for incoming connections
    if (listen(server_fd, 5) == -1) {
        perror("listen() error");
        exit(1);
    }

    printf("Waiting for incoming connections...\n");

    // Accept incoming connections
    int client_fd;
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    char buffer[1024];


    struct pollfd pfds[2];


    

    client_fd = accept(server_fd, (struct sockaddr *) &client_addr, &client_addr_len);
    if(client_fd == -1) {
    printf("ERROR_ch on accept");
    exit(1);
    }

 


        printf("Client connected: %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        pfds[0].fd = STDIN_FILENO;
        pfds[0].events = POLLIN ;
        pfds[1].fd = client_fd;
        pfds[1].events = POLLIN ;
        // Loop to receive and send messages
        while (1) {
             memset(buffer, 0, BUFFER_SIZE);
            if (poll(pfds, 2, -1) < 0){
                printf("ERROR_ch on poll");
            }else{
            
            if (pfds[0].revents & POLLIN)
            {
                fgets(buffer, 1024, stdin);
                buffer[strlen(buffer)-1] = '\0'; // remove newline character

                // Send message to server
                if (send(client_fd, buffer, strlen(buffer), 0) == -1) {
                    perror("send() error");
                    exit(1);
                }
            }
            if ( pfds[1].revents & POLLIN)
            {
                int num_bytes = recv(client_fd, buffer, 1024, 0);
                if (num_bytes == -1) {
                    perror("recv() error");
                    exit(1);
                } else if (num_bytes == 0) {
                    printf("Connection closed by client.\n");
                    break;
                
                } else {
                    buffer[num_bytes] = '\0';
                    printf("Client: %s\n", buffer);
                }
            }            
            }//end else
        }//while end
        


        close(client_fd);
    

    close(server_fd);
    return 0;
}

int client_chat(int argc, char *argv[]) {
    // printf("\nargv[0] %s",argv[0]);
    // printf("\nargv[1] %s",argv[1]);
    // printf("\nargv[2] %s",argv[2]);
    // printf("\nargv[3] %s",argv[3]);
    //  printf("\nargv %d\n",argc);
  
if (argc != 4) {
    fprintf(stderr, "Usage: %s -c IP PORT\n", argv[0]);
    exit(1);
}

char *ip = argv[2];
int port = atoi(argv[3]);


    // Create socket
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd == -1) {
        perror("socket() error");
        exit(1);
    }

    // Connect to server
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip);
    server_addr.sin_port = htons(port);

    if (connect(sock_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1) {
        perror("connect() error");
        exit(1);
    }else{
        printf("connect server\n");
    }


    struct pollfd pfds[2]; //we want to monitor 2 socets

    pfds[0].fd = sock_fd; //the input is the sock
    pfds[0].events = POLLIN; // Tell me when ready to read
    pfds[1].fd = STDIN_FILENO;
    pfds[1].events = POLLIN; // Tell me when ready to read


    char buffer[1024];
    while (1) {
        memset(buffer, 0, BUFFER_SIZE);

        int num_events = poll(pfds, 2, -1); 

        if (num_events == 0) {
            printf("Poll timed out!\n");
        } else {

            if ( pfds[1].revents & POLLIN) {
                
                fgets(buffer, BUFFER_SIZE, stdin);
                buffer[strlen(buffer) - 1] = '\0';

                // Send message to server
                int bytes_sent = send(sock_fd, buffer, strlen(buffer), 0);
                if (bytes_sent < 0){
                    perror("send() error");
                    exit(1);
                }
            } 

            if (pfds[0].revents & POLLIN)
            {
                int num_bytes = recv(sock_fd, buffer, BUFFER_SIZE, 0);

                if (num_bytes < 0){
                    perror("recv() error");
                    exit(1);
                }else if (num_bytes == 0){
                    printf("Connection closed by server.\n");
                    break;
                }
                buffer[num_bytes] = '\0';/////////////////
                printf("Server: %s\n", buffer);
            }
        }
        
    }

    close(sock_fd);
    return 0;
}