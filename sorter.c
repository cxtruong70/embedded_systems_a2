#include <stdio.h>      // for printf
#include <stdlib.h>     // memset, malloc, free
#include <time.h>       // for srand(time(NULL))
#include <pthread.h>    // for pthreads
#include <stdbool.h>    // for bool

#define INITIAL_ARR_SIZE 100;
#define FALSE 0
#define TRUE 1

// static void delay(int seconds, long nanoseconds);
static int *arr;

// Internal helper functions
static void *sortArrays(void *ptr);
static void initArray(int *arr);
static void shuffleArray(int *arr);
static void swap(int *a, int *b);
static void bubbleSort(int *arr);
static void incrementCountSorted(void);

// Exposed interface functions
void Sorter_startSorting(void);
void Sorter_stopSorting(void);
void Sorter_setArraySize(int size);
int Sorter_getArraySize(void);
int* Sorter_getArrayData(int *length);
long long Sorter_getNumberArraysSorted(void);

// Prototypes for useful development functions
static void delay(int seconds, long nanoseconds);
// static void printArray(int *arr);

static int inputArrSize = INITIAL_ARR_SIZE; // Sort size input, a separate variable to avoid critical section
static int internalArrSize = INITIAL_ARR_SIZE; // Sort size variable to edit, once sort loop finishes
static long long countSorted = 0;
static bool stopFlag = FALSE;
static bool randInit = FALSE;
pthread_t sortThread;
pthread_mutex_t checkStopMutex;
pthread_mutex_t inputSizeMutex;
pthread_mutex_t countMutex;
pthread_mutex_t arrayAccessMutex;
pthread_mutex_t internalSizeMutex;

/*
    Internal functions
*/

static void *sortArrays(void *ptr)
{      
    // Lock array access until the array has been
    // initialized and filled with values to
    // avoid returning garbage to exposed data 
    // methods
    pthread_mutex_lock(&arrayAccessMutex);
    while (1) {
        // Lock inputArrSize variable as it
        // is read here and externally accessed
        pthread_mutex_lock(&inputSizeMutex);
        internalArrSize = inputArrSize; // Set new, if any, size of array
        pthread_mutex_unlock(&inputSizeMutex);

        // Unlock access to the array as it is 
        // now populated and ready to read
        pthread_mutex_unlock(&arrayAccessMutex);

        // Allocate specified array size to fill and sort
        arr = (int*)malloc(internalArrSize*sizeof(int));
        initArray(arr);
        shuffleArray(arr);
        // printArray(arr);

        // Sort shuffled array
        bubbleSort(arr);
        // printArray(arr);

        // Lock access to the array - it should not
        // be accessed when "freed" and unlock
        // when it is allocated and filled again
        // in the next loop
        pthread_mutex_lock(&arrayAccessMutex);
        free(arr);

        incrementCountSorted();
        
        // Check stop flag
        pthread_mutex_lock(&checkStopMutex);
        if(stopFlag){
            // If stop flag is TRUE, the thread will return,
            // and these mutexes are to be released
            // to allow the thread to start again later
            pthread_mutex_unlock(&checkStopMutex);
            pthread_mutex_unlock(&arrayAccessMutex);
            return 0;
        }
        pthread_mutex_unlock(&checkStopMutex);
    }

    return 0;
}

static void initArray(int *arr)
{
    pthread_mutex_lock(&arrayAccessMutex);
    // Fills array with enumeration of index,
    // from 1,2,...,n to allow for permutation
    for(int i = 0; i < internalArrSize; i++){
        arr[i] = i+1;
    }
    pthread_mutex_unlock(&arrayAccessMutex);
}   

static void shuffleArray(int *arr)
{   
    // Permutes the array
    for(int i = 0; i < internalArrSize; i++){
        int idxRange = internalArrSize-i;
        int j = rand() % idxRange;
        int swapIdx = j + i;
        swap(&arr[i], &arr[swapIdx]);
    }
}

