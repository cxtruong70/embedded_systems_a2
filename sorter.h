// sorter.h
// Module to spawn a separate thread to sort random arrays
// (permutations) on a background thread. It provides access to the 
// contents of the current (potentially partially sorted) array,
// and to the count of the total number of arrays sorted

#ifndef _SORTER_H_
#define _SORTER_H_

// Begin/end the background thread which sorts random permutations
void Sorter_startSorting(void);
void Sorter_stopSorting(void);

// Get the size of the array currently being sorted.
// Set the size of the nezt array to sort (don't change current array)
int Sorter_getArraySize(void);
void Sorter_setArraySize(int newSize);

// Get a copy of the current (potentially partially sorted) array.
// Returns a newly allocated array and sets "length" to be the 
// number of elements in the rerturned array (output-only parameter).
// The calling code must call free() on the returned pointer.
int* Sorter_getArrayData(int *length);

// Get the number of arrays which ahve finished being sorted.
long long Sorter_getNumberArraysSorted(void);

#endif