/* lab4v2.c */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sched.h>
#include <time.h>

#define BILLION 1000000000L

// read buttonPress pipe
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
	// not sure if we have this library installed
    u_int32_t timeDiff = 0;
	struct timespec before, after;

    // mkfifo("/tmp/BP_pipe", 777); // returns 0 if successful?

    // open /tmp/N_pipe1 and store into GPS descriptor
    int gpsd = open("/tmp/N_pipe1", O_RDONLY );



    // create thread to reap BP pipe


    while(1)
    {

        // read from /tmp/N_pipe1
        read(gpsd, &gpsBuffer, 1);
        // create timeStamp (can use gettimeofday() look up example)
		clock_gettime(CLOCK_MONOTONIC, &before);
		printf("TESTING STUFF BETWEEN CLOCK READS \n");
		clock_gettime(CLOCK_MONOTONIC, &after);

		timeDiff = BILLION * (after.tv_sec - before.tv_sec) + after.tv_nsec - before.tv_nsec;	
		printf("Difference between timestamps: %llu ns \n",(long long unsigned int) timeDiff);

        // wait 250 ms?
        usleep(250000);
    }


    return 0;
}