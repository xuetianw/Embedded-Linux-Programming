#include <stdio.h>
#include <pthread.h>
#include "udp_listen.h"

int main() {

//    A2D_init();

//    pot_start_reading();
    UdpListener_startListening();

    printf("Hello, World!\n");

    Udp_cleanup();
//    por_cleanup();

    static unsigned long long int *s_pPrimes = NULL;
    unsigned long long int *pNewArray = malloc(sizeof(*pNewArray) * 2);

    return 0;
}