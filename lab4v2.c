/* lab4v2.c */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sched.h>
#include <time.h>

/* Defines */
#define BILLION 1000000000L
#define MILLISECONDS 1000000

/* Variables */
// pos 0 = time , pos 1 = data
u_int64_t before_time_and_data[2];


// prints interpolated value and timestamps related to GPS stuff
void * printResults(void * arg )
{
	while(1)
	{
		printf("Printing Thread waiting to print... \n");
		usleep(1000000);
	}
}

// buttonPress pipe handling thread
void * readBP_Pipe(void * arg )
{
    // open /tmp/BP_Pipe and store into button press descriptor
    // int bpd = open("tmp/BP_pipe", O_RDONLY);

    // if gpeds not zero? then 
    while(1)
    {
        printf("Read BP Thread Running: \n");
        // Read from button press descriptor
        // read(bpd,someOtherStruct, 1);
        // Wait until next GPS
        usleep(1000000);
        // do Interpolation
    }

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
		clock_gettime(CLOCK_MONOTONIC_RAW, &before);
		before_time_and_data[0] = before.tv_nsec;
		before_time_and_data[1] = gps_data;
        printf("Before: Stamp %lu | Data %lu \n", before_time_and_data[0], before_time_and_data[1]);

        // wait 250 ms?
        usleep(250000);
        gps_data++;
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