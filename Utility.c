//
// Created by fred on 2020-01-06.
//

#include <time.h>
#include "Utility.h"

void second_timing() {//    printf("Timing test\n");
    long seconds = 1;
    long nanoseconds = 0;
    struct timespec reqDelay = {seconds, nanoseconds};
    nanosleep(&reqDelay, (struct timespec*) NULL);
}