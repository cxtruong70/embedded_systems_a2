#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <stdbool.h>
#include "sorter.h"
#include "socket.h"

static void delay(int seconds, long nanoseconds);
static void printArrayN(int *arr, int *length);
int testSorterComplex(void);
int testSorterSimple(void);

int testSocketSimple(void);


int main(){
    // testSorter();
    // testSorter2();
    // testSorter();
    // testSorter2();
    testSocketSimple();
    printf("returned\n");
    return 0;
}

static void delay(int seconds, long nanoseconds)
{
	struct timespec reqDelay = {seconds, nanoseconds};
	nanosleep(&reqDelay, (struct timespec *) NULL);
}

static void printArrayN(int *arr, int *length)
{   
    // Prints each array item
    int num = *length;
    for(int i = 0; i < num; i++){
        printf("R:%d ",arr[i]);
    }
    printf("\n");
}

int testSorterComplex(void){
    Sorter_startSorting();
    delay(1,0);
    Sorter_setArraySize(20);
    delay(1,0);
    Sorter_setArraySize(100);
    delay(1,0);
    Sorter_setArraySize(200);
    delay(1,0);
    Sorter_setArraySize(10);
    delay(1,0);
    int length = 15;    
    int *arr = Sorter_getArrayData(&length);

    printf("length returned: %d\n", length);
    printArrayN(arr, &length);

    int arrSize = Sorter_getArraySize();
    printf("Array Size: %d\n", arrSize);

    long long numSorted = Sorter_getNumberArraysSorted();
    printf("Arrays Sorted: %lld\n", numSorted);

    free(arr);
    delay(3,0);
    Sorter_stopSorting();
    printf("Thread returned\n");
    return 0;
}

int testSorterSimple(void){
    Sorter_startSorting();
    delay(1,0);
    Sorter_stopSorting();
    return 0;
}

int testSocketSimple(void){
    Sorter_startSorting();
    Socket_startListening();
    delay(1,0);
    Socket_stopListening();
    Sorter_stopSorting();
    return 0;
}