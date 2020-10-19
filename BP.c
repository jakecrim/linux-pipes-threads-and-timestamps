#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <semaphore.h>
#include <sched.h>
#include <pthread.h>
#include <time.h>


// Defines
#define GPEDS_OFFSET 0x10 // change to real offset later

unsigned long * gpsel0;
unsigned long * gpeds0;

int main(void)
{
    struct sched_param param1;
	struct timespec bpTime;

    int bpfd = open("/tmp/BP_pipe", O_WRONLY);

    // getting access to rPi registers for reading gpeds0
    int fd = open("/dev/mem", O_RDWR | O_SYNC);
    if(fd < 0)
    {
        printf("Error Opening Linux File \n");
        return 0;
    }
    // offset for GPEDS ...
    gpsel0 = (unsigned long *)mmap(NULL, getpagesize(), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0x3F200000);
    
    gpeds0 = gpsel0 + GPEDS_OFFSET; 
    // clearing gpeds reg
    *(gpeds0) = 0x00010000

	// sched stuff
    // period of 60 ms 
    param1.sched_priority = 10; 
    sched_setscheduler(0, SCHED_FIFO , &param1);
    //initialize the timer (lab2 pt2)
        // set clock init clock_gettime();

    while(1)
    {
        if(*(gpdes0) != 0))
        {
            // Create timeStamp
			clock_gettime(CLOCK_MONOTONIC_RAW, &bpTime);

            // write to P2 through pipe (/tmp/BP_Pipe)
			write(bpfd, bpTime.tv_nsec, 4);

            // clearing gpeds reg
            *(gpeds0) = 0x00010000
        }

		usleep(60000);
    }
}