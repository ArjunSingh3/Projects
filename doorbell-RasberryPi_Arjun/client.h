#ifndef CLIENT_H_
#define CLIENT_H_

#include <arpa/inet.h>
#include <errno.h>
#include <getopt.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

// Contains all of the information needed to create to connect to the server and
// send it a message.
typedef struct Config {
    char *port;
    char *host;
    uint8_t *payload;
    char *hw_id;
} Config;


// Creates a TCP socket and connects it to the specified host and port. It
// returns the socket file descriptor.
int client_connect(Config config);

// Using the the action and message provided by the command line, format the
// data to follow the protocol and send it to the server.
void client_send_image(int sockfd, uint8_t * img, char * hw_id, size_t payload_size);


// Close the socket when your program is done running.
void client_close(int sockfd);

#endif