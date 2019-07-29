// Demo application to read analog input voltage 0 on the BeagleBone
// Assumes ADC cape already loaded:
//
//root@beaglebone:/# echo BB-ADC > /sys/devices/platform/bone_capemgr/slots
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <pthread.h>
#include <zconf.h>
#include "a2d.h"
#include "sorter.h"


int A2D_reading[10] = {0, 500, 1000, 1500, 2000, 2500, 3000, 3500, 4000, 4100};
int arraysize[10] = {1, 20, 60, 120, 250, 300, 500, 800, 1200, 2100};


#define A2D_FILE_VOLTAGE0 "/sys/bus/iio/devices/iio:device0/in_voltage0_raw"
#define A2D_VOLTAGE_REF_V 1.8
#define A2D_MAX_READING 4095


static pthread_t pot_id;
static int stopping = 0;

void stop_a2d() {
    stopping = 1;
}


int process_voltage(double reading);

void write_to_file(char* fileName, char* write_value) {
    FILE* file = fopen(fileName, "w");
    if (file == NULL) {
        printf("ERROR OPENING %s.", fileName);
    }

    int charWritten = fprintf(file, "%s", write_value);

    if (charWritten <= 0) {
        printf("ERROR WRITING INTO FILE");
    }
    fclose(file);
}


int getVoltage0Reading()
{
// Open file
    FILE *f = fopen(A2D_FILE_VOLTAGE0, "r");
    if (!f) {
        printf("ERROR: Unable to open voltage input file. Cape loaded?\n");
        printf("try: echo BB-ADC > /sys/devices/platform/bone_capemgr/slots\n");
        exit(-1);
    }
// Get reading
    int a2dReading = 0;
    int itemsRead = fscanf(f, "%d", &a2dReading);
    if (itemsRead <= 0) {
        printf("ERROR: Unable to read values from voltage input file.\n");
        exit(-1);
    }
// Close file
    fclose(f);

    return a2dReading;

}



void *pot_thread()
{
    while (!stopping) {
        int reading = getVoltage0Reading();
//        double voltage = ((double)reading / A2D_MAX_READING) * A2D_VOLTAGE_REF_V;
//        printf("Value %5d ==> %5.2fV\n", reading, voltage);
        int new_arraysize = process_voltage(reading);
        printf("set new_arraysize: %d\n", new_arraysize);
        Sorter_setArraySize(new_arraysize);
        sleep(1);
    }
    printf("a2d pot_thread terminated\n");
    return NULL;
}


int process_voltage(double reading) {
    for (int i = 0; i < 9; i ++) {
        int left_read = A2D_reading[i];
        int right_read = A2D_reading[i + 1];
        if (left_read < reading && reading < right_read) {
            int left_size = arraysize[i];
            int right_size = arraysize[i + 1];
            double slope = (double) (right_size - left_size) / (right_read - left_read);
            int result = left_size + slope * (reading - A2D_reading[i]);
            return result;
        }
    }
    return 1;
}

void pot_start_reading ()
{
    pthread_create(&pot_id, NULL, pot_thread, NULL);
}

void A2D_init()
{
    write_to_file("/sys/devices/platform/bone_capemgr/slots", "BB-ADC");
}

void pot_cleanup()
{
    pthread_join(pot_id, NULL);
}