/* lab4.c */
// this is version 1 and is working


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sched.h>


int main(void)
{
    // open /tmp/N_pipe1
    int gpsd = open("/tmp/N_pipe1", O_RDONLY);
    char gpsBuffer;
    while(1)
    {
        // part 1 read ... (tmp/N_pipe1)
        read(gpsd, &gpsBuffer, 1);

        // create timeStamp

        printf("Current Buffer: %d \n", gpsBuffer);

        // wait 250 ms?
        usleep(250000);
    }


    return 0;
}