#include <stdio.h>
#include <pthread.h>
#include "udp_listen.h"
#include "a2d.h"
#include "I2C.h"
#include "sorter.h"
#include <stdlib.h>

int main() {

    A2D_init();
    I2C_init();

    pot_start_reading();
    I2C_start_display();
    UdpListener_startListening();
    Sorter_startSorting();


    Udp_cleanup();
    I2C_display_cleanup();
    pot_cleanup();
    Sorter_cleanup();
    return 0;
}