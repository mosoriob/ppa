#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#define MASTER 0
#define TPOINTS 800
#define MAXSTEPS  10000
#define PI 3.14159265
#define NUMTASKS 3

double  etime,                /* elapsed time in seconds */
  values[TPOINTS+2],  /* values at time t */
  oldval[TPOINTS+2],  /* values at time (t-dt) */
  newval[TPOINTS+2];  /* values at time (t+dt) */


typedef struct thread {   
     pthread_t thread_id;       
     int       thread_num;
     int       thread_step;
     int       thread_first;
} ThreadData;

void *thread_start(void *thread)
{
 ThreadData *my_data  = (ThreadData*)thread;
 int id=my_data->thread_num;
 int step=my_data->thread_step;
 int first=my_data->thread_first;
 int left, right;
 int j;
 double x, fac = 2.0 * PI;

  for (j=first; j <= (first+step); j++){
    x = (double)j/(double)(TPOINTS - 1);
    values[j] = sin (fac * x);
  } 

  if (id == NUMTASKS-1)
    right = 0;
  else
    right = id + 1;

  if (id == 0)
   left = NUMTASKS - 1;
  else
    left = id - 1;

 printf("thread %d: step=%d, left: %d, right: %d \n", id, step, left, right);
 return NULL;
}

int main(int argc, char *argv[])
{
  int i;
  ThreadData thread[NUMTASKS]; 
  int nmin = TPOINTS/NUMTASKS;
  int nleft = TPOINTS%NUMTASKS;
  int k=0, npoints;
  int first, npts;


  for(i=0; i<NUMTASKS; i++)
  {
    npts = (i < nleft) ? nmin + 1 : nmin;
    first = k + 1;
    npoints = npts;
    k += npts;
    thread[i].thread_num=i;
    thread[i].thread_step=npoints;
    thread[i].thread_first=first;
    pthread_create(&(thread[i].thread_id), NULL, thread_start, (void *)(thread+i));
  }

  //wait for all threads
  for (i = 0; i < NUMTASKS; i++)
     pthread_join(thread[i].thread_id, NULL); 
  for (i = 0; i < TPOINTS; i++)
    printf("%f \n", values[i]);
  //for (i = 0; i < NUMTASKS; i++)
  //   printf(" %i)thread: number %i\n",i,thread[i].thread_num);
  return 0;
}