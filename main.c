#include <string.h>
#include "Chat.h"
#include "Client.h"
#include "Server.h"

// void generate_data(unsigned char *data, size_t size)
// {
//     if (RAND_bytes(data, size) != 1)
//     {
//         fprintf(stderr, "Failed to generate random data\n");
//         exit(1);
//     }
// }

// void calculate_checksum(unsigned char *data, size_t size, unsigned char *checksum)
// {
//     SHA256(data, size, checksum);
// }
int main(int argc, char *argv[])
{

    if (argc < 3 || argc > 7)
    {
        printf("miss parameter\n");
        exit(1);
    }
    if (strcmp(argv[1], "-c") != 0 && strcmp(argv[1], "-s") != 0)
    {
        printf("Client/Server usage: %s -c [IP_PORT] -p [TYPE]\n", argv[0]);
        exit(1);
    }


    // Check if the program is being run as a client or a server
    switch (argv[1][1])
    {
    case 'c':
    // If the "-p" flag is present in the arguments, execute client_partB
        if (argc >= 5 && strcmp(argv[4], "-p") == 0)
        {
            client_partB(argc, argv);
        }
        else
        {
            // Otherwise, execute client_chat
            client_chat(argc, argv);
        }
        break;
    case 's':
        // If the "-p" flag is present in the arguments, execute server_partB
        if (argc >= 4 && strcmp(argv[3], "-p") == 0)
        {
            server_partB(argc, argv);
        }
        else
        {
            // Otherwise, execute server_chat
            server_chat(argc, argv);
        }
        break;
    default:
        // Invalid argument, handle error as necessary
        break;
    }

    return 0;
}
