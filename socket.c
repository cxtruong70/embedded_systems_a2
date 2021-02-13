#include <stdio.h>      // for printf
#include <stdlib.h>     // memset, malloc, free, exit(0)
#include <sys/socket.h>	//for socket ofcourse
#include <string.h>     //for strncmp
#include <time.h>       // for srand(time(NULL))
#include <pthread.h>    // for pthreads
#include <stdbool.h>    // for bool
#include <string.h>     //memset
#include <errno.h>      //For errno - the error number
#include <netinet/udp.h>//Provides declarations for udp header
#include <netinet/ip.h>	//Provides declarations for ip header
#include "sorter.h"

#define FALSE 0
#define TRUE 1
#define PORT_NUM 12346
#define PACKET_SIZE 1500

static char returnMessage[PACKET_SIZE];

static void *listenPort(void *ptr);
static void processMessageRc(char *message);
static bool startsWith(const char* pre, const char*str);
// static void nullTerminateStr(*char);

void Socket_startListening(void);
void Socket_stopListening(void);

static bool stopFlag = FALSE;
pthread_t listeningThread;
pthread_mutex_t checkStopMutex;
pthread_mutex_t messageRcMutex;
static int socketDescriptor = 0;
// static int remotePortNum = 0;
// static char remoteHostName[PACKET_SIZE];

static void *listenPort(void *ptr)
{      
    // Initialize local socket
	struct sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;                   // Connection may be from network
	sin.sin_addr.s_addr = htonl(INADDR_ANY);    // Host to Network long
	sin.sin_port = htons(PORT_NUM);             // Host to Network short
	unsigned int sin_len = sizeof(sin);

    // Create the socket for UDP
	if((socketDescriptor = socket(PF_INET, SOCK_DGRAM, 0)) == -1){
		printf("ERROR: Socket descritptor creation failed\n");
		exit(-1);
	}

	// Bind the socket to 12345
	if(bind (socketDescriptor, (struct sockaddr*) &sin, sizeof(sin)) == -1){
		printf("ERROR: Bind to socket failed, socket in use\n");
		exit(-1);
	}

    while (1) {
        // Clear return message for next message
        memset(&returnMessage, 0, PACKET_SIZE);

        // Initialize buffer for message
		char buffer[PACKET_SIZE];
        memset(&buffer, 0, PACKET_SIZE);

		// Receive data from bound port; Blocking call
		int bytesRecieved = recvfrom(socketDescriptor,
			buffer, PACKET_SIZE, 0,
			(struct sockaddr *) &sin, &sin_len);
		if (bytesRecieved < 0){
			printf("ERROR: recvfrom failed\n");
		}
		// printf("%d\n", bytes_recieved);

		// Ensure string null terminated
		// Reference: workshop tutorial w/ prof. Brian Fraser
		int terminateIdx = (bytesRecieved < PACKET_SIZE) 
			? bytesRecieved : PACKET_SIZE - 1;
		buffer[terminateIdx] = '\0';

        // Process the recieved message
        processMessageRc(buffer);

        // Check stop flag before re-running
        // the loop and blocking
        pthread_mutex_lock(&checkStopMutex);
        if (stopFlag){
            pthread_mutex_unlock(&checkStopMutex);
            break;
        }
        pthread_mutex_unlock(&checkStopMutex);
	}


    // Background thread for listening to the 
    // port and processing responses based
    // on recieved input
    // while (1) {


    //     // Check stop flag
    //     pthread_mutex_lock(&checkStopMutex);
    //     if(stopFlag == true){
    //         // If stop flag is true, the thread will return,
    //         // and these mutexes are to be released
    //         // to allow the thread to start again later
    //         pthread_mutex_unlock(&checkStopMutex);
    //         return 0;
    //     }
    //     pthread_mutex_unlock(&checkStopMutex);
    // }

    return 0;
}

static void processMessageRc(char *message){
    const char HELP[6]  = "help\0";
    const char COUNT[6] = "count\0";
    const char GET[6]   = "get\0";
    const char STOP[6]  = "stop\0";
    long long numSorted = 0;
    int switchFlag = 0;

    if(startsWith(HELP, message))   switchFlag = 1;
    if(startsWith(COUNT, message))  switchFlag = 2;
    if(startsWith(GET, message))    switchFlag = 3;
    if(startsWith(STOP, message))   switchFlag = 4;

    switch (switchFlag){
        case 1 :
            strcpy(returnMessage, "Accepted command examples:\ncount      -- display number of arrays sorted.\nget length -- display length of array currently being sorted.\nget array  -- display the full array being sorted.\nstop       -- cause the server program to end");
            printf("%s\n", returnMessage);
            break;
        case 2 :
            numSorted = Sorter_getNumberArraysSorted();
            printf("Number of arrays sorted = %lld.\n", numSorted);
            break;
        case 3 :
            printf("get\n");
            // implement get array, get #, get length
            break;
        case 4 :
            pthread_mutex_lock(&checkStopMutex);
            stopFlag = TRUE;
            pthread_mutex_unlock(&checkStopMutex);
            printf("aights stoppin\n");
            break;
        default :
            strcpy(returnMessage,"Invalid input");
            printf("%s\n",returnMessage);
    }
}

// Reference: https://stackoverflow.com/questions/4770985/how-to-check-if-a-string-starts-with-another-string-in-c
static bool startsWith(const char *pre, const char *str)
{
    size_t lenpre = strlen(pre),
           lenstr = strlen(str);
    return lenstr < lenpre ? false : memcmp(pre, str, lenpre) == 0;
}

void Socket_startListening(void){
    
    // Default message, palceholder 
    // to start the thread
    char *message = "Threadz";

    // Reset the stopping flag before 
    // entering the thread and sorting loop
    pthread_mutex_lock(&checkStopMutex);
    stopFlag = FALSE;
    pthread_mutex_unlock(&checkStopMutex);

    // Start the thread and check for errors
    int returnCode = pthread_create(&listeningThread, NULL, listenPort, message);
    if (returnCode){                          
        printf("ERROR; return code from pthread_create() is %d\n", returnCode);                            
        exit(-1);                          
    }  
    printf("thread started\n");
}

void Socket_stopListening(void)
{   
    // Signal the flag to stop 
    // the background thread from
    // listening to the port
    pthread_join(listeningThread, NULL);
    printf("thread joined\n");

}