static void swap(int *a, int *b)
{   
    // Swap two values between integers
    pthread_mutex_lock(&arrayAccessMutex);
    delay(0,100);
    int temp = *a;
    *a = *b;
    *b = temp;
    pthread_mutex_unlock(&arrayAccessMutex);
}

static void bubbleSort(int *arr)
{
    // Sorts largest value to end,
    // then sorts next largest value to n-1
    for(int i = 0; i < internalArrSize-1; i++){
        for (int j = 0; j < internalArrSize-1-i; j++)
            if (arr[j] > arr[j+1]){
                swap(&arr[j], &arr[j+1]);
            }
    }
}

void incrementCountSorted(void){
    // Increment the number of arrays sorted
    pthread_mutex_lock(&countMutex);
    countSorted = countSorted + 1;
    pthread_mutex_unlock(&countMutex);
}


/*
    Externally exposed interface functions
*/

void Sorter_startSorting(void){
    // Start the random seed if it has
    // not already been started
    if (!randInit) srand(time(NULL));
    randInit = TRUE;
    
    // Default message, palceholder 
    // to start the thread
    char *message = "Threadz";

    // Reset the stopping flag before 
    // entering the thread and sorting loop
    pthread_mutex_lock(&checkStopMutex);
    stopFlag = FALSE;
    pthread_mutex_unlock(&checkStopMutex);

    // Start the thread and check for errors
    int returnCode = pthread_create(&sortThread, NULL, sortArrays, message);
    if (returnCode){                          
        printf("ERROR; return code from pthread_create() is %d\n", returnCode);                            
        exit(-1);                          
    }  
    printf("thread started\n");
}

void Sorter_stopSorting(void)
{   
    // Signal the flag to break out of the
    // sorting loop and wait for the
    // thread to stop and join
    pthread_mutex_lock(&checkStopMutex);
    stopFlag = TRUE;
    pthread_mutex_unlock(&checkStopMutex);
    pthread_join(sortThread, NULL);
    printf("thread joined\n");

    // Reset initial array size values
    // in case the sorting loop is initiated
    // again in the same run of the program
    internalArrSize = INITIAL_ARR_SIZE;
    inputArrSize = INITIAL_ARR_SIZE;
}

void Sorter_setArraySize(int size)
{   
    // Set the array size of subsequent
    // arrays to sort
    pthread_mutex_lock(&inputSizeMutex);
    inputArrSize = size;
    pthread_mutex_unlock(&inputSizeMutex);
}


int Sorter_getArraySize(void){
    // Return current array size
    pthread_mutex_lock(&inputSizeMutex);
    int temp = inputArrSize;
    pthread_mutex_unlock(&inputSizeMutex);
    return temp;
}

int *Sorter_getArrayData(int *length)
{
    // Allocate and return a full array
    // in its current state, sorted or not
    pthread_mutex_lock(&arrayAccessMutex);
    pthread_mutex_lock(&inputSizeMutex);
    *length = *length < internalArrSize ? *length : internalArrSize;
    int *temp = (int*)malloc(*length*sizeof(int));
    for (int i = 0; i < *length; i++){
        temp[i] = arr[i];
    }
    pthread_mutex_unlock(&inputSizeMutex);
    pthread_mutex_unlock(&arrayAccessMutex);
    return temp;
}

long long Sorter_getNumberArraysSorted(void){
    // Return the number of arrays sorted
    long long temp;
    pthread_mutex_lock(&countMutex);
    temp = countSorted;
    pthread_mutex_unlock(&countMutex);
    return temp;
}

/*
    Useful functions for development
*/

// static void printArray(int *arr)
// {   
//     // Print array in one line
//     pthread_mutex_lock(&arrayAccessMutex);
//     for(int i = 0; i < internalArrSize; i++){
//         printf("%d ",arr[i]);
//     }
//     pthread_mutex_unlock(&arrayAccessMutex);
//     printf("\n");
// }

static void delay(int seconds, long nanoseconds)
{
	struct timespec reqDelay = {seconds, nanoseconds};
	nanosleep(&reqDelay, (struct timespec *) NULL);
}
