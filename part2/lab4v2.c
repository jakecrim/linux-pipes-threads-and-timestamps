/* lab4v2.c */
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sched.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>
#include <semaphore.h>

// declarations
void * interpolateData(void * arg);

/* Defines */
#define BILLION 1000000000L
#define MILLISECONDS 1000000

/* Variables */
// pos 0 = time , pos 1 = data
u_int32_t before_time_and_data[2];
u_int32_t button_press_time;
u_int32_t after_time_and_data[2];

// pos 0 = initial button press, pos 1 = final timestamp after button press 
u_int32_t interpolateInfoArr[2] = {0};
float greatestInterpolatedValue = 0;

// get the 'after' button press time stamp
bool getAfterTS = false;
bool printFlag = false;
// useful for waiting until no interpolateThreads are still active
int interpolateThreadsActive = 0;

sem_t threadActiveLock;




// prints interpolated value and timestamps related to GPS stuff
void * printResults(void * arg )
{
    float prevInterpolatedCheck = 0;
	while(1)
	{
        sem_wait(&threadActiveLock);
        if(printFlag)
        {
            printFlag = false;
            printf("Interpolated Value %0.4f \n", greatestInterpolatedValue);
        }
        sem_post(&threadActiveLock);

		usleep(100);
	}
}

// buttonPress pipe handling thread
void * readBP_Pipe(void * arg )
{
	struct timespec bpTime, after;
	int gps_data = 22;
	pthread_t threadID;

    // test variable until pipes are working
	int buttonPressed = 0;
    struct timeval bpTime2;
    // open /tmp/BP_Pipe and store into button press descriptor
    int bpd = open("/tmp/BP_pipe", O_RDWR);

    int test = 0;
    u_int32_t ulTest = 0;

    while(1)
    {
        // Read from button press descriptor
        printf("\n-----------------------------------------\n");
        read(bpd, &button_press_time, 4);
        button_press_time = (button_press_time / 1000);
        // printf("ms: %lu \n", button_press_time);

        // signal that the timestamp after the button press needs retrieved
        getAfterTS = true;

        // spawn child thread
        pthread_create( &threadID, NULL, interpolateData, NULL);
        pthread_join(threadID, NULL);


        printf("Thread Done \n");
        printf("\n-----------------------------------------\n");
    }

}

void * interpolateData(void * arg )
{
    printf("--- NEW THREAD SPAWNED --- \n");

    // do nothing until the after timestamp flag is setback to false, meaning the timestamp has been retrieved
    // and interpolation can be done
    while(getAfterTS)
    {
        usleep(5);
    }

    // save timestamps & data in the current scope
    u_int32_t pressAfterTime = 0;
    float interpolatedData = 0;
    float percentage = 0;
    u_int32_t bpTimestamp = button_press_time;  // how long after the last GPS data was the button pressed
    u_int32_t beforeTimestamp = before_time_and_data[0];
    float beforeData = (float)before_time_and_data[1];
    u_int32_t afterTimestamp = after_time_and_data[0];
    float afterData = (float)after_time_and_data[1];

    // handle edge cases where the timer resets at 999 ms so adjustments are needed
    if(beforeTimestamp > bpTimestamp)
    {
        pressAfterTime = (1000 - beforeTimestamp) + bpTimestamp;
        bpTimestamp = 1000 + bpTimestamp;
    }
    else
    {
        pressAfterTime = bpTimestamp - beforeTimestamp;
    }
    if(afterTimestamp < bpTimestamp)
    {
       afterTimestamp = 1000 + afterTimestamp; 
    }

    // interpolate
    percentage = (float) pressAfterTime / (float) 250;
    interpolatedData = percentage*(afterData - beforeData) + beforeData;
    // printf("Interpolated Value: %0.4f \n", interpolatedData);

    // take sem
    sem_wait(&threadActiveLock);
        /* Adjusting Global interpolate info array */
        if(beforeTimestamp < interpolateInfoArr[0])
            interpolateInfoArr[0] = beforeTimestamp;
        if(afterTimestamp > interpolateInfoArr[1])
            interpolateInfoArr[1] = afterTimestamp;
        if(interpolatedData > greatestInterpolatedValue)
        {
            greatestInterpolatedValue = interpolatedData;
            printFlag = true;
        }
    // release sem
    sem_post(&threadActiveLock);
}

int main(void)
{
    char gpsBuffer;
    int gpsBufferVal;
    u_int32_t timeDiff = 0;
	// struct timespec before, after;
    struct timeval before, after, temp;

    //testing
    int interrupt = 0;
	int gps_data = 0;

    /* initializing everything */
    sem_init(&threadActiveLock, 0, 1);
    
    // open /tmp/N_pipe1 and store into GPS descriptor
    int gpsd = open("/tmp/N_pipe1", O_RDONLY );

    // create thread to reap BP pipe
	int threadCount = 2;
	pthread_t threadID[threadCount];

	pthread_create( &threadID[0], NULL, readBP_Pipe, NULL);	
	pthread_create( &threadID[1], NULL, printResults, NULL);	

    while(1)
    {

        /* read from /tmp/N_pipe1 */
        read(gpsd, &gpsBuffer, 1);
        /* Create timestamp for each read */
        gettimeofday(&temp, NULL);
        if(getAfterTS == false)
        {
            before_time_and_data[0] = (temp.tv_usec / 1000);
            before_time_and_data[1] = gpsBuffer;
        }
        else //button press has happened and we need the time of the next GPS message
        {
            after_time_and_data[0] = (temp.tv_usec / 1000); // time index
            after_time_and_data[1] = (int)gpsBuffer; //data index 
            getAfterTS = false; // done getting the timestamp that happened AFTER button press
        }
        
    }

	// joining all threads
	for(int i = 0; i < threadCount; i++)
		pthread_join(threadID[i], NULL);

    return 0;

}

	/* OLD CODE FOR LATER USE */

    // while(1)
    // {

    //         /* read from /tmp/N_pipe1 */
    //     // read(gpsd, &gpsBuffer, 1);

    //     /* Create timestamp for each read */
	// 	clock_gettime(CLOCK_MONOTONIC_RAW, &before);
    //     printf("Before Stamp: %lu \n", before.tv_nsec);


    //     if(interrupt == 2)
    //     {
	// 	    clock_gettime(CLOCK_MONOTONIC_RAW, &after);
    //         printf("After Stamp:  %lu \n", after.tv_nsec);

	// 	    timeDiff = (after.tv_sec - before.tv_sec) * 1000000 + (after.tv_nsec - before.tv_nsec);	
    // 		printf("Difference between timestamps: %llu ns \n",(long long unsigned int) timeDiff);

    //         interrupt = 0;
    //     }


    //     // wait 250 ms?
    //     usleep(250000);
    //     interrupt++;
    // }