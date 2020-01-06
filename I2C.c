//
// Created by fred on 14/07/19.
//
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include "I2C.h"
#include "sorter.h"
#include "change_digits.h"


#define I2CDRV_LINUX_BUS0 "/dev/i2c-0"
#define I2CDRV_LINUX_BUS1 "/dev/i2c-1"
#define I2CDRV_LINUX_BUS2 "/dev/i2c-2"

#define EXPORT_PATH "/sys/class/gpio/export"

void second_timing();

static int initI2cBus(char* bus, int address)
{
    int i2cFileDesc = open(bus, O_RDWR);
    if (i2cFileDesc < 0) {
        printf("I2C: Unable to open bus for read/write (%s)\n", bus);
        perror("Error is:");
        exit(1);
    }
    int result = ioctl(i2cFileDesc, I2C_SLAVE, address);
    if (result < 0) {
        perror("I2C: Unable to set I2C device to slave address.");
        exit(1);
    }
    return i2cFileDesc;
}


//The following function allows the program to write to an I2C device's register:
static void writeI2cReg(int i2cFileDesc, unsigned char regAddr,
                        unsigned char value)
{
    unsigned char buff[2];
    buff[0] = regAddr;
    buff[1] = value;
    int res = write(i2cFileDesc, buff, 2);
    if (res != 2) {
        perror("I2C: Unable to write i2c register.");
        exit(1);
    }
}

//The following function allows the program to read from an I2C device's register:
static unsigned char readI2cReg(int i2cFileDesc, unsigned char regAddr)
{
// To read a register, must first write the address
    int res = write(i2cFileDesc, &regAddr, sizeof(regAddr));
    if (res != sizeof(regAddr)) {
        perror("I2C: Unable to write to i2c register.");
        exit(1);
    }
// Now read the value and return it
    char value = 0;
    res = read(i2cFileDesc, &value, sizeof(value));
    if (res != sizeof(value)) {
        perror("I2C: Unable to read from i2c register");
        exit(1);
    }
    return value;
}



void export_gpio(int pin) {// Use fopen() to open the file for write access.
    FILE* pfile = fopen(EXPORT_PATH, "w");
    if (pfile == NULL) {
        printf("ERROR: Unable to open export file.\n");
        exit(1);
    }
    // Write to data to the file using fprintf():
    fprintf(pfile, "%d", pin);
    fclose(pfile);
}


int FileIODrv_echo_to_file(char* file, char* buff)
{
    // Open direction file
    int file_desc = open(file, O_WRONLY);
    if (file_desc < 0) {
        printf("FILEIODRV ERROR: Unable to open file for write: %s\n", file);
        perror("Error is:");
        exit(-1);
    }

    int bytes_written = write(file_desc, buff, strlen(buff));
//    die_on_failed("Unable to write to file for echo.");

    // Close
    close(file_desc);

    return bytes_written;
}


#define I2C_DEVICE_ADDRESS 0x20

#define REG_DIRA 0x00
#define REG_DIRB 0x01
#define REG_OUTA 0x14
#define REG_OUTB 0x15

#define ZERO_REG_A 0xA1
#define ZERO_REG_B 0x86
#define ONE_REG_A 0x24
#define ONE_REG_B 0x24
#define TWO_REG_A 0x31
#define TWO_REG_B 0x0E
#define THREE_REG_A 0xB0
#define THREE_REG_B 0x0E
#define FOUR_REG_A 0x90
#define FOUR_REG_B 0x8A
#define FIVE_REG_A 0xB0
#define FIVE_REG_B 0x8C
#define SIX_REG_A 0xB1
#define SIX_REG_B 0x88
#define SEVIN_REG_A 0x80
#define SEVIN_REG_B 0x07
#define EIGHT_REG_A 0xB1
#define EIGHT_REG_B 0x8E
#define NIGHT_REG_A 0x90
#define NIGHT_REG_B 0x8E

static int stopping = 0;



void turn_on_left();

void turn_on_right();

void stop_I2C() {
    stopping = 1;
}

void i2c_cleanup();
void *i2c_display_thread();

void display_number();

int i2cFileDesc;

void turn_off_right();
void turn_off_left();
void timing();

pthread_t display_id;




