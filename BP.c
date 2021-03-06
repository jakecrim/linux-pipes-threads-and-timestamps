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
#include <sys/time.h>


// Defines
#define GPEDS_OFFSET 0x10 // change to real offset later

unsigned long * gpsel0;
unsigned long * gpeds0;

int main(void)
{
    struct sched_param param1;
	struct timespec bpTime1;
    struct timeval bpTime2;

    int pipeFSD = mkfifo("/tmp/BP_pipe", 0777); // returns 0 if successful?
    if(pipeFSD <  0)
        printf("Making unsuccesful \n");
    else
        printf("Pipe made \n");

    int bpfd = open("/tmp/BP_pipe", O_RDWR);

    printf("test1 \n");

    // getting access to rPi registers for reading gpeds0
    int fd = open("/dev/mem", O_RDWR | O_SYNC);
    if(fd < 0)
    {
        printf("Error Opening Linux File \n");
        return 0;
    }
    printf("test2 \n");
    // offset for GPEDS ...
    gpsel0 = (unsigned long *)mmap(NULL, getpagesize(), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0x3F200000);
    
    gpeds0 = gpsel0 + GPEDS_OFFSET; 
    // clearing gpeds reg
    *(gpeds0) = 0x00010000;

	// sched stuff
    // period of 60 ms 
    param1.sched_priority = 10; 
    sched_setscheduler(0, SCHED_FIFO , &param1);
    //initialize the timer (lab2 pt2)
        // set clock init clock_gettime();
    
    int test = 5;

    while(1)
    {
        printf("GPEDS VALUE: 0x%08x \n", *(gpeds0));
        if(*(gpeds0) != 0)
        {
            // Create timeStamp
			clock_gettime(CLOCK_MONOTONIC_RAW, &bpTime1);
            gettimeofday(&bpTime2, NULL);
            printf("bp1 time: %lu us bp2 time: %lu \n", bpTime1.tv_sec , bpTime2.tv_sec);
            printf("Button Pressed \n");
            // // write to P2 through pipe (/tmp/BP_Pipe)
			write(bpfd, &bpTime2.tv_usec, 4);

            // clearing gpeds reg
            *(gpeds0) = 0x00010000;
            test;
        }

		usleep(60000);
    }
}