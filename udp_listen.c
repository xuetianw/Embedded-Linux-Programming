/*
 * UDP Listening program on port 22110
 * By Brian Fraser, Modified from Linux Programming Unleashed (book)
 *
 * Usage:
 *	On the target, run this program (netListenTest).
 *	On the host:
 *		> netcat -u 192.168.0.171 22110
 *		(Change the IP address to your board)
 *
 *	On the host, type in a number and press enter:
 *		4<ENTER>
 *
 *	On the target, you'll see a debug message:
 *	    Message received (2 bytes):
 *	    '4
 *	    '
 *
 *	On the host, you'll see the message:
 *	    Math: 4 + 1 = 5
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>			// for strncmp()
#include <unistd.h>			// for close()
#include <pthread.h>
#include "udp_listen.h"

#define MSG_MAX_LEN 1024
#define PORT		22110

pthread_t udp_id;
void *udp_thread();
void process_message(char *message);


void UdpListener_startListening()
{
    pthread_create(&udp_id, NULL, udp_thread, NULL);
}

void Udp_cleanup()
{
    pthread_join(udp_id, NULL);
}

void *udp_thread()
{
	printf("Fred's Net Listen Test on UDP port %d:\n", PORT);
	printf("Connect using: \n");
	printf("    netcat -u 127.0.0.1 %d\n", PORT);

	// Buffer to hold packet data:
	char message[MSG_MAX_LEN];

	// Address
	struct sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;                   // Connection may be from network
	sin.sin_addr.s_addr = htonl(INADDR_ANY);    // Host to Network long
	sin.sin_port = htons(PORT);                 // Host to Network short
	
	// Create the socket for UDP
	int socketDescriptor = socket(PF_INET, SOCK_DGRAM, 0);

	// Bind the socket to the port (PORT) that we specify
	bind (socketDescriptor, (struct sockaddr*) &sin, sizeof(sin));
	
	while (1) {
		// Get the data (blocking)
		// Will change sin (the address) to be the address of the client.
		// Note: sin passes information in and out of call!
		unsigned int sin_len = sizeof(sin);
		int bytesRx = recvfrom(socketDescriptor, message, MSG_MAX_LEN, 0,
				(struct sockaddr *) &sin, &sin_len);

		// Make it null terminated (so string functions work):
		// NOTE: Unsafe in some cases; why?
		message[bytesRx] = 0;
//		printf("Message received (%d bytes): \n\n'%s'\n", bytesRx, message);
		process_message(message);

		// Extract the value from the message:
		// (Process the message any way your app requires).
//		int incMe = atoi(message);

		// Compose the reply message (re-using the same buffer here):
		// (NOTE: watch for buffer overflows!).
//		sprintf(message, "Math: %d + 1 = %d\n", incMe, incMe + 1);

		// Transmit a reply:
		sin_len = sizeof(sin);
		sendto( socketDescriptor,
				message, strlen(message),
				0,
				(struct sockaddr *) &sin, sin_len);
	}

	// Close
	close(socketDescriptor);
	
	return 0;
}

void process_message(char *message) {
    printf("%s\n", message);
    if(strcmp(message, "help\n")==0)
    {
        sprintf(message, "%s", "Accepted command examples:\n"
               "count      -- display number arrays sorted.\n"
               "get length -- display length of array currently being sorted.\n"
               "get array  -- display the full array being sorted.\n"
               "get 10     -- display the tenth element of array currently being sorted.\n"
               "stop       -- cause the server program to end.\n");
    }
}
