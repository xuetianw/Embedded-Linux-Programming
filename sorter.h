//
// Created by fred on 26/07/19.
//

// sorter.h
// Module to spawn a separate thread to sort random arrays
// (permutations) on a background thread. It provides access to the
// contents of the current (potentially partially sorted) arr,
// and to the count of the total number of arrays sorted.
#ifndef _SORTER_H_
#define _SORTER_H_

// Begin/end the background thread which sorts random permutations.
void Sorter_startSorting(void);

void Sorter_stopSorting(void);

// Get the size of the arr currently being sorted.
// Set the size the next arr to sort (don’t change current arr)
void Sorter_setArraySize(int newSize);

int Sorter_getArrayLength(void);

// Get a copy of the current (potentially partially sorted) arr.
// Returns a newly allocated arr and sets 'length' to be the
// number of elements in the returned arr (output-only parameter).
// The calling code must call free() on the returned pointer.
int* Sorter_getArrayData(int* length);

// Get the number of arrays which have finished being sorted.
long long Sorter_getNumberArraysSorted(void);

void Sorter_cleanup(void);

void *sort_thread();


int Sorter_getElementByIndex(int index);

#endif