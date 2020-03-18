//
// Created by fred on 26/07/19.
//

#include "sorter.h"
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <zconf.h>

static int* arr;
int array_size = 100;
long long sorted_total;
pthread_t sorter_id;
int static stopping = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex3 = PTHREAD_MUTEX_INITIALIZER;

int actual_size = 0;


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
    sorted_total++;
}

void static permutate_array()
{
    srand(time(NULL));
    for (int i = 0; i < 1000; i++) {
        int index1 = rand() % array_size;
        int index2 = rand() % array_size;
        int temp = arr[index1];
        arr[index1] = arr[index2];
        arr[index2] = temp;
    }
}

void* sort_thread()
{
    while (!stopping) {
        pthread_mutex_lock(&mutex2);

        free(arr);
        pthread_mutex_lock(&mutex);

        arr = malloc(sizeof(*arr) * array_size);
        actual_size = array_size;

        for (int i = 0; i < array_size; i++) {
            arr[i] = i + 1;
        }

        pthread_mutex_unlock(&mutex2);

        permutate_array();
        bubbleSort();

        pthread_mutex_unlock(&mutex);

    }
    free(arr);
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
    pthread_mutex_lock(&mutex);
    {
        array_size = newSize;
    }
    pthread_mutex_unlock(&mutex);
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
    pthread_mutex_lock(&mutex2);
    int* copy = malloc(sizeof(*copy) * actual_size);
    {
        for (int i = 0; i < actual_size; i++) {
            copy[i] = arr[i];
        }
        *length = actual_size;
    }
    pthread_mutex_unlock(&mutex2);
    return copy;
}

// Get the number of arrays which have finished being sorted.
long long Sorter_getNumberArraysSorted(void)
{
    return sorted_total;
}

