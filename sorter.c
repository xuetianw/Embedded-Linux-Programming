//
// Created by fred on 26/07/19.
//

#include "sorter.h"
#include <stdlib.h>
#include <pthread.h>

int* p;
int array_size = 100;
long long unsigned sorted_total;
pthread_t sorter_id;
int stop = 0;


void init() {
    p = malloc(sizeof(p) * array_size);
}


void swap(int *xp, int *yp)
{
    int temp = *xp;
    *xp = *yp;
    *yp = temp;
}

// An optimized version of Bubble Sort
void bubbleSort(int arr[], int n)
{
    int i, j;
    int swapped;
    for (i = 0; i < n-1; i++)
    {
        swapped = 0;
        for (j = 0; j < n-i-1; j++)
        {
            if (arr[j] > arr[j+1])
            {
                swap(&arr[j], &arr[j+1]);
                swapped = 1;
            }
        }

        // IF no two elements were swapped by inner loop, then break
        if (swapped == 0)
            break;
    }
}

void permutate_array() {
    for (int i = 0; i < array_size/2; i++) {
        int temp = p[array_size-1];
        p[array_size-1] = p[i];
        p[i-1] = temp;
    }
}

void *sort_thread() {
    while (!stop) {

    }
}

void Sorter_startSorting(void) {
    pthread_create(&sorter_id, NULL, sort_thread, NULL);
}

void Sorter_stopSorting(void) {
    stop = 1;
}

// Get the size of the array currently being sorted.
// Set the size the next array to sort (don’t change current array)
void Sorter_setArraySize(int newSize) {
    array_size = newSize;
}

int Sorter_getArrayLength(void) {
    return array_size;
}

// Get a copy of the current (potentially partially sorted) array.
// Returns a newly allocated array and sets 'length' to be the
// number of elements in the returned array (output-only parameter).
// The calling code must call free() on the returned pointer.
int* Sorter_getArrayData(int* length) {

}

// Get the number of arrays which have finished being sorted.
long long Sorter_getNumberArraysSorted(void) {
    return sorted_total;
}