void I2C_init() {
    export_gpio(61);
    export_gpio(44);
    FileIODrv_echo_to_file("/sys/class/gpio/gpio61/direction", "out");
    FileIODrv_echo_to_file("/sys/class/gpio/gpio44/direction", "out");
    turn_on_left();
    turn_on_right();


    FileIODrv_echo_to_file("/sys/devices/platform/bone_capemgr/slots", "BB-I2C1");
    printf("Drive display (assumes GPIO #61 and #44 are output and 1\n");
    i2cFileDesc = initI2cBus(I2CDRV_LINUX_BUS1, I2C_DEVICE_ADDRESS);
    writeI2cReg(i2cFileDesc, REG_DIRA, 0x00);
    writeI2cReg(i2cFileDesc, REG_DIRB, 0x00);
}

void I2C_start_display()
{
    pthread_create(&display_id, NULL, i2c_display_thread, NULL);
}

void I2C_display_cleanup()
{
    pthread_join(display_id, NULL);

}


void display_digits() {
    turn_on_left();
    turn_off_right();
    display_number(get_left_digit());
    timing();
    turn_off_left();
    turn_on_right();
    display_number(get_right_digit());
    timing();
}




void *i2c_display_thread() {
    // Drive an hour-glass looking character
// (Like an X with a bar on top & bottom)
    while (!stopping) {
//        printf("left dight: %d, right digit %d\n", left_digit, right_digit);
        display_digits();
//        unsigned char regVal = readI2cReg(i2cFileDesc, REG_OUTA);
//        printf("Reg OUT-A = 0x%02x\n", regVal);
    }
    turn_off_right();
    turn_off_left();
    printf("i2c_display_thread terminated\n");
    return NULL;
}

void display_number(int left) {
    switch (left) {
        case 0 :
            writeI2cReg(i2cFileDesc, REG_OUTA, ZERO_REG_A);
            writeI2cReg(i2cFileDesc, REG_OUTB, ZERO_REG_B);
            break;
        case 1:
            writeI2cReg(i2cFileDesc, REG_OUTA, ONE_REG_A);
            writeI2cReg(i2cFileDesc, REG_OUTB, ONE_REG_B);
            break;
        case 2:
            writeI2cReg(i2cFileDesc, REG_OUTA, TWO_REG_A);
            writeI2cReg(i2cFileDesc, REG_OUTB, TWO_REG_B);
            break;
        case 3:
            writeI2cReg(i2cFileDesc, REG_OUTA, THREE_REG_A);
            writeI2cReg(i2cFileDesc, REG_OUTB, THREE_REG_B);
            break;
        case 4:
            writeI2cReg(i2cFileDesc, REG_OUTA, FOUR_REG_A);
            writeI2cReg(i2cFileDesc, REG_OUTB, FOUR_REG_B);
            break;
        case 5:
            writeI2cReg(i2cFileDesc, REG_OUTA, FIVE_REG_A);
            writeI2cReg(i2cFileDesc, REG_OUTB, FIVE_REG_B);
            break;
        case 6:
            writeI2cReg(i2cFileDesc, REG_OUTA, SIX_REG_A);
            writeI2cReg(i2cFileDesc, REG_OUTB, SIX_REG_B);
            break;
        case 7:
            writeI2cReg(i2cFileDesc, REG_OUTA, SEVIN_REG_A);
            writeI2cReg(i2cFileDesc, REG_OUTB, SEVIN_REG_B);
            break;
        case 8:
            writeI2cReg(i2cFileDesc, REG_OUTA, EIGHT_REG_A);
            writeI2cReg(i2cFileDesc, REG_OUTB, EIGHT_REG_B);
            break;
        case 9:
            writeI2cReg(i2cFileDesc, REG_OUTA, NIGHT_REG_A);
            writeI2cReg(i2cFileDesc, REG_OUTB, NIGHT_REG_B);
            break;
        default:
            writeI2cReg(i2cFileDesc, REG_OUTA, 0x00);
            writeI2cReg(i2cFileDesc, REG_OUTB, 0x80);
    }
}

void turn_on_right() { FileIODrv_echo_to_file("/sys/class/gpio/gpio44/value", "1"); }

void turn_off_right() { FileIODrv_echo_to_file("/sys/class/gpio/gpio44/value", "0"); }

void turn_on_left() { FileIODrv_echo_to_file("/sys/class/gpio/gpio61/value", "1"); }

void turn_off_left() { FileIODrv_echo_to_file("/sys/class/gpio/gpio61/value", "0"); }


void timing() {//    printf("Timing test\n");
    long seconds = 0;
    long nanoseconds = 10000000;
    struct timespec reqDelay = {seconds, nanoseconds};
    nanosleep(&reqDelay, (struct timespec*) NULL);
}
