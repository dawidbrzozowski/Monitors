#include "monitory.h"
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
#include <iostream>
#include <thread>
#define N 1000 //How many times
Buffer buffer;
int i;
void produce()
{
    for (i = 0; i < N; i++)
    {
         printf("Iteration: %d\n",i);
        sleep(0.5);
        buffer.Producer();
    }
}

void consume()
{
    while(TRUE)
    {
        if (i == N)
            return;
        sleep(0.5);
        buffer.Consumer();
    }
}

void read1()
{
    while(TRUE)
    {
        if (i == N)
            return;
        sleep(0.5);
        buffer.Reader(FIRST_R);
    }
}
void read2()
{
    while(TRUE)
    {
        if (i == N)
            return;
        sleep(0.5);
        buffer.Reader(SECOND_R);
    }
}

int main()
{
    thread p(produce);
    thread c(consume);
    thread r1(read1);
    thread r2(read2);

    p.join();
    c.join();
    r2.join();
    r1.join();
    printf("End\n");
    return 0;
}