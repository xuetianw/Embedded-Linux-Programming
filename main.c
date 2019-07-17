#include <stdio.h>
#include <pthread.h>
#include "udp_listen.h"

int main() {



    UdpListener_startListening();

    printf("Hello, World!\n");

    Udp_cleanup();
    return 0;
}