#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>			// for strncmp()
#include <unistd.h>			// for close()
#include <pthread.h>
#include "udp_listen.h"
#include "a2d.h"
#include "I2C.h"
#include "sorter.h"
#include "change_digits.h"

#define MSG_MAX_LEN 10240
#define PORT		22110

pthread_t udp_id;
void *udp_thread();
void process_message(char *message, struct sockaddr_in* sin);
static int stopping = 0;

void UdpListener_startListening()
{
    pthread_create(&udp_id, NULL, udp_thread, NULL);
}

void Udp_cleanup()
{
    pthread_join(udp_id, NULL);
}

int socketDescriptor;
unsigned int sin_len;

void send_message(const char* message, struct sockaddr_in* sin);

void *udp_thread()
{
	printf("Fred's Net Listen Test on UDP port %d:\n", PORT);
	printf("Connect using: \n");
//	printf("    netcat -u 127.0.0.1 %d\n", PORT);
    printf("    netcat -u 192.168.7.2 %d\n", PORT);

    // Buffer to hold packet data:
	char message[MSG_MAX_LEN];

	// Address
	struct sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;                   // Connection may be from network
	sin.sin_addr.s_addr = htonl(INADDR_ANY);    // Host to Network long
	sin.sin_port = htons(PORT);                 // Host to Network short
	
	// Create the socket for UDP
	socketDescriptor = socket(PF_INET, SOCK_DGRAM, 0);

	// Bind the socket to the port (PORT) that we specify
	bind (socketDescriptor, (struct sockaddr*) &sin, sizeof(sin));
	
	while (!stopping) {
		// Get the data (blocking)
		// Will change sin (the address) to be the address of the client.
		// Note: sin passes information in and out of call!
		sin_len = sizeof(sin);
		int bytesRx = recvfrom(socketDescriptor, message, MSG_MAX_LEN, 0,
				(struct sockaddr *) &sin, &sin_len);

		// Make it null terminated (so string functions work):
		message[bytesRx] = 0;

		process_message(message, &sin);

		// Transmit a reply:
        send_message(message, &sin);
    }
    printf("udp_thread terminated\n");

	// Close
	close(socketDescriptor);
	
	return 0;
}

void send_message(const char* message, struct sockaddr_in* sin) {
    sendto(socketDescriptor,
           message, strlen(message),
           0,
           (struct sockaddr*) sin, sin_len);
}

void process_message(char *message, struct sockaddr_in* sin) {
    printf("%s\n", message);
    if(strcmp(message, "help\n") == 0)
    {
        sprintf(message, "%s", "Accepted command examples:\n"
                               "count      -- display number arrays sorted.\n"
                               "get length -- display length of arr currently being sorted.\n"
                               "get arr    -- display the full arr being sorted.\n"
                               "get 10     -- display the tenth element of arr currently being sorted.\n"
                               "stop       -- cause the server program to end.\n");
    } else if (strcmp(message, "count\n") == 0) {
        sprintf(message, "Number of arrays sorted = %lld\n", Sorter_getNumberArraysSorted());
    }  else if (strcmp(message, "get length\n") == 0) {
        sprintf(message, "Current array length = %d\n", Sorter_getArrayLength());
    } else if (strcmp(message, "get arr\n") == 0) {
        int array_length;
        int *array = Sorter_getArrayData(&array_length);
        int count = 0;
        for (int i = 0; i < array_length; i++) {
            char result[1];
            if ( (i == (array_length)) || ((count != 0) && (count % 10 == 9))) {
                strcat(message, "\n");
            }
            sprintf(result, "%d", array[count]);
            strcat(message, result);
            strcat(message, ", ");
            count++;
            if (i % 300 == 0) {
                send_message(message, sin);
                memset( message, '\0', strlen(message) );
            }
        }

        strcat(message, "\n");

        free(array);
    } else if (strcmp(message, "stop\n") == 0) {
        stopping = 1;
        stop_a2d();
        stop_I2C();
        Sorter_stopSorting();
        stop_change_digits();
    } else {
        if (strlen(message) >= strlen("get #\n")) {
            char subbuff[strlen(message) + 1];
            memcpy(subbuff, message, 4);
            subbuff[3] = '\0';
            if(strcmp(subbuff, "get") == 0) {
                memcpy(subbuff, &message[4], strlen(message)- strlen("get "));
                subbuff[strlen(message)- strlen("get ")] = '\0';
                printf("%s\n", subbuff);
                char* end;
                long requested_index = strtol(subbuff, &end, 0);
                printf("requested_index :%ld\n", requested_index);
                int array_length;
                int *array = Sorter_getArrayData(&array_length);
                if (requested_index < 0 || requested_index > array_length) {
                    sprintf(message, "Invalid argument. Must be between 1 and %d (# found).\n",
                            array_length);
                } else {
                    sprintf(message, "Value %ld = %d\n", requested_index, array[requested_index]);
                }
                free(array);
            }
        }
    }
}
