//
// Created by fred on 26/07/19.
//

#include "sorter.h"
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <zconf.h>

int* arr;
int array_size = 2100;
long long unsigned sorted_total;
pthread_t sorter_id;
int static stopping = 0;

//
//void init() {
//    arr = malloc(sizeof(arr) * array_size);
//}


void static swap(int* xp, int* yp)
{
    int temp = *xp;
    *xp = *yp;
    *yp = temp;
}

// An optimized version of Bubble Sort
void static bubbleSort()
{
    int i, j;
    int swapped;
    for (i = 0; i < array_size - 1; i++) {
        swapped = 0;
        for (j = 0; j < array_size - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                swap(&arr[j], &arr[j + 1]);
                swapped = 1;
            }
        }

        // IF no two elements were swapped by inner loop, then break
        if (swapped == 0)
            break;
    }
}

void static permutate_array()
{
    srand(time(NULL));
    for (int i = 0; i < 100; i++) {
        int index1 = rand() % array_size;
        int index2 = rand() % array_size;
        int temp = arr[array_size - 1];
        arr[array_size - 1] = arr[i];
        arr[i] = temp;
    }
}

void* sort_thread()
{
    while (!stopping) {
        arr = malloc(sizeof(arr) * array_size);
        for (int i = 0; i < array_size; i++) {
            arr[i] = i + 1;
        }
        permutate_array();
        bubbleSort();
        sorted_total++;
        free(arr);
//        printf("sorted_total: %lld\n", sorted_total);
    }
}

void Sorter_startSorting(void)
{
    pthread_create(&sorter_id, NULL, sort_thread, NULL);
}

void Sorter_stopSorting(void)
{
    stopping = 1;
}

void Sorter_cleanup(void)
{
    pthread_join(sorter_id, NULL);
}

// Get the size of the arr currently being sorted.
// Set the size the next arr to sort (don’t change current arr)
void Sorter_setArraySize(int newSize) {
    array_size = newSize;
}

int Sorter_getArrayLength(void)
{
    return array_size;
}

// Get a copy of the current (potentially partially sorted) arr.
// Returns a newly allocated arr and sets 'length' to be the
// number of elements in the returned arr (output-only parameter).
// The calling code must call free() on the returned pointer.
int* Sorter_getArrayData(int* length)
{

}

// Get the number of arrays which have finished being sorted.
long long Sorter_getNumberArraysSorted(void)
{
    return sorted_total;
}