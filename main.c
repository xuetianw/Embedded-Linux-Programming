#include <stdio.h>
#include <pthread.h>
#include "udp_listen.h"
#include "a2d.h"
#include "I2C.h"
#include <stdlib.h>

int main() {

//    A2D_init();
    I2C_init();

//    pot_start_reading();
    I2C_start_display();
    UdpListener_startListening();

    printf("Hello, World!\n");

    Udp_cleanup();
    I2C_display_cleanup();
//    por_cleanup();


    return 0;
}