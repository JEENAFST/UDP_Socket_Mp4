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



void send_file(FILE *file, int socket, struct sockaddr_in server_address, socklen_t server_address_len) {

    char buffer[MAX_BUFFER_SIZE];

    size_t bytesRead;



    while ((bytesRead = fread(buffer, 1, sizeof(buffer), file)) > 0) {

        if (sendto(socket, buffer, bytesRead, 0, (struct sockaddr *)&server_address, server_address_len) == -1) {

            die("sendto() failed");

        }

    }



    // Send an empty packet to signal the end of the file

    if (sendto(socket, NULL, 0, 0, (struct sockaddr *)&server_address, server_address_len) == -1) {

        die("sendto() failed");

    }

}



int main(int argc, char *argv[]) {

    if (argc != 4) {

        fprintf(stderr, "Usage: %s <filename> <server_ip> <server_port>\n", argv[0]);

        exit(1);

    }



    char *filename = argv[1];

    char *server_ip = argv[2];

    int server_port = atoi(argv[3]);



    FILE *file = fopen(filename, "rb");

    if (!file) {

        die("File not found");

    }



    // Create a UDP socket

    int udp_socket = socket(AF_INET, SOCK_DGRAM, 0);

    if (udp_socket == -1) {

        die("socket() failed");

    }



    // Set up the server address structure

    struct sockaddr_in server_address;

    memset(&server_address, 0, sizeof(server_address));

    server_address.sin_family = AF_INET;

    server_address.sin_addr.s_addr = inet_addr(server_ip);

    server_address.sin_port = htons(server_port);



    // Send the file

    send_file(file, udp_socket, server_address, sizeof(server_address));



    fclose(file);

    close(udp_socket);



    return 0;

}

