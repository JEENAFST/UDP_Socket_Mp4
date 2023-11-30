#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <unistd.h>

#include <arpa/inet.h>



#define MAX_BUFFER_SIZE 1024



void die(char *message) {

    perror(message);

    exit(1);

}



void receive_file(FILE *file, int socket, struct sockaddr_in client_address, socklen_t client_address_len) {

    char buffer[MAX_BUFFER_SIZE];

    ssize_t bytesRead;



    while (1) {

        bytesRead = recvfrom(socket, buffer, sizeof(buffer), 0, (struct sockaddr *)&client_address, &client_address_len);

        if (bytesRead == -1) {

            die("recvfrom() failed");

        }



        if (bytesRead == 0) {

            break;  // End of file reached

        }



        fwrite(buffer, 1, bytesRead, file);

    }

}



int main(int argc, char *argv[]) {

    if (argc != 3) {

        fprintf(stderr, "Usage: %s <output_filename> <listening_port>\n", argv[0]);

        exit(1);

    }



    char *output_filename = argv[1];

    int listening_port = atoi(argv[2]);



    FILE *file = fopen(output_filename, "wb");

    if (!file) {

        die("Unable to create output file");

    }



    // Create a UDP socket

    int udp_socket = socket(AF_INET, SOCK_DGRAM, 0);

    if (udp_socket == -1) {

        die("socket() failed");

    }



    // Set up the server address structure

    struct sockaddr_in server_address, client_address;

    memset(&server_address, 0, sizeof(server_address));

    server_address.sin_family = AF_INET;

    server_address.sin_addr.s_addr = htonl(INADDR_ANY);

    server_address.sin_port = htons(listening_port);



    // Bind the socket

    if (bind(udp_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {

        die("bind() failed");

    }



    // Receive the file

    receive_file(file, udp_socket, client_address, sizeof(client_address));



    fclose(file);

    close(udp_socket);



    return 0;

}

