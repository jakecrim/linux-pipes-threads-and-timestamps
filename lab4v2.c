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
#include <pthread.h>

// declarations
void * interpolateData(void * arg);

/* Defines */
#define BILLION 1000000000L
#define MILLISECONDS 1000000

/* Variables */
// pos 0 = time , pos 1 = data
u_int64_t before_time_and_data[2];
u_int64_t button_press_time;
u_int64_t after_time_and_data[2];

// get the 'after' button press time stamp
bool getAfterTS = false;


// prints interpolated value and timestamps related to GPS stuff
void * printResults(void * arg )
{
	while(1)
	{
		printf("Printing Thread waiting to print... \n");

        // read from pipe, info for printing

		usleep(10000000);
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
    // open /tmp/BP_Pipe and store into button press descriptor
    // int bpd = open("tmp/BP_pipe", O_RDONLY);

    while(1)
    {
        // Read from button press descriptor
        // read(bpd,someOtherStruct, 1);
        // THIS TIME WILL COME THROUGH THE PIPE ONCE THATS WORKING

		// IF button pressed, spawn child thread to handle
		// just to test for now, 
		if(buttonPressed == 5)
		{
            printf("Button Pressed: \n");

            // signal that the timestamp after the button press needs retrieved
            getAfterTS = true;
            clock_gettime(CLOCK_MONOTONIC_RAW, &bpTime);
            button_press_time = bpTime.tv_nsec;
			pthread_create( &threadID, NULL, interpolateData, NULL);
			pthread_join(threadID, NULL);
            buttonPressed = 0;
		}

		buttonPressed++;
        // Wait until next GPS
        usleep(1000000);
        // do Interpolation
    }

}

void * interpolateData(void * arg )
{
    u_int64_t timeDiff = 0;
    // how long after the last GPS data was the button pressed
    u_int64_t pressAfterTime = 0;
	// get after_time_and_data info	

    // do nothing until the after timestamp flag is setback to false, meaning the timestamp has been retrieved
    // and interpolation can be done
    while(getAfterTS)
    {
        usleep(5);
    }
	// interpolate
	printf("Interpolating... \n");
    printf("Before time: %lu ns \n", before_time_and_data[0]);
    printf("After  time: %lu ns \n", after_time_and_data[0]);
	timeDiff = after_time_and_data[0] - before_time_and_data[0];	
	printf("Difference between timestamps: %llu ms \n",(long long unsigned int) timeDiff / 1000000);
    pressAfterTime = button_press_time - before_time_and_data[0];
	printf("Button pressed %llu ms after last GPS data-point. \n",(long long unsigned int) pressAfterTime / 1000000);
	

}

int main(void)
{
    char gpsBuffer;
    u_int32_t timeDiff = 0;
	struct timespec before, after;

    //testing
    int interrupt = 0;
	int gps_data = 0;

    // mkfifo("/tmp/BP_pipe", 777); // returns 0 if successful?

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
        // read(gpsd, &gpsBuffer, 1);

        /* Create timestamp for each read */
        if(getAfterTS == false)
        {
            clock_gettime(CLOCK_MONOTONIC_RAW, &before);
            before_time_and_data[0] = before.tv_nsec;
            before_time_and_data[1] = gps_data;
        }
        else
        {
            clock_gettime(CLOCK_MONOTONIC_RAW, &after);
            after_time_and_data[0] = after.tv_nsec;
            after_time_and_data[1] = gps_data;
            getAfterTS = false;
        }
        
        // printf("Before: Stamp %lu | Data %lu \n", before_time_and_data[0], before_time_and_data[1]);

        // wait 250 ms
        usleep(250000);
        gps_data++; // for testing
    }

	// joining all 3 threads
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