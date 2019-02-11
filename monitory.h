#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include "monitor.h"

#define BUFFER_SIZE 9
#define FIRST_R 1
#define SECOND_R 2
#define TRUE 1
#define FALSE 0
using namespace std;

class Buffer: public Monitor{
private:
    Condition full, empty1, empty2, rtr1, rtr2, rtc;
    int count, readyToConsume, wasRead1, wasRead2, em1, em2, r1, r2, ifConsumedThenSkip;

public:
    Buffer()
    {
        count = 0;
        readyToConsume = 0;
        wasRead1 = FALSE;
        wasRead2 = FALSE;
        em1 = 1;
        em2 = 1;
        r1 = 1;
        r2 = 1;
        ifConsumedThenSkip = 0;
    }

    void Producer();
    void Consumer();
    void Reader(int reader);
};

void Buffer::Producer()
{
    enter();
    if (count == BUFFER_SIZE)
    {
        printf("Producer czeka na full\n");
        wait(full);
    }
    count++;
    printf("An item has been added to the buffer. Count: %d\n",count);
    if (count == 1)
    {
            printf("Producer sygnalizuje empty(obydwa\n");
            if (em1 == 1)
            {
                em1 = 0;
                signal(empty1);
            }
            if (em2 == 1)
            {
                em2 = 0;
                signal(empty2);
            }
    }
    leave();
}

void Buffer::Consumer()
{
    enter();
    if (readyToConsume == 0)
    {
            printf("Consumer czeka na rtc\n");
            wait(rtc);
    }
    count--;
    ifConsumedThenSkip = TRUE;
    printf("An item has been removed from the buffer. Count: %d\n",count);
    readyToConsume = 0;
    if (count == BUFFER_SIZE-1)
    {
        printf("Consumer daje signal na full \n");
         signal(full);
    } 
    printf("Consumer daje signal na rtr i ustawia wasReady na FALSE \n");
    wasRead1 = FALSE;
    wasRead2 = FALSE;
    if (r1 == 1)
        {
            printf("sygnal faktyczny na rtr1\n");
            r1 = 0;
            signal(rtr1);
        }
        
    if (r2 == 1)
    {
        printf("sygnal faktyczny na rtr2\n");
        r2 = 0;
        signal(rtr2);
    }
    leave();
}

void Buffer::Reader(int reader)
{
    enter();
    
    if (reader == FIRST_R)
    {
        if (count == 0)
        {
            printf("Reader%d czeka na empty.\n",reader);
            em1 = 1;
            wait(empty1);
        }
        if (wasRead1 == TRUE)
        {
            printf("Reader1 czeka na rtr1.\n");
            r1 = 1;
            wait(rtr1);
        }      
        else
        {
            printf("Reader1 ustawia wasRead1 na TRUE \n"); 
            wasRead1 = TRUE;  
        }
        
    }
    if (reader == SECOND_R)
    {
        if (count == 0)
        {
            printf("Reader%d czeka na empty.\n",reader);
            em2 = 1;
            wait(empty2);
        }
        if (wasRead2 == TRUE)
           {
                printf("Reader2 czeka na rtr2.\n");
                r2 = 1;
                wait(rtr2);
           }
        else
            {
                printf("Reader2 ustawia wasRead2 na TRUE \n");
                wasRead2 = TRUE;
            }
    }
            //tu warunek ze zmienna dzielona
    if (!ifConsumedThenSkip)
        readyToConsume ++;
    ifConsumedThenSkip = FALSE;
    if (readyToConsume == 1)
        signal(rtc);

    leave();
}
