//
// Created by fred on 2020-01-06.
//

#include "change_digits.h"
#include "sorter.h"
#include "Utility.h"
#include <stdio.h>

#include <pthread.h>

static int left_digit = 0;
static int right_digit = 0;

static int stopping = 0;

int previous_sort_total = 0;
int sort_total;
int sorted_lastsecond;
void *change_digits();

pthread_t display_id;

int get_left_digit() {
    return left_digit;
}

int get_right_digit() {
    return right_digit;
}

void stop_change_digits() {
    stopping = 1;
}

void Start_changing_digit() {
    pthread_create(&display_id, NULL, change_digits, NULL);
}


void *change_digits() {
    while (!stopping) {
        second_timing();
        sort_total = Sorter_getNumberArraysSorted();
        sorted_lastsecond = sort_total - previous_sort_total;
        previous_sort_total = sort_total;
        if (sorted_lastsecond >= 99) {
            left_digit = 9;
            right_digit = 9;
        } else {
            left_digit = sorted_lastsecond / 10;
            right_digit = sorted_lastsecond % 10;
        }
        printf("previous_sort_total %d \n", sorted_lastsecond);
    }
    printf("change_digits terminated\n");
    return NULL;
}


void change_display_cleanup() {
    pthread_join(display_id, NULL);
}
