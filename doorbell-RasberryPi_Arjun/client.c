#include "client.h"

// Creates a TCP socket and connects it to the specified host and port. It
// returns the socket file descriptor.
int client_connect(Config config)
{
        // Sets up structs for address info of specified hostname/port pair
    printf("Connecting to TCP server...\n");
    struct addrinfo hints, *result, *rp;
    int sockfd = -1;
    int status;
    printf("Initialized addrinfo structs\n");

    // Sets the IP version to unspecified to be version neutral and the socket type to TCP
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;     // Either IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP
    printf("hints stuff successfully assigned\n");

    // Checks to see if there is correct address info for hostname/port pair
    if ((status = getaddrinfo(config.host, config.port, &hints, &result)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        exit(EXIT_FAILURE);
    }
    printf("status of getaddrinfo: %d\n", status);
    printf("Successfully obtained host IP address\n");

    // Loops through linked list pairs of info to establish a valid socket
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);

        // Socket incorrect
        if (sockfd < 0) {
            printf(strerror(ENOTCONN));
            continue;
        }

        // Checks to see if connection is valid
        if (connect(sockfd, rp->ai_addr, rp->ai_addrlen) != -1) {
            printf("connection established\n");
            break;
        } else {
            printf(strerror(ETIMEDOUT));
            close(sockfd);
        }
    }

    freeaddrinfo(result); // free the linked list
    if (sockfd == -1) {
        printf("could not establish connection with TCP server\n");
        exit(EXIT_FAILURE);
    }
    return sockfd;
}

// Send youur image to the server
// int sockfd: Socket file descriptor number
// uint8_t * img: img data
// char * hw_id: homework id personal identifier
// size_t payload_size: size of message to send
void client_send_image(int sockfd, uint8_t * img, char * hw_id, size_t payload_size)
{
	printf("inside client_send_image\n");
	int leftover = 0;
    	/* PLACE CODE HERE */
	uint8_t *client_data= (uint8_t *)malloc(sizeof(uint8_t)*payload_size+9);
	memcpy(client_data,hw_id,9);
	printf("client_data%s\n",client_data);
	printf("hw_id:%s\n",client_data);
	memcpy(client_data+9,img,payload_size);
	//leftover = send(sockfd,client_data,payload_size,0);
	
	//for(int i = 0; i < 20; i++)
	//	printf("%x", client_data[i]);

	printf("bitmap:\n%s\n",client_data);
	int total_bytes_sent=0;
	printf("image:\n%s\n",img);
	leftover = payload_size+9;
	int x= 0;
	printf("payload_size: %d,\n",payload_size);
	while(total_bytes_sent<payload_size+9){
		//total_bytes_sent += leftover;
		x = send(sockfd,client_data+total_bytes_sent,leftover,0);
		printf("total_bytes_sent: %d\n",total_bytes_sent);
		if(x==-1){
			printf("breaks here\n");
			break;
		}
		total_bytes_sent += x;
		leftover -= x;
		printf("total_bytes_sent: %d\n",total_bytes_sent);
	}
	free(client_data);
}


// Close the socket when your program is done running.
void client_close(int sockfd)
{
    	/* PLACE CODE HERE */
	close(sockfd);

}